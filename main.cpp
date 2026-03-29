/**
 * @file main.cpp
 * @brief Entry point for the Scientific Conference Organization Tool.
 * @details Handles the initialization of the ReviewManager and dictates whether 
 * the program runs in interactive GUI mode or batch processing mode via CLI.
 */

#include "ReviewManager/reviewmanager.h"
#include "menu/menu.h"
#include <iostream>
#include <string>

/**
 * @brief Main execution function.
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments. Expects "-b <input.csv> <output.csv>" for batch mode.
 * @return 0 on successful execution.
 */
int main(int argc, char* argv[]) {
    ReviewManager manager;

    if (argc == 4 && std::string(argv[1]) == "-b") {
        manager.load(argv[2]);
        manager.prepareNetwork();
        manager.runAssignments();
        manager.saveBatchResults(argv[3]); 
        std::cout << "Batch concluído. Resultados em: " << argv[3] << std::endl;
        return 0;
    }

    Menu menu(manager);
    menu.display();
    return 0;
}
