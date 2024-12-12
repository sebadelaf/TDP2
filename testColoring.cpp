#include <iostream>
#include "Graph.h"
#include "Coloring.h"
#include <chrono>
using namespace std;

void testImprovedGreedyColoring() {
    cout << "=== Test de improvedGreedyColoring ===" << endl;

    // Crear un grafo con 5 vértices
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);

    // Crear un objeto Coloring
    Coloring coloring;
    vector<int> result;

    // Ejecutar el algoritmo de coloreo greedy mejorado
    int numColors = coloring.improvedGreedyColoring(g, result);

    // Mostrar los colores asignados
    cout << "Colores asignados:" << endl;
    for (int i = 0; i < (int)result.size(); i++) {
        cout << "Vertice " << i + 1 << ": Color " << result[i] << endl;
    }
    cout << "Número de colores utilizados: " << numColors << endl;
    cout << "Test de improvedGreedyColoring completado.\n" << endl;
}

void testApproximateMaxClique() {
    cout << "=== Test de approximateMaxClique ===" << endl;

    // Crear un grafo con 5 vértices
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);

    // Crear un objeto Coloring
    Coloring coloring;

    // Aproximar el tamaño de la máxima clique
    int cliqueSize = coloring.approximateMaxClique(g);

    // Mostrar el tamaño aproximado de la clique
    cout << "Tamaño de la máxima clique aproximada: " << cliqueSize << endl;
    cout << "Test de approximateMaxClique completado.\n" << endl;
}

void testDsatur() {
    cout << "=== Test de DSATUR con Branch and Bound ===" << endl;

    // Crear un grafo con 6 vértices
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 0);
    g.addEdge(0, 3); // Agregar más conexiones para mayor complejidad

    // Crear un objeto Coloring
    Coloring coloring;
    vector<int> bestSolution;
    int upperBound = g.V;
    bool timeout = false;

    // Ejecutar el algoritmo DSATUR Branch and Bound
    auto start = chrono::steady_clock::now();
    coloring.dsaturBranchAndBound(g, bestSolution, upperBound, start, timeout);

    // Mostrar los resultados
    if (timeout) {
        cout << "Tiempo excedido durante la ejecución de DSATUR Branch and Bound." << endl;
    } else {
        cout << "Mejor solución encontrada:" << endl;
        for (int i = 0; i < (int)bestSolution.size(); i++) {
            cout << "Vertice " << i + 1 << ": Color " << bestSolution[i] << endl;
        }
        cout << "Número de colores utilizados: " << upperBound << endl;
    }
    cout << "Test de DSATUR completado.\n" << endl;
}

int main() {
    testImprovedGreedyColoring();
    testApproximateMaxClique();
    testDsatur();
    return 0;
}
