/**
 * @file main.cpp
 * @brief Application entry point for the Conference Management System.
 * @brief (PT) Ponto de entrada da aplicação do Sistema de Gestão de Conferências.
 *
 * @details
 * (EN) Instantiates the ReviewManager (which owns the graph, parameters, and control settings)
 * and the Menu (which owns the user interface), then starts the interactive loop.
 * All pipeline logic (loading, network building, max-flow, output) is triggered from the menu.
 *
 * (PT) Instancia o ReviewManager (que detém o grafo, os parâmetros e as definições de controlo)
 * e o Menu (que detém a interface com o utilizador), e inicia o ciclo interativo.
 * Toda a lógica do pipeline (carregamento, construção da rede, fluxo máximo, saída) é
 * despoletada a partir do menu.
 *
 * @author DA Project Group
 * @date 2026
 */

#include "ReviewManager/reviewmanager.h"
#include "menu/menu.h"

/**
 * @brief (EN) Application entry point.
 * @brief (PT) Ponto de entrada da aplicação.
 *
 * @details
 * (EN) Creates a ReviewManager instance as the single owner of all data and pipeline logic,
 * then creates a Menu bound to it and starts the interactive display loop.
 * The program runs until the user selects option 0 (Exit) from the menu.
 *
 * (PT) Cria uma instância de ReviewManager como único detentor de todos os dados e lógica
 * do pipeline, depois cria um Menu ligado a ele e inicia o ciclo interativo de apresentação.
 * O programa corre até o utilizador selecionar a opção 0 (Sair) no menu.
 *
 * @return 0 on successful exit. / (PT) 0 em caso de saída bem-sucedida.
 */
int main() {
    ReviewManager manager; // (EN) Owns the graph, params, and control settings.
                           // (PT) Detém o grafo, os parâmetros e as definições de controlo.

    Menu menu(manager);    // (EN) Presentation layer — holds a reference to the manager.
                           // (PT) Camada de apresentação — guarda uma referência para o manager.

    menu.display();        // (EN) Start the interactive menu loop.
                           // (PT) Inicia o ciclo interativo do menu.

    return 0;
}