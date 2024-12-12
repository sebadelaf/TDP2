#include "Graph.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
using namespace std;

Graph::Graph(int V) : V(V), adj(V) {} //Constructor

void Graph::addEdge(int u, int v) { //Agregar arista
    if (u < 0 || v < 0 || u >= V || v >= V) { //Validar que los índices estén dentro de rango
        cerr << "Error: Índices de vértices fuera de rango." << endl; //Imprimir mensaje de error
        return;
    }
    adj[u].push_back(v);
    adj[v].push_back(u);
}

Graph readGraphFromFile(const std::string& filename) { //Función para leer un grafo desde archivo
    ifstream file(filename); //Abrir archivo
    if (!file.is_open()) { //Validar que el archivo se haya abierto correctamente
        cerr << "Error: No se pudo abrir el archivo " << filename << endl; //Imprimir mensaje de error
        exit(EXIT_FAILURE); //Salir del programa
    }

    vector<pair<int, int>> edges; //Vector de pares de enteros
    int u, v; //Variables para leer los vértices
    int maxVertex = 0; //Variable para almacenar el vértice máximo

    while (file >> u >> v) { //Leer vértices
        if (u <= 0 || v <= 0) {
            cerr << "Error: Los vértices deben ser mayores a 0." << endl;
            exit(EXIT_FAILURE);
        }
        edges.emplace_back(u - 1, v - 1); // Convertir a índice base 0
        maxVertex = max({maxVertex, u, v});
    }

    file.close();

    Graph graph(maxVertex); //Crear grafo con el vértice máximo
    for (const auto& edge : edges) { //Agregar aristas al grafo
        graph.addEdge(edge.first, edge.second); //Agregar arista
    }

    return graph; //Retornar grafo
}
