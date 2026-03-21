/**
 * @file ReviewManager.h
 * @brief High-level orchestrator for the submission-reviewer assignment workflow.
 * @brief (PT) Orquestrador de alto nível para o fluxo de trabalho de atribuição submissão-revisor.
 *
 * @details
 * (EN) This header defines the ReviewManager class, which owns the graph, parameters, and
 * control settings, and coordinates the full pipeline:
 *  1. Load data from a CSV file (via Parser).
 *  2. Build the flow network (source → submissions → reviewers → sink).
 *  3. Run the Edmonds-Karp max-flow algorithm to compute optimal assignments.
 *  4. Display results, configuration, and graph structure to stdout.
 *
 * (PT) Este ficheiro define a classe ReviewManager, que detém o grafo, os parâmetros e as
 * definições de controlo, e coordena o pipeline completo:
 *  1. Carregar dados de um ficheiro CSV (via Parser).
 *  2. Construir a rede de fluxo (fonte → submissões → revisores → sumidouro).
 *  3. Executar o algoritmo de fluxo máximo de Edmonds-Karp para calcular as atribuições ótimas.
 *  4. Apresentar resultados, configuração e estrutura do grafo no stdout.
 *
 * @author DA Project Group
 * @date 2026
 */

#ifndef REVIEW_MANAGER_H
#define REVIEW_MANAGER_H

#include <iomanip>
#include "../parser/parser.h"
#include "../data_structures/graph.h"

/**
 * @class ReviewManager
 * @brief (EN) Owns and coordinates all components of the reviewer assignment system.
 * @brief (PT) Detém e coordena todos os componentes do sistema de atribuição de revisores.
 *
 * @details
 * (EN) ReviewManager acts as the single point of control for the assignment pipeline.
 * It holds the Graph<int> (the flow network), the AssignmentParams (matching rules),
 * the ControlSettings (output and execution flags), and a Parser instance.
 * The typical usage sequence is:
 * @code
 * ReviewManager rm;
 * rm.load("input.csv");       // Parse CSV into graph + params + control
 * rm.prepareNetwork();        // Wire source/sink and domain-matching edges
 * rm.runAssignments();        // Run Edmonds-Karp max-flow
 * rm.showAssignments();       // Print the resulting reviewer assignments
 * @endcode
 *
 * (PT) O ReviewManager atua como ponto único de controlo do pipeline de atribuição.
 * Detém o Graph<int> (a rede de fluxo), os AssignmentParams (regras de correspondência),
 * os ControlSettings (flags de saída e execução) e uma instância de Parser.
 * A sequência de utilização típica é (ver exemplo acima).
 */
class ReviewManager {
private:
    /**
     * @brief (EN) The flow network graph holding all submission and reviewer vertices.
     * @brief (PT) O grafo da rede de fluxo com todos os vértices de submissão e revisor.
     */
    Graph<int> g;

    /**
     * @brief (EN) Algorithm parameters controlling domain matching and review count limits.
     * @brief (PT) Parâmetros do algoritmo que controlam a correspondência de domínios e os limites de revisão.
     */
    AssignmentParams params;

    /**
     * @brief (EN) Runtime control flags (which operations to run, output file name).
     * @brief (PT) Flags de controlo de execução (quais operações executar, nome do ficheiro de saída).
     */
    ControlSettings control;

    /**
     * @brief (EN) Parser instance used to read the CSV and build the network.
     * @brief (PT) Instância do Parser usada para ler o CSV e construir a rede.
     */
    Parser parser;

public:
    /**
     * @brief (EN) Loads data from a CSV file into the graph, parameters, and control settings.
     * @brief (PT) Carrega dados de um ficheiro CSV para o grafo, os parâmetros e as definições de controlo.
     *
     * @details
     * (EN) Delegates to Parser::loadData(). After this call, the graph contains all
     * submission and reviewer vertices with their metadata, and params/control are populated.
     * The flow network edges are NOT yet built — call prepareNetwork() afterwards.
     *
     * (PT) Delega para Parser::loadData(). Após esta chamada, o grafo contém todos os
     * vértices de submissão e revisor com os seus metadados, e params/control estão preenchidos.
     * As arestas da rede de fluxo ainda NÃO estão construídas — chame prepareNetwork() a seguir.
     *
     * @param filename Path to the CSV input file. / (PT) Caminho para o ficheiro CSV de entrada.
     */
    void load(const std::string &filename) {
        parser.loadData(filename, g, params, control);
    }

    /**
     * @brief (EN) Prints the current assignment parameters and control settings to stdout.
     * @brief (PT) Imprime os parâmetros de atribuição e definições de controlo atuais no stdout.
     *
     * @details
     * (EN) Outputs a formatted table with three sections:
     *  - Assignment rules: minimum reviews per paper and maximum reviews per reviewer.
     *  - Logic flags: which domain expertise combinations are enabled.
     *  - Control settings: output filename and whether assignment generation is active.
     *
     * (PT) Imprime uma tabela formatada com três secções:
     *  - Regras de atribuição: revisões mínimas por artigo e máximas por revisor.
     *  - Flags de lógica: quais combinações de especialização de domínio estão ativas.
     *  - Definições de controlo: nome do ficheiro de saída e se a geração de atribuições está ativa.
     */
    void printParameters() {
        std::cout << "\n--- Current Configuration (Parameters) ---\n";
        std::cout << std::left << std::setw(30) << "Parameter" << "Value" << std::endl;
        std::cout << std::string(40, '-') << std::endl;

        // Assignment rules / (PT) Regras de atribuição
        std::cout << std::setw(30) << "Min Reviews / Paper:" << params.minReviews << std::endl;
        std::cout << std::setw(30) << "Max Reviews / Reviewer:" << params.maxReviews << std::endl;

        // Domain expertise flags / (PT) Flags de especialização de domínio
        std::cout << "\n--- Logic Flags (Expertise Rules) ---\n";
        std::cout << std::setw(30) << "Use Primary Reviewer Exp:" << (params.usePrimaryRev ? "YES" : "NO") << std::endl;
        std::cout << std::setw(30) << "Use Secondary Reviewer Exp:" << (params.useSecondaryRev ? "YES" : "NO") << std::endl;
        std::cout << std::setw(30) << "Use Primary Sub Domain:" << (params.usePrimarySub ? "YES" : "NO") << std::endl;
        std::cout << std::setw(30) << "Use Secondary Sub Domain:" << (params.useSecondarySub ? "YES" : "NO") << std::endl;

        // Control settings / (PT) Definições de controlo
        std::cout << "\n--- Control Settings ---\n";
        std::cout << std::setw(30) << "Output Filename:" << control.outputFileName << std::endl;
        std::cout << std::setw(30) << "Generate Assignments:" << (control.generateAssignments ? "ON" : "OFF") << std::endl;
    }

    /**
     * @brief (EN) Clears all existing edges and rebuilds the flow network from scratch.
     * @brief (PT) Remove todas as arestas existentes e reconstrói a rede de fluxo de raiz.
     *
     * @details
     * (EN) First removes all outgoing edges from every vertex (resetting the network state),
     * then delegates to Parser::buildNetwork() to wire source, sink, submission→reviewer,
     * and reviewer→sink edges according to the current AssignmentParams.
     * This must be called before runAssignments() whenever params change or after load().
     *
     * (PT) Primeiro remove todas as arestas de saída de cada vértice (reiniciando o estado da rede),
     * depois delega para Parser::buildNetwork() para ligar fonte, sumidouro, submissão→revisor
     * e revisor→sumidouro de acordo com os AssignmentParams atuais.
     * Deve ser chamado antes de runAssignments() sempre que os params mudem ou após load().
     */
    void prepareNetwork() {
        // Reset all edges before rebuilding / (PT) Reinicia todas as arestas antes de reconstruir
        for (auto v : g.getVertexSet()) {
            v->removeOutgoingEdges();
        }
        parser.buildNetwork(g, params);
    }

    /**
     * @brief (EN) Runs the Edmonds-Karp max-flow algorithm to compute reviewer assignments.
     * @brief (PT) Executa o algoritmo de fluxo máximo de Edmonds-Karp para calcular as atribuições de revisores.
     *
     * @details
     * (EN) Uses super-source (id=0) and super-sink (id=9999) as the flow endpoints.
     * After this call, each submission→reviewer edge with flow > 0 represents a confirmed assignment.
     * prepareNetwork() must be called before this method.
     *
     * (PT) Usa o super-fonte (id=0) e o super-sumidouro (id=9999) como extremos do fluxo.
     * Após esta chamada, cada aresta submissão→revisor com fluxo > 0 representa uma atribuição confirmada.
     * prepareNetwork() deve ser chamado antes deste método.
     */
    void runAssignments() {
        edmondsKarp(&g, 0, 9999);
    }

    /**
     * @brief (EN) Prints all confirmed reviewer-to-paper assignments to stdout.
     * @brief (PT) Imprime todas as atribuições confirmadas de revisor para artigo no stdout.
     *
     * @details
     * (EN) Iterates over all SUBMISSION vertices and their outgoing edges. An assignment
     * is confirmed when the edge leads to a REVIEWER vertex and its flow is greater than 0.
     * If no assignments are found, a hint message is printed instead.
     *
     * (PT) Itera sobre todos os vértices SUBMISSION e as suas arestas de saída. Uma atribuição
     * é confirmada quando a aresta leva a um vértice REVIEWER e o seu fluxo é maior que 0.
     * Se não forem encontradas atribuições, é impressa uma mensagem de aviso.
     */
    void showAssignments() {
        std::cout << "\n--- Final Paper Assignments ---\n";
        bool found = false;

        for (auto v : g.getVertexSet()) {
            if (v->type == VertexType::SUBMISSION) {
                for (auto edge : v->getAdj()) {
                    // flow > 0 means this submission-reviewer pair was matched
                    // (PT) flow > 0 significa que esta submissão-revisor foi emparelhada
                    if (edge->getFlow() > 0 && edge->getDest()->type == VertexType::REVIEWER) {
                        std::cout << "Paper " << v->getInfo() << " [" << v->title << "] assigned to -> "
                                  << "Reviewer " << edge->getDest()->getInfo()
                                  << " [" << edge->getDest()->title << "]\n";
                        found = true;
                    }
                }
            }
        }

        if (!found) {
            std::cout << "No assignments found. Did you run the algorithm yet?\n";
        }
    }

    /**
     * @brief (EN) Prints all edges in the graph with their capacities and node type labels.
     * @brief (PT) Imprime todas as arestas do grafo com as suas capacidades e rótulos de tipo de nó.
     *
     * @details
     * (EN) Iterates over every vertex and its outgoing edges, formatting each connection as:
     * @code
     * ID <src> [TYPE]  --(<capacity>)-->  ID <dest> [TYPE]
     * @endcode
     * Useful for debugging the network structure before running the flow algorithm.
     *
     * (PT) Itera sobre cada vértice e as suas arestas de saída, formatando cada ligação como
     * indicado no exemplo acima. Útil para depurar a estrutura da rede antes de correr o algoritmo.
     */
    void printEdges() {
        std::cout << "\n--- Graph Edges (Connections & Capacities) ---\n";
        auto vertices = g.getVertexSet();

        for (auto v : vertices) {
            for (auto e : v->getAdj()) {
                auto dest = e->getDest();

                std::cout << "ID " << v->getInfo();
                if (v->type == VertexType::SOURCE)     std::cout << " [SOURCE]";
                else if (v->type == VertexType::SUBMISSION) std::cout << " [PAPER]";
                else if (v->type == VertexType::REVIEWER)  std::cout << " [REV]";

                std::cout << "  --(" << e->getWeight() << ")-->  ";

                std::cout << "ID " << dest->getInfo();
                if (dest->type == VertexType::SINK)       std::cout << " [SINK]";
                else if (dest->type == VertexType::REVIEWER) std::cout << " [REV]";

                std::cout << std::endl;
            }
        }
    }

    // ---- Accessors ----

    /**
     * @brief (EN) Returns a reference to the internal flow network graph.
     * @brief (PT) Devolve uma referência para o grafo interno da rede de fluxo.
     * @return Reference to Graph<int>. / (PT) Referência para Graph<int>.
     */
    Graph<int>& getGraph() { return g; }

    /**
     * @brief (EN) Returns a reference to the runtime control settings.
     * @brief (PT) Devolve uma referência para as definições de controlo de execução.
     * @return Reference to ControlSettings. / (PT) Referência para ControlSettings.
     */
    ControlSettings& getControl() { return control; }

    /**
     * @brief (EN) Returns a reference to the assignment algorithm parameters.
     * @brief (PT) Devolve uma referência para os parâmetros do algoritmo de atribuição.
     * @return Reference to AssignmentParams. / (PT) Referência para AssignmentParams.
     */
    AssignmentParams& getParams() { return params; }
};

#endif