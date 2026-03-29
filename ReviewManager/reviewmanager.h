#ifndef REVIEW_MANAGER_H
#define REVIEW_MANAGER_H

#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <fstream>
#include <vector>
#include <tuple>
#include <iostream>
#include "../parser/parser.h"
#include "../data_structures/graph.h"

class ReviewManager {
private:
    Graph<int> g;
    AssignmentParams params;
    ControlSettings control;
    Parser parser;

    /**
     * @brief Identifica revisores que são verdadeiramente indispensáveis para o MinReviews.
     */
public:
    std::vector<int> findRiskyReviewers() {
        std::vector<int> riskyReviewers;
        std::map<int, std::vector<int>> potentialCandidates;

        for (auto v : g.getVertexSet()) {
            if (v->type == VertexType::SUBMISSION) {
                for (auto e : v->getAdj()) {
                    if (e->getDest()->type == VertexType::REVIEWER) {
                        potentialCandidates[v->getInfo()].push_back(e->getDest()->getInfo());
                    }
                }
            }
        }

        std::set<int> activeReviewers;
        for (auto v : g.getVertexSet()) {
            if (v->type == VertexType::SUBMISSION) {
                for (auto e : v->getAdj()) {
                    if (e->getFlow() > 0 && e->getDest()->type == VertexType::REVIEWER) {
                        activeReviewers.insert(e->getDest()->getInfo());
                    }
                }
            }
        }

        for (int revId : activeReviewers) {
            bool isMandatory = false;
            for (auto const& [paperId, candidates] : potentialCandidates) {
                int countWithoutHim = 0;
                for (int candId : candidates) {
                    if (candId != revId) countWithoutHim++;
                }
                if (countWithoutHim < params.minReviews) {
                    isMandatory = true;
                    break;
                }
            }
            if (isMandatory) riskyReviewers.push_back(revId - 1000);
        }
        std::sort(riskyReviewers.begin(), riskyReviewers.end());
        return riskyReviewers;
    }
    void load(const std::string &filename) {
        parser.loadData(filename, g, params, control);
    }

    void prepareNetwork() {
        for (auto v : g.getVertexSet()) v->removeOutgoingEdges();
        parser.buildNetwork(g, params);
    }

    void runAssignments() {
        edmondsKarp(&g, 0, 9999);
    }

    // --- MÉTODOS REQUERIDOS PELO MENU ---

    void printParameters() {
        std::cout << "\n--- Current Configuration ---\n";
        std::cout << "Min Reviews / Paper: " << params.minReviews << "\n";
        std::cout << "Max Reviews / Reviewer: " << params.maxReviews << "\n";
        std::cout << "Primary Expertise Rule: " << (params.usePrimaryRev ? "ON" : "OFF") << "\n";
        std::cout << "Output File: " << control.outputFileName << "\n";
    }

    void showAssignments() {
        std::cout << "\n--- Final Paper Assignments ---\n";
        bool found = false;
        for (auto v : g.getVertexSet()) {
            if (v->type == VertexType::SUBMISSION) {
                for (auto edge : v->getAdj()) {
                    if (edge->getFlow() > 0 && edge->getDest()->type == VertexType::REVIEWER) {
                        std::cout << "Paper " << v->getInfo() << " [" << v->title << "] assigned to -> "
                                  << "Reviewer " << edge->getDest()->getInfo() - 1000 << "\n";
                        found = true;
                    }
                }
            }
        }
        if (!found) std::cout << "No assignments found.\n";
    }

    void printEdges() {
        std::cout << "\n--- Graph Edges ---\n";
        for (auto v : g.getVertexSet()) {
            for (auto e : v->getAdj()) {
                std::cout << "ID " << v->getInfo() << " --(" << e->getWeight() << ")--> ID " << e->getDest()->getInfo() << "\n";
            }
        }
    }

    // --- MÉTODOS DE BATCH (FORMATO CSV) ---

    void saveBatchResults(const std::string &filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) return;

        auto getMatchedDomain = [this](auto sub, auto rev) -> int {
            if (sub->primaryDomain == rev->primaryDomain) return sub->primaryDomain;
            if (params.useSecondaryRev && sub->primaryDomain == rev->secondaryDomain) return sub->primaryDomain;
            if (params.useSecondarySub && sub->secondaryDomain == rev->primaryDomain) return sub->secondaryDomain;
            if (params.useSecondarySub && params.useSecondaryRev && sub->secondaryDomain == rev->secondaryDomain && sub->secondaryDomain != -1) return sub->secondaryDomain;
            return -1;
        };

        // 1. Sub -> Rev
        outFile << "#SubmissionId,ReviewerId,Match\n";
        int total = 0;
        std::vector<std::tuple<int, int, int>> revMap;
        for (auto v : g.getVertexSet()) {
            if (v->type == VertexType::SUBMISSION) {
                for (auto e : v->getAdj()) {
                    if (e->getFlow() > 0 && e->getDest()->type == VertexType::REVIEWER) {
                        int m = getMatchedDomain(v, e->getDest());
                        outFile << v->getInfo() << ", " << (e->getDest()->getInfo() - 1000) << ", " << m << "\n";
                        revMap.push_back({e->getDest()->getInfo() - 1000, v->getInfo(), m});
                        total++;
                    }
                }
            }
        }

        // 2. Rev -> Sub (Sorted)
        outFile << "#ReviewerId,SubmissionId,Match\n";
        std::sort(revMap.begin(), revMap.end());
        for (auto const& [rid, sid, m] : revMap) outFile << rid << ", " << sid << ", " << m << "\n";

        // 3. Total e Risco
        outFile << "#Total: " << total << "\n";
        if (control.riskAnalysis > 0) {
            outFile << "#Risk Analysis: " << control.riskAnalysis << "\n";
            std::vector<int> risky = findRiskyReviewers();
            for (size_t i = 0; i < risky.size(); ++i) {
                outFile << risky[i] << (i < risky.size() - 1 ? ", " : "");
            }
            outFile << "\n";
        }

        // 4. Missing Reviews
        bool first = true;
        for (auto v : g.getVertexSet()) {
            if (v->type == VertexType::SUBMISSION) {
                int count = 0;
                for (auto e : v->getAdj()) if (e->getFlow() > 0 && e->getDest()->type == VertexType::REVIEWER) count++;
                if (count < params.minReviews) {
                    if (first) { outFile << "#SubmissionId,Domain,MissingReviews\n"; first = false; }
                    outFile << v->getInfo() << ", " << v->primaryDomain << ", " << (params.minReviews - count) << "\n";
                }
            }
        }
        outFile.close();
    }

    Graph<int>& getGraph() { return g; }
    ControlSettings& getControl() { return control; }
    AssignmentParams& getParams() { return params; }
};

#endif