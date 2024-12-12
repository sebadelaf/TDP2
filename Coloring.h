#ifndef COLORING_H
#define COLORING_H

#include <vector>
#include <set>
#include <chrono>
#include "Graph.h"

class Coloring {
public:
    // Constructor opcional si se necesita (en este caso no es estrictamente necesario)
    Coloring();

    // Coloreo greedy mejorado (por grado decreciente)
    int improvedGreedyColoring(const Graph& graph, std::vector<int>& result);

    // Heurística para encontrar una clique máxima aproximada
    int approximateMaxClique(const Graph& graph);

    // DSATUR con poda (branch and bound)
    // Esta función controlará el timeout y delegará en una función privada
    void dsaturBranchAndBound(const Graph& graph, 
                              std::vector<int>& bestSolution, 
                              int& upperBound, 
                              const std::chrono::steady_clock::time_point& startTime, 
                              bool& timeout);

private:
    // Funciones auxiliares
    int selectVertex(const std::vector<int>& saturation, const std::vector<int>& degree, const std::vector<bool>& colored);
    void dsaturBranchAndBoundRecursive(const Graph& graph, 
                                       std::vector<int>& bestSolution, 
                                       int& upperBound, 
                                       int usedColors, 
                                       int step, 
                                       std::vector<int>& color, 
                                       std::vector<int>& saturation, 
                                       std::vector<bool>& colored, 
                                       int maxClique,
                                       const std::vector<int>& degree,
                                       const std::chrono::steady_clock::time_point& startTime, 
                                       bool& timeout);
};

#endif
