/**
 * @file Parser.h
 * @brief CSV parser and flow network builder for the submission-reviewer assignment problem.
 * @brief (PT) Parser de CSV e construtor de rede de fluxo para o problema de atribuição submissão-revisor.
 *
 * @details
 * (EN) This header defines the data structures and Parser class responsible for:
 *  - Reading a structured CSV input file containing submissions, reviewers, algorithm parameters, and control settings.
 *  - Populating a Graph<int> with the corresponding vertices and metadata.
 *  - Building the flow network (source → submissions → reviewers → sink) used by Edmonds-Karp
 *    to compute an optimal reviewer assignment.
 *
 * (PT) Este ficheiro define as estruturas de dados e a classe Parser responsáveis por:
 *  - Ler um ficheiro CSV estruturado com submissões, revisores, parâmetros do algoritmo e definições de controlo.
 *  - Popular um Graph<int> com os vértices e metadados correspondentes.
 *  - Construir a rede de fluxo (fonte → submissões → revisores → sumidouro) usada pelo Edmonds-Karp
 *    para calcular uma atribuição ótima de revisores.
 *
 * @author DA Project Group
 * @date 2026
 */

#ifndef PARSER_H
#define PARSER_H

#include "../data_structures/graph.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

/**
 * @struct AssignmentParams
 * @brief (EN) Holds algorithm parameters that control how reviewer-submission matching is performed.
 * @brief (PT) Contém os parâmetros do algoritmo que controlam como é feita a correspondência revisor-submissão.
 *
 * @details
 * (EN) These parameters are read from the [Parameters] section of the CSV input file.
 * They determine the minimum number of reviews each submission must receive, the maximum
 * workload per reviewer, and which domain expertise combinations are considered valid matches.
 *
 * (PT) Estes parâmetros são lidos da secção [Parameters] do ficheiro CSV de entrada.
 * Determinam o número mínimo de revisões que cada submissão deve receber, a carga máxima
 * por revisor, e quais combinações de domínio de especialização são consideradas correspondências válidas.
 */
struct AssignmentParams {
    /**
     * @brief (EN) Minimum number of reviews required per submission.
     * @brief (PT) Número mínimo de revisões necessárias por submissão.
     * @note (EN) Used as the capacity of source→submission edges. / (PT) Usado como capacidade das arestas fonte→submissão.
     */
    int minReviews = 0;

    /**
     * @brief (EN) Maximum number of submissions a single reviewer can be assigned.
     * @brief (PT) Número máximo de submissões que um único revisor pode receber.
     * @note (EN) Used as the capacity of reviewer→sink edges. / (PT) Usado como capacidade das arestas revisor→sumidouro.
     */
    int maxReviews = 0;

    /**
     * @brief (EN) Whether a reviewer's primary domain can match a submission's primary domain (Case A).
     * @brief (PT) Se o domínio primário do revisor pode corresponder ao domínio primário da submissão (Caso A).
     * @note Default: 1 (enabled). / (PT) Por defeito: 1 (ativo).
     */
    int usePrimaryRev = 1;

    /**
     * @brief (EN) Whether a reviewer's secondary domain can match a submission's primary domain (Case B).
     * @brief (PT) Se o domínio secundário do revisor pode corresponder ao domínio primário da submissão (Caso B).
     * @note Default: 0 (disabled). / (PT) Por defeito: 0 (inativo).
     */
    int useSecondaryRev = 0;

    /**
     * @brief (EN) Whether a submission's secondary domain can match a reviewer's primary domain (Case C).
     * @brief (PT) Se o domínio secundário da submissão pode corresponder ao domínio primário do revisor (Caso C).
     * @note Default: 1 (enabled). / (PT) Por defeito: 1 (ativo).
     */
    int usePrimarySub = 1;

    /**
     * @brief (EN) Whether a submission's secondary domain can match a reviewer's secondary domain (Case D).
     * @brief (PT) Se o domínio secundário da submissão pode corresponder ao domínio secundário do revisor (Caso D).
     * @note Default: 0 (disabled). / (PT) Por defeito: 0 (inativo).
     */
    int useSecondarySub = 0;
};

/**
 * @struct ControlSettings
 * @brief (EN) Holds runtime control flags and output configuration read from the CSV.
 * @brief (PT) Contém flags de controlo de execução e configuração de saída lidos do CSV.
 *
 * @details
 * (EN) These settings are read from the [Control] section of the CSV input file and
 * determine which operations are performed and where the results are written.
 *
 * (PT) Estas definições são lidas da secção [Control] do ficheiro CSV de entrada e
 * determinam quais operações são executadas e onde os resultados são escritos.
 */
struct ControlSettings {
    /**
     * @brief (EN) If non-zero, the assignment algorithm is run and results are written to output.
     * @brief (PT) Se diferente de zero, o algoritmo de atribuição é executado e os resultados são escritos.
     */
    int generateAssignments = 0;

    /**
     * @brief (EN) If non-zero, a risk analysis is performed on the resulting assignments.
     * @brief (PT) Se diferente de zero, é realizada uma análise de risco sobre as atribuições resultantes.
     */
    int riskAnalysis = 0;

    /**
     * @brief (EN) Name of the output CSV file where assignments will be written.
     * @brief (PT) Nome do ficheiro CSV de saída onde as atribuições serão escritas.
     */
    std::string outputFileName = "output.csv";
};

/**
 * @class Parser
 * @brief (EN) Reads a structured CSV file and builds the graph and configuration for the assignment problem.
 * @brief (PT) Lê um ficheiro CSV estruturado e constrói o grafo e a configuração para o problema de atribuição.
 *
 * @details
 * (EN) The Parser class provides two main operations:
 *  - loadData(): parses the CSV file and populates the graph with submission and reviewer vertices,
 *    along with filling in AssignmentParams and ControlSettings.
 *  - buildNetwork(): constructs the flow network by adding source/sink nodes and connecting
 *    submissions to compatible reviewers based on domain matching rules.
 *
 * The expected CSV format uses '#' as section headers:
 * @code
 * #Submissions
 * #Id, Title, ..., PrimaryDomain, SecondaryDomain
 * 1, Paper A, ..., 3, 7
 *
 * #Reviewers
 * #Id, Name, ..., PrimaryDomain, SecondaryDomain
 * 101, Alice, ..., 3, -1
 *
 * #Parameters
 * MinReviewsPerSubmission, 2
 * MaxReviewsPerReviewer, 3
 *
 * #Control
 * GenerateAssignments, 1
 * OutputFileName, result.csv
 * @endcode
 *
 * (PT) A classe Parser fornece duas operações principais:
 *  - loadData(): analisa o ficheiro CSV e povoa o grafo com vértices de submissão e revisor,
 *    preenchendo também AssignmentParams e ControlSettings.
 *  - buildNetwork(): constrói a rede de fluxo adicionando nós fonte/sumidouro e ligando
 *    submissões a revisores compatíveis com base em regras de correspondência de domínio.
 *
 * O formato CSV esperado usa '#' como cabeçalhos de secção (ver exemplo acima).
 */
class Parser {
public:
    /**
     * @brief (EN) Trims leading and trailing whitespace characters from a string.
     * @brief (PT) Remove espaços em branco no início e fim de uma string.
     *
     * @details
     * (EN) Removes spaces, tabs, carriage returns, and newlines from both ends.
     * Used internally to clean up cells read from the CSV file.
     *
     * (PT) Remove espaços, tabulações, retornos de carro e newlines de ambos os extremos.
     * Usado internamente para limpar células lidas do ficheiro CSV.
     *
     * @param s Input string. / (PT) String de entrada.
     * @return Trimmed string. / (PT) String sem espaços nas extremidades.
     */
    std::string trim(const std::string& s) {
        size_t first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, (last - first + 1));
    }

    /**
     * @brief (EN) Parses the CSV input file and populates the graph, parameters, and control settings.
     * @brief (PT) Analisa o ficheiro CSV de entrada e povoa o grafo, os parâmetros e as definições de controlo.
     *
     * @details
     * (EN) Reads the file line by line, detecting section headers (lines starting with '#').
     * Each section is processed differently:
     *  - SUBMISSIONS: creates a vertex with VertexType::SUBMISSION and assigns title and domain fields.
     *  - REVIEWERS: creates a vertex with VertexType::REVIEWER and assigns title and domain fields.
     *  - PARAMETERS: fills in the AssignmentParams struct (minReviews, maxReviews, domain flags).
     *  - CONTROL: fills in the ControlSettings struct (generateAssignments, riskAnalysis, outputFileName).
     * Malformed rows are silently skipped via a catch-all exception handler.
     *
     * (PT) Lê o ficheiro linha a linha, detetando cabeçalhos de secção (linhas que começam com '#').
     * Cada secção é processada de forma diferente:
     *  - SUBMISSIONS: cria um vértice com VertexType::SUBMISSION e atribui título e domínios.
     *  - REVIEWERS: cria um vértice com VertexType::REVIEWER e atribui título e domínios.
     *  - PARAMETERS: preenche o struct AssignmentParams (minReviews, maxReviews, flags de domínio).
     *  - CONTROL: preenche o struct ControlSettings (generateAssignments, riskAnalysis, outputFileName).
     * Linhas mal formatadas são silenciosamente ignoradas via handler de exceção genérico.
     *
     * @param filename Path to the CSV input file. / (PT) Caminho para o ficheiro CSV de entrada.
     * @param g Graph to populate with submission and reviewer vertices. / (PT) Grafo a popular com vértices de submissão e revisor.
     * @param params AssignmentParams struct to fill. / (PT) Struct AssignmentParams a preencher.
     * @param control ControlSettings struct to fill. / (PT) Struct ControlSettings a preencher.
     */
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
                if (header.empty()) continue;

                // Skip column label lines (Id, Name, Title, ...)
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
                // (PT) Ignora linhas que não correspondem ao formato esperado
                continue;
            }
        }
        file.close();
    }

    /**
     * @brief (EN) Constructs the flow network from the loaded graph vertices and assignment parameters.
     * @brief (PT) Constrói a rede de fluxo a partir dos vértices carregados e dos parâmetros de atribuição.
     *
     * @details
     * (EN) Adds a super-source (id=0) and super-sink (id=9999) to the graph, then wires the network:
     *  - source → submission: capacity = minReviews (each paper needs at least this many reviews).
     *  - submission → reviewer: capacity = 1, added only when the reviewer is a domain match.
     *  - reviewer → sink: capacity = maxReviews (limits the total workload per reviewer).
     *
     * Domain matching between a submission and a reviewer follows four prioritised cases:
     *  - Case A (always enabled): submission.primaryDomain == reviewer.primaryDomain.
     *  - Case B (if useSecondaryRev): submission.primaryDomain == reviewer.secondaryDomain.
     *  - Case C (if useSecondarySub): submission.secondaryDomain == reviewer.primaryDomain.
     *  - Case D (if useSecondarySub && useSecondaryRev): both secondary domains match and are not -1.
     *
     * (PT) Adiciona um super-fonte (id=0) e um super-sumidouro (id=9999) ao grafo e liga a rede:
     *  - fonte → submissão: capacidade = minReviews (cada artigo precisa de pelo menos este número de revisões).
     *  - submissão → revisor: capacidade = 1, adicionada apenas quando o revisor é compatível por domínio.
     *  - revisor → sumidouro: capacidade = maxReviews (limita a carga total por revisor).
     *
     * A correspondência de domínios entre submissão e revisor segue quatro casos por prioridade:
     *  - Caso A (sempre ativo): submission.primaryDomain == reviewer.primaryDomain.
     *  - Caso B (se useSecondaryRev): submission.primaryDomain == reviewer.secondaryDomain.
     *  - Caso C (se useSecondarySub): submission.secondaryDomain == reviewer.primaryDomain.
     *  - Caso D (se useSecondarySub && useSecondaryRev): ambos os domínios secundários coincidem e não são -1.
     *
     * @param g Graph already populated with submission and reviewer vertices via loadData().
     *          (PT) Grafo já populado com vértices de submissão e revisor via loadData().
     * @param p Assignment parameters controlling capacity and domain matching rules.
     *          (PT) Parâmetros de atribuição que controlam capacidade e regras de correspondência de domínio.
     */
    void buildNetwork(Graph<int> &g, const AssignmentParams &p) {
        // 1. Add super-source (0) and super-sink (9999)
        // (PT) Adiciona super-fonte (0) e super-sumidouro (9999)
        g.addVertex(0);
        g.addVertex(9999);
        auto s = g.findVertex(0);
        auto t = g.findVertex(9999);
        s->type = VertexType::SOURCE;
        t->type = VertexType::SINK;

        auto nodes = g.getVertexSet();
        for (auto v : nodes) {
            if (v->type == VertexType::SUBMISSION) {
                // Each paper needs 'minReviews' reviews → source edge capacity
                // (PT) Cada artigo precisa de 'minReviews' revisões → capacidade da aresta fonte
                s->addEdge(v, p.minReviews);
                
                for (auto rev : nodes) {
                    if (rev->type == VertexType::REVIEWER) {
                        bool isMatch = false;

                        // Case A: Primary to Primary (gold standard match)
                        // (PT) Caso A: Primário para Primário (correspondência ideal)
                        if (v->primaryDomain == rev->primaryDomain) {
                            isMatch = true;
                        }
                        // Case B: Primary submission domain vs Secondary reviewer domain
                        // (PT) Caso B: Domínio primário da submissão vs domínio secundário do revisor
                        else if (p.useSecondaryRev && v->primaryDomain == rev->secondaryDomain) {
                            isMatch = true;
                        }
                        // Case C: Secondary submission domain vs Primary reviewer domain
                        // (PT) Caso C: Domínio secundário da submissão vs domínio primário do revisor
                        else if (p.useSecondarySub && v->secondaryDomain == rev->primaryDomain) {
                            isMatch = true;
                        }
                        // Case D: Secondary submission domain vs Secondary reviewer domain
                        // (PT) Caso D: Domínio secundário da submissão vs domínio secundário do revisor
                        else if (p.useSecondarySub && p.useSecondaryRev && 
                                 v->secondaryDomain == rev->secondaryDomain && v->secondaryDomain != -1) {
                            isMatch = true;
                        }

                        if (isMatch) {
                            // Capacity = 1: one reviewer handles one paper at a time
                            // (PT) Capacidade = 1: um revisor trata de um artigo de cada vez
                            v->addEdge(rev, 1);
                        }
                    }
                }
            } 
            else if (v->type == VertexType::REVIEWER) {
                // Reviewer workload cap: limits total submissions assigned
                // (PT) Limite de carga do revisor: limita o total de submissões atribuídas
                v->addEdge(t, p.maxReviews);
            }
        }
    }
};

#endif