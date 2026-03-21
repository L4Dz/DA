/**
 * @file Menu.h
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

/**
 * @class Menu
 * @brief (EN) Text-based interactive menu that orchestrates user interaction with the ReviewManager.
 * @brief (PT) Menu interativo em modo texto que orquestra a interação do utilizador com o ReviewManager.
 *
 * @details
 * (EN) The Menu class acts as the presentation layer of the system. It does not own any data —
 * it holds only a reference to an existing ReviewManager, which owns the graph, parameters,
 * and control settings. The menu loop runs until the user selects option 0 (Exit).
 *
 * Available options:
 *  - 1: Load data from "input.csv" into the ReviewManager.
 *  - 2: Print a summary of all vertices (ID and title) in the graph.
 *  - 3: Build the network, run Edmonds-Karp, and display the resulting assignments.
 *  - 4: Build the network and print all edges with capacities (debug view).
 *  - 5: Print current algorithm parameters and control settings.
 *  - 0: Exit the application.
 *
 * (PT) A classe Menu atua como a camada de apresentação do sistema. Não detém dados próprios —
 * guarda apenas uma referência para um ReviewManager existente, que detém o grafo, os parâmetros
 * e as definições de controlo. O ciclo do menu corre até o utilizador selecionar a opção 0 (Sair).
 *
 * Opções disponíveis:
 *  - 1: Carregar dados de "input.csv" para o ReviewManager.
 *  - 2: Imprimir um resumo de todos os vértices (ID e título) no grafo.
 *  - 3: Construir a rede, correr Edmonds-Karp e mostrar as atribuições resultantes.
 *  - 4: Construir a rede e imprimir todas as arestas com capacidades (vista de depuração).
 *  - 5: Imprimir os parâmetros do algoritmo e as definições de controlo atuais.
 *  - 0: Sair da aplicação.
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
     * @param rm Reference to the ReviewManager to use. / (PT) Referência para o ReviewManager a usar.
     */
    Menu(ReviewManager &rm) : manager(rm) {}

    /**
     * @brief (EN) Starts and runs the interactive menu loop until the user exits.
     * @brief (PT) Inicia e executa o ciclo interativo do menu até o utilizador sair.
     *
     * @details
     * (EN) Repeatedly displays the menu options and reads the user's choice, delegating
     * each selection to handleChoice(). The loop exits when choice == 0.
     *
     * (PT) Apresenta repetidamente as opções do menu e lê a escolha do utilizador,
     * delegando cada seleção para handleChoice(). O ciclo termina quando choice == 0.
     */
    void display() {
        int choice = -1;
        while (choice != 0) {
            std::cout << "\n==== Conference Management System ====\n";
            std::cout << "1. Load Data (input.csv)\n";
            std::cout << "2. Show Graph (Nodes & Domains)\n";
            std::cout << "3. Run Assignments (Edmonds-Karp)\n";
            std::cout << "4. print Edges  (For debugs)\n";
            std::cout << "5. Show Parameters\n";
            std::cout << "0. Exit\n";
            std::cout << "Selection: ";
            std::cin >> choice;

            handleChoice(choice);
        }
    }

    /**
     * @brief (EN) Dispatches a menu selection to the corresponding ReviewManager operation.
     * @brief (PT) Despacha uma seleção do menu para a operação correspondente do ReviewManager.
     *
     * @details
     * (EN) Each case maps directly to a pipeline step:
     *  - Case 1: Loads "input.csv" via manager.load().
     *  - Case 2: Prints a graph vertex summary via printGraphSummary().
     *  - Case 3: Calls prepareNetwork(), runAssignments(), and showAssignments() in sequence.
     *  - Case 4: Calls prepareNetwork() and printEdges() (useful for debugging the network structure).
     *  - Case 5: Calls printParameters() to display the current configuration.
     *  - Case 0: Prints exit message (loop termination is handled in display()).
     *  - Default: Prints an invalid option warning.
     *
     * (PT) Cada caso mapeia diretamente para um passo do pipeline:
     *  - Caso 1: Carrega "input.csv" via manager.load().
     *  - Caso 2: Imprime um resumo dos vértices do grafo via printGraphSummary().
     *  - Caso 3: Chama prepareNetwork(), runAssignments() e showAssignments() em sequência.
     *  - Caso 4: Chama prepareNetwork() e printEdges() (útil para depurar a estrutura da rede).
     *  - Caso 5: Chama printParameters() para mostrar a configuração atual.
     *  - Caso 0: Imprime mensagem de saída (a terminação do ciclo é tratada em display()).
     *  - Defeito: Imprime aviso de opção inválida.
     *
     * @param choice The menu option selected by the user. / (PT) A opção do menu selecionada pelo utilizador.
     */
    void handleChoice(int choice) {
        switch (choice) {
            case 1:
                manager.load("input.csv");
                std::cout << "Data loaded successfully.\n";
                break;
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
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid option.\n";
        }
    }

    /**
     * @brief (EN) Prints a summary of all vertices in the graph (ID and title).
     * @brief (PT) Imprime um resumo de todos os vértices no grafo (ID e título).
     *
     * @details
     * (EN) Retrieves the full vertex set from the graph and prints each vertex's integer ID
     * and title field. If the graph is empty (e.g., data has not been loaded yet), a
     * descriptive warning is printed instead.
     *
     * (PT) Obtém o conjunto completo de vértices do grafo e imprime o ID inteiro e o campo
     * title de cada vértice. Se o grafo estiver vazio (ex.: os dados ainda não foram carregados),
     * é impressa uma mensagem de aviso descritiva.
     */
    void printGraphSummary() {
        auto vertices = manager.getGraph().getVertexSet();
        if (vertices.empty()) {
            std::cout << "Graph is empty. Please load data first.\n";
            return;
        }
        for (auto v : vertices) {
            std::cout << "ID: " << v->getInfo() << " | Title: " << v->title << "\n";
        }
    }
};

#endif