#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class Graph {
public:
    int V; // Número de vértices
    std::vector<std::vector<int>> adj; // Lista de adyacencia

    Graph(int V); //Constructor
    void addEdge(int u, int v); //Agregar arista
};

// Función para leer un grafo desde archivo
Graph readGraphFromFile(const std::string& filename);

#endif
