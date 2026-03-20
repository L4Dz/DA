#include <iostream>
#include <iomanip> // For pretty printing
#include "parser/parser.h"
#include "data_structures/graph.h"

// Assuming ReviewManager is in its own file or defined here
#include "ReviewManager/reviewmanager.h"

int main() {
    ReviewManager manager;
    std::string filename = "input.csv";

    std::cout << "--- Testing Scientific Conference Tool ---" << std::endl;
    
    // 1. Load the data
    std::cout << "[Step 1] Loading " << filename << "..." << std::endl;
    manager.load(filename);

    // 2. Build the graph connections (the network)
    std::cout << "[Step 2] Building network edges..." << std::endl;
    manager.prepareNetwork();

    // 3. Print the Graph Content
    std::cout << "\n--- Current Graph State ---\n";
    std::cout << std::left << std::setw(8) << "ID" 
              << std::setw(25) << "Title/Name" 
              << std::setw(15) << "Type" 
              << "Primary Domain" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    auto vertexSet = manager.getGraph().getVertexSet();
    
    for (auto v : vertexSet) {
        std::string typeStr;
        switch(v->type) {
            case VertexType::SUBMISSION: typeStr = "SUBMISSION"; break;
            case VertexType::REVIEWER:   typeStr = "REVIEWER"; break;
            case VertexType::SOURCE:     typeStr = "SOURCE"; break;
            case VertexType::SINK:       typeStr = "SINK"; break;
            default:                     typeStr = "UNKNOWN"; break;
        }

        std::cout << std::left << std::setw(8) << v->getInfo() 
                  << std::setw(25) << v->title.substr(0, 24) // limit title length
                  << std::setw(15) << typeStr 
                  << v->primaryDomain << std::endl;
    }

    // 4. Verify Parameters were stored
    std::cout << "\n--- Loaded Parameters ---\n";
    std::cout << "Min Reviews: " << manager.getParams().minReviews << std::endl;
    std::cout << "Max Reviews: " << manager.getParams().maxReviews << std::endl;

    return 0;
}