#ifndef REVIEW_MANAGER_H
#define REVIEW_MANAGER_H

#include "../parser/parser.h"
#include "../data_structures/graph.h"


class ReviewManager {
private:
    Graph<int> g;              // The graph lives HERE
    AssignmentParams params;    // The rules live HERE
    ControlSettings control;    // The tasks live HERE
    Parser parser;              // The tool used to fill them

public:
    // This function tells the parser: "Here are my containers, fill them."
    void load(const std::string &filename) {
        parser.loadData(filename, g, params, control);
    }

    // This function builds the network based on the params we just loaded
    void prepareNetwork() {
        parser.buildNetwork(g, params);
    }

    // Accessors so main can see what happened
    Graph<int>& getGraph() { return g; }
    ControlSettings& getControl() { return control; }
    AssignmentParams& getParams() { return params; }
};

#endif