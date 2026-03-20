#ifndef PARSER_H
#define PARSER_H

#include "../data_structures/graph.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

// Full definitions of the structs so the compiler can access their members
struct AssignmentParams {
    int minReviews = 0;
    int maxReviews = 0;
    int usePrimaryRev = 1;
    int useSecondaryRev = 0;
    int usePrimarySub = 1;
    int useSecondarySub = 0;
};

struct ControlSettings {
    int generateAssignments = 0;
    int riskAnalysis = 0;
    std::string outputFileName = "output.csv";
};

class Parser {
public:
    // Helper to clean up strings from the CSV
    std::string trim(const std::string& s) {
        size_t first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, (last - first + 1));
    }

    void loadData(const std::string &filename, Graph<int> &g, AssignmentParams &params, ControlSettings &control) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }

        std::string line, section = "NONE";

        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty()) continue;

            // 1. Detect Section Headers
            if (line[0] == '#') {
                std::string header = trim(line.substr(1));
                // If the line is JUST '#' or has no text after, skip it
                if (header.empty()) continue;

                // Check for labels (Id, Title, etc.) - we skip these
                if (header.find("Id") == 0 || header.find("Name") == 0 || header.find("Title") == 0) continue;

                // Set the current section context
                if (header.find("Submissions") != std::string::npos) section = "SUBMISSIONS";
                else if (header.find("Reviewers") != std::string::npos) section = "REVIEWERS";
                else if (header.find("Parameters") != std::string::npos) section = "PARAMETERS";
                else if (header.find("Control") != std::string::npos) section = "CONTROL";
                continue; 
            }

            // 2. Split Data Row
            std::stringstream ss(line);
            std::string cell;
            std::vector<std::string> row;
            while (std::getline(ss, cell, ',')) {
                row.push_back(trim(cell));
            }

            // 3. Process based on Section
            try {
                if (section == "SUBMISSIONS" && row.size() >= 5) {
                    int id = std::stoi(row[0]);
                    g.addVertex(id);
                    auto v = g.findVertex(id);
                    v->type = VertexType::SUBMISSION;
                    v->title = row[1];
                    v->primaryDomain = std::stoi(row[4]);
                    v->secondaryDomain = (row.size() > 5 && !row[5].empty()) ? std::stoi(row[5]) : -1;
                } 
                else if (section == "REVIEWERS" && row.size() >= 4) {
                    int id = std::stoi(row[0]);
                    g.addVertex(id);
                    auto v = g.findVertex(id);
                    v->type = VertexType::REVIEWER;
                    v->title = row[1];
                    v->primaryDomain = std::stoi(row[3]);
                    v->secondaryDomain = (row.size() > 4 && !row[4].empty()) ? std::stoi(row[4]) : -1;
                } 
                else if (section == "PARAMETERS" && row.size() >= 2) {
                    std::string key = row[0];
                    int val = std::stoi(row[1]);
                    if (key == "MinReviewsPerSubmission") params.minReviews = val;
                    else if (key == "MaxReviewsPerReviewer") params.maxReviews = val;
                    else if (key == "PrimaryReviewerExpertise") params.usePrimaryRev = val;
                    else if (key == "SecondaryReviewerExpertise") params.useSecondaryRev = val;
                    else if (key == "PrimarySubmissionDomain") params.usePrimarySub = val;
                    else if (key == "SecondarySubmissionDomain") params.useSecondarySub = val;
                }
                else if (section == "CONTROL" && row.size() >= 2) {
                    std::string key = row[0];
                    if (key == "GenerateAssignments") control.generateAssignments = std::stoi(row[1]);
                    else if (key == "RiskAnalysis") control.riskAnalysis = std::stoi(row[1]);
                    else if (key == "OutputFileName") control.outputFileName = row[1];
                }
            } catch (...) {
                // Ignore rows that don't match the expected format
                continue;
            }
        }
        file.close();
    }

    void buildNetwork(Graph<int> &g, const AssignmentParams &p) {
        // Clear or setup Source/Sink
        g.addVertex(0);
        g.addVertex(9999);
        auto s = g.findVertex(0); s->type = VertexType::SOURCE;
        auto t = g.findVertex(9999); t->type = VertexType::SINK;

        auto nodes = g.getVertexSet();
        for (auto v : nodes) {
            if (v->type == VertexType::SUBMISSION) {
                // Edge from Source to Submission with capacity = minReviews
                s->addEdge(v, p.minReviews);
                
                // Potential edges to Reviewers
                for (auto rev : nodes) {
                    if (rev->type == VertexType::REVIEWER) {
                        // Basic T2.1 Rule: Match on Primary Domains
                        if (v->primaryDomain == rev->primaryDomain) {
                            v->addEdge(rev, 1);
                        }
                    }
                }
            } 
            else if (v->type == VertexType::REVIEWER) {
                // Edge from Reviewer to Sink with capacity = maxReviews
                v->addEdge(t, p.maxReviews);
            }
        }
    }
};

#endif