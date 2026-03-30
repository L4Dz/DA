// Path: DA2026_PRJ1_T<TN>_G<GN>/Code/src/menu/menu.h

/**
 * @file menu.h
 * @brief Interactive command-line menu for the Conference Management System.
 * @brief (PT) Menu interativo de linha de comandos para o Sistema de Gestão de Conferências.
 *
 * @details
 * (EN) This header defines the Menu class, which provides a simple text-based interface
 * that drives the full assignment pipeline through user input. It holds a reference to
 * a ReviewManager instance and dispatches each menu option to the appropriate manager method.
 *
 * (PT) Este ficheiro define a classe Menu, que fornece uma interface de texto simples
 * que conduz o pipeline completo de atribuição através da entrada do utilizador. Detém uma
 * referência para uma instância de ReviewManager e despacha cada opção do menu para o
 * método correspondente do manager.
 *
 * @author DA Project Group
 * @date 2026
 */

#ifndef MENU_H
#define MENU_H

#include "../ReviewManager/reviewmanager.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * @class Menu
 * @brief (EN) Text-based interactive menu that orchestrates user interaction with the ReviewManager.
 * @brief (PT) Menu interativo em modo texto que orquestra a interação do utilizador com o ReviewManager.
 */
class Menu {
private:
    /**
     * @brief (EN) Reference to the ReviewManager that owns the graph and all pipeline logic.
     * @brief (PT) Referência para o ReviewManager que detém o grafo e toda a lógica do pipeline.
     */
    ReviewManager &manager;

public:
    /**
     * @brief (EN) Constructs a Menu bound to the given ReviewManager.
     * @brief (PT) Constrói um Menu ligado ao ReviewManager dado.
     * @param rm Reference to the ReviewManager to use.
     */
    Menu(ReviewManager &rm) : manager(rm) {}

    /**
     * @brief (EN) Starts and runs the interactive menu loop until the user exits.
     * @brief (PT) Inicia e executa o ciclo interativo do menu até o utilizador sair.
     * @note Time Complexity: $O(1)$ overhead per iteration, delegating the actual complexity to the selected operations.
     */
    void display() {
        int choice = -1;
        while (choice != 0) {
            std::cout << "\n==== Conference Management System ====\n";
            std::cout << "1. Load Dataset (1-14)\n";
            std::cout << "2. Show Graph Summary (Nodes)\n";
            std::cout << "3. Run Assignments (Edmonds-Karp)\n";
            std::cout << "4. Print Edges (Debug)\n";
            std::cout << "5. Show Parameters\n";
            std::cout << "6. Display Risk Level\n";
            std::cout << "0. Exit\n";
            std::cout << "Selection: ";
            
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            handleChoice(choice);
        }
    }

    /**
     * @brief (EN) Dispatches a menu selection to the corresponding ReviewManager operation.
     * @brief (PT) Despacha uma seleção do menu para a operação correspondente do ReviewManager.
     * @param choice Integer representing the user's menu selection.
     * @note Time Complexity: $O(1)$ routing. The execution time depends entirely on the underlying ReviewManager method called.
     */
    void handleChoice(int choice) {
        switch (choice) {
            case 1: {
                int dsNumber;
                std::cout << "Escolha um dataset (1-14): ";
                std::cin >> dsNumber;
                
                if (dsNumber >= 1 && dsNumber <= 14) {
                    std::string filename = "dataset/input/dataset" + std::to_string(dsNumber) + ".csv";
                    manager.load(filename);
                    std::cout << "Data loaded from: " << filename << "\n";
                } else {
                    std::cout << "Invalid dataset number.\n";
                }
                break;
            }
            case 2:
                printGraphSummary();
                break;
            case 3:
                manager.prepareNetwork();
                manager.runAssignments();
                manager.showAssignments();
                std::cout << "Assignments calculated.\n";
                break;
            case 4:
                manager.prepareNetwork();
                manager.printEdges();
                break;
            case 5:
                manager.printParameters();
                break;
            case 6: {
                if (manager.getGraph().getVertexSet().empty()) {
                    std::cout << "\n[Error]: No data loaded. Please select a dataset (Option 1) first.\n";
                    break;
                }

                int riskFlag = manager.getControl().riskAnalysis;
                
                if (riskFlag == 0) {
                    std::cout << "\n[Risk Analysis]: No risk analysis was required by this dataset (Flag 0).\n";
                } else {
                    std::cout << "\n[Risk Analysis]: Analyzing critical reviewers (Flag " << riskFlag << ")...\n";
                    
                    manager.prepareNetwork(); 
                    manager.runAssignments();
                    
                    std::vector<int> risky = manager.findRiskyReviewers();
                    
                    if (risky.empty()) {
                        std::cout << "No risky reviewers identified. (The system has enough redundancy).\n";
                    } else {
                        std::cout << "CRITICAL/RISKY REVIEWER IDs: ";
                        for (size_t i = 0; i < risky.size(); ++i) {
                            std::cout << risky[i] << (i < risky.size() - 1 ? ", " : "");
                        }
                        std::cout << "\n";
                    }
                }
                break;
            }
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid option selected.\n";
        }
    }

    /**
     * @brief (EN) Prints a summary of all vertices in the graph (ID and title).
     * @brief (PT) Imprime um resumo de todos os vértices no grafo (ID e título).
     * @note Time Complexity: $O(V)$ where V is the total number of vertices (submissions + reviewers) in the graph.
     */
    void printGraphSummary() {
        auto vertices = manager.getGraph().getVertexSet();
        if (vertices.empty()) {
            std::cout << "Graph is empty. Please load data first.\n";
            return;
        }
        std::cout << "\n--- Graph Nodes Summary ---\n";
        for (auto v : vertices) {
            std::cout << "ID: " << v->getInfo() << " | Title: " << v->title << "\n";
        }
    }
};

#endif
