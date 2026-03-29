#include "ReviewManager/reviewmanager.h"
#include "menu/menu.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    ReviewManager manager;

    if (argc == 4 && std::string(argv[1]) == "-b") {
        // 1. Carrega os dados (O parser pode ler o nome do ficheiro do CSV, mas vamos ignorar)
        manager.load(argv[2]);
        
        // 2. Prepara e corre
        manager.prepareNetwork();
        manager.runAssignments();
        
        // 3. Salva APENAS neste caminho que passaste no argumento
        manager.saveBatchResults(argv[3]); 
        
        std::cout << "Batch concluído. Resultados em: " << argv[3] << std::endl;
        return 0;
    }

    Menu menu(manager);
    menu.display();
    return 0;
}