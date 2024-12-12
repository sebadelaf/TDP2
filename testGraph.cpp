#include <iostream>
#include "Graph.h"
using namespace std;

void testGraph() {
    cout << "=== Test de la clase Graph ===" << endl;

    // Crear un grafo con 5 vértices
    Graph g(5);

    // Agregar algunas aristas
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);

    // Mostrar las conexiones de los vértices
    cout << "Conexiones del grafo:" << endl;
    for (int i = 0; i < g.V; i++) {
        cout << "Vertice " << i + 1 << ": ";
        for (int neighbor : g.adj[i]) {
            cout << neighbor + 1 << " ";
        }
        cout << endl;
    }

    // Intentar agregar una arista fuera de rango
    cout << "\nProbando agregar una arista fuera de rango:" << endl;
    g.addEdge(-1, 10); // Debe mostrar un error

    cout << "Test de Graph completado.\n" << endl;
}

int main() {
    testGraph();
    return 0;
}
