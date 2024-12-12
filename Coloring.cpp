#include "Coloring.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include <limits>
#include <chrono>
using namespace std;
using namespace std::chrono;


Coloring::Coloring() {}
// Función para colorear un grafo utilizando el algoritmo greedy mejorado
int Coloring::improvedGreedyColoring(const Graph& graph, std::vector<int>& result) {
    int V = graph.V; // Cantidad de vértices
    vector<pair<int,int>> vertices; // Vector de pares (grado, vértice)
    vertices.reserve(V); // Reservar memoria para los vértices 
    for (int i = 0; i < V; i++) { // Llenar el vector de vértices 
        vertices.push_back({(int)graph.adj[i].size(), i});
    }

    // Ordenar por grado decreciente
    sort(vertices.begin(), vertices.end(), [](auto &a, auto &b) {
        return a.first > b.first;
    });

    result.assign(V, -1); // Inicializar el vector de colores
    vector<bool> available(V, false); // Vector para marcar los colores disponibles

    // Asignar el primer color al vértice con mayor grado
    int firstVertex = vertices[0].second;
    result[firstVertex] = 0;

    // Para cada vértice en el orden dado
    for (int idx = 1; idx < V; idx++) {
        int u = vertices[idx].second;

        // Restaurar array disponible
        fill(available.begin(), available.end(), false);

        // Marcar colores usados por los vecinos
        for (auto &w : graph.adj[u]) {
            if (result[w] != -1) {
                available[result[w]] = true;
            }
        }

        // Encontrar el primer color disponible
        int cr;
        for (cr = 0; cr < V; cr++) {
            if (!available[cr]) {
                break;
            }
        }

        result[u] = cr;
    }

    int maxColor = *max_element(result.begin(), result.end()); // Encontrar el color máximo
    return maxColor + 1;
}

int Coloring::approximateMaxClique(const Graph& graph) { // Función para encontrar una clique máxima aproximada
    vector<bool> visited(graph.V, false); // Vector de visitados
    int maxCliqueSize = 0; // Tamaño de la clique máxima

    for (int i = 0; i < graph.V; ++i) { 
        if (!visited[i]) { 
            set<int> clique; // Conjunto de vértices de la clique
            clique.insert(i);

            for (int neighbor : graph.adj[i]) {
                bool canJoinClique = true;
                for (int member : clique) {
                    // Verificar si el vecino es adyacente a todos los miembros de la clique
                    if (find(graph.adj[member].begin(), graph.adj[member].end(), neighbor) == graph.adj[member].end()) {
                        canJoinClique = false; // Si no es adyacente, no se puede unir
                        break;
                    }
                }
                if (canJoinClique) {
                    clique.insert(neighbor); // Si se puede unir, agregar a la clique
                }
            }
            maxCliqueSize = max(maxCliqueSize, (int)clique.size()); // Actualizar el tamaño de la clique máxima
        }
    }
    return maxCliqueSize; // Devolver el tamaño de la clique máxima
}
// Función auxiliar para seleccionar un vértice
int Coloring::selectVertex(const vector<int>& saturation, const vector<int>& degree, const vector<bool>& colored) {
    int maxSaturation = -1, maxDegree = -1, vertex = -1;
    for (int i = 0; i < (int)saturation.size(); ++i) {
        // Seleccionar el vértice no coloreado con mayor grado de saturación
        if (!colored[i] && (saturation[i] > maxSaturation || 
            (saturation[i] == maxSaturation && degree[i] > maxDegree))) {
            maxSaturation = saturation[i];
            maxDegree = degree[i];
            vertex = i;
        }
    }
    return vertex;
}

// Función auxiliar para colorear un grafo utilizando DSATUR con poda (branch and bound)
void Coloring::dsaturBranchAndBoundRecursive(const Graph& graph, vector<int>& bestSolution, int& upperBound, int usedColors, int step, 
                                             vector<int>& color, vector<int>& saturation, vector<bool>& colored, int maxClique,
                                             const vector<int>& degree,
                                             const steady_clock::time_point& startTime, bool& timeout) {
    // Verificar tiempo transcurrido
    auto now = steady_clock::now(); 
    auto elapsed = duration_cast<seconds>(now - startTime).count();
    if (elapsed > 30) { 
        // Si se exceden 30 segundos, marcar timeout
        timeout = true;
        return; 
    }

    // Cota inferior actual
    int lowerBound = max(maxClique, usedColors);

    // Si la cota inferior es mayor o igual que la cota superior, podar
    if (lowerBound >= upperBound) return;

    // Si todos los vértices están coloreados, actualizar la mejor solución
    if (step == graph.V) {
        upperBound = usedColors; 
        bestSolution = color; 
        return;
    }

    // Seleccionar el vértice con mayor grado de saturación
    int vertex = selectVertex(saturation, degree, colored);

    // Encontrar los colores vecinos
    set<int> neighborColors;
    for (int neighbor : graph.adj[vertex]) {
        if (color[neighbor] != -1) {
            neighborColors.insert(color[neighbor]);
        }
    }

    for (int c = 0; c < upperBound; ++c) {
        if (neighborColors.count(c)) continue;

        // Asignar el color al vértice
        color[vertex] = c;
        colored[vertex] = true;

        // Actualizar el grado de saturación de los vecinos
        for (int neighbor : graph.adj[vertex]) {
            if (!colored[neighbor]) {
                saturation[neighbor]++;
            }
        }

        // Llamada recursiva
        dsaturBranchAndBoundRecursive(graph, bestSolution, upperBound, max(usedColors, c + 1), step + 1, 
                                      color, saturation, colored, maxClique, degree, startTime, timeout);

        if (timeout) return; // Si se produjo timeout

        // Deshacer cambios para backtracking
        color[vertex] = -1;
        colored[vertex] = false;
        for (int neighbor : graph.adj[vertex]) {
            if (!colored[neighbor]) {
                saturation[neighbor]--; // Restaurar el grado de saturación
            }
        }
    }
}
// Función para iniciar el coloreo de un grafo utilizando DSATUR con poda (branch and bound)
void Coloring::dsaturBranchAndBound(const Graph& graph, 
                                    vector<int>& bestSolution, 
                                    int& upperBound, 
                                    const steady_clock::time_point& startTime, 
                                    bool& timeout) {
    vector<int> color(graph.V, -1);
    vector<int> saturation(graph.V, 0);
    vector<bool> colored(graph.V, false);

    // Calcular grados para DSATUR
    vector<int> degree(graph.V, 0);
    for (int i = 0; i < graph.V; i++) {
        degree[i] = (int)graph.adj[i].size();
    }

    int maxClique = approximateMaxClique(graph);

    // Llamar a la función recursiva
    dsaturBranchAndBoundRecursive(graph, bestSolution, upperBound, 0, 0, color, saturation, colored, maxClique, degree, startTime, timeout);
}
