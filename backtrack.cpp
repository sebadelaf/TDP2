#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
using namespace std;

class Graph {
public:
    int V; // Número de vértices
    vector<vector<int>> adj; // Lista de adyacencia

    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

// Heurística para encontrar una clique máxima aproximada
int approximateMaxClique(Graph& graph) {
    vector<bool> visited(graph.V, false);
    int maxCliqueSize = 0;

    for (int i = 0; i < graph.V; ++i) {
        if (!visited[i]) {
            set<int> clique;
            clique.insert(i);

            for (int neighbor : graph.adj[i]) {
                bool canJoinClique = true;
                for (int member : clique) {
                    if (find(graph.adj[member].begin(), graph.adj[member].end(), neighbor) == graph.adj[member].end()) {
                        canJoinClique = false;
                        break;
                    }
                }
                if (canJoinClique) {
                    clique.insert(neighbor);
                }
            }
            maxCliqueSize = max(maxCliqueSize, static_cast<int>(clique.size()));
        }
    }
    return maxCliqueSize;
}

// Función para seleccionar el siguiente vértice por DSATUR
int selectVertex(const vector<int>& saturation, const vector<int>& degree, const vector<bool>& colored) {
    int maxSaturation = -1, maxDegree = -1, vertex = -1;
    for (int i = 0; i < saturation.size(); ++i) {
        if (!colored[i] && (saturation[i] > maxSaturation || 
            (saturation[i] == maxSaturation && degree[i] > maxDegree))) {
            maxSaturation = saturation[i];
            maxDegree = degree[i];
            vertex = i;
        }
    }
    return vertex;
}

// DSATUR con poda (branch and bound)
void dsaturBranchAndBound(Graph& graph, vector<int>& bestSolution, int& upperBound, int usedColors, int step, vector<int>& color, vector<int>& saturation, vector<bool>& colored, int maxClique) {
    // Cota inferior actual
    int lowerBound = max(maxClique, usedColors);

    // Si la cota inferior es mayor o igual que la cota superior, podar
    if (lowerBound >= upperBound) return;

    // Si todos los vértices están coloreados, actualizar la mejor solución
    if (step == graph.V) {
        upperBound = usedColors; // Actualizamos el upper bound con los colores usados
        bestSolution = color; // Guardamos la solución encontrada
        return;
    }

    // Seleccionar el vértice con mayor grado de saturación
    int vertex = selectVertex(saturation, graph.adj[0], colored);

    // Intentar colorear el vértice con el color más bajo disponible
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
        dsaturBranchAndBound(graph, bestSolution, upperBound, max(usedColors, c + 1), step + 1, color, saturation, colored, maxClique);

        // Deshacer cambios para backtracking
        color[vertex] = -1;
        colored[vertex] = false;
        for (int neighbor : graph.adj[vertex]) {
            if (!colored[neighbor]) {
                saturation[neighbor]--;
            }
        }
    }
}

// Leer grafo desde archivo
Graph readGraphFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // Leer aristas del archivo
    vector<pair<int, int>> edges;
    int u, v;
    int maxVertex = 0;

    while (file >> u >> v) {
        edges.emplace_back(u - 1, v - 1); // Convertir a índice base 0
        maxVertex = max({maxVertex, u, v});
    }

    // Crear grafo
    Graph graph(maxVertex);
    for (const auto& edge : edges) {
        graph.addEdge(edge.first, edge.second);
    }

    return graph;
}

bool esEntero(const string& str) {
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return !str.empty();
}

void menu() {
    while (true) {
        cout << "--------------------------------------" << endl;
        cout << "   Menu de Coloreo de Grafos" << endl;
        cout << "--------------------------------------" << endl;
        cout << "1. Leer y colorear un grafo desde archivo" << endl;
        cout << "2. Salir" << endl;
        cout << "Seleccione una opcion: ";
        
        string opcionStr;
        cin >> opcionStr;

        // Validar que la opción sea un entero
        if (!esEntero(opcionStr)) {
            cout << "Error: Por favor, ingrese un numero valido." << endl;
            continue;
        }

        int opcion = stoi(opcionStr);

        if (opcion == 1) {
            cout << "Ingrese el nombre del archivo (sin la carpeta grafos/): ";
            string filename;
            cin >> filename;

            // Construir ruta completa
            string rutaArchivo = "grafos/" + filename;

            try {
                // Leer grafo desde el archivo
                Graph g = readGraphFromFile(rutaArchivo);

                // Preparar variables para branch and bound
                vector<int> color(g.V, -1);
                vector<int> saturation(g.V, 0);
                vector<bool> colored(g.V, false);
                vector<int> bestSolution;
                int upperBound = g.V; // Cota superior inicial (peor caso)
                int maxClique = approximateMaxClique(g);

                // Ejecutar DSATUR con poda
                dsaturBranchAndBound(g, bestSolution, upperBound, 0, 0, color, saturation, colored, maxClique);

                // Mostrar los colores asignados
                cout << "Mejor solucion encontrada:" << endl;
                for (int i = 0; i < bestSolution.size(); ++i) {
                    cout << "Vertice " << i + 1 << ": Color " << bestSolution[i] << endl;
                }

                // Mostrar la cantidad mínima de colores (upper bound final)
                cout << "Numero minimo de colores encontrados: " << upperBound << endl;

            } catch (const exception& e) {
                cout << "Error al procesar el archivo: " << e.what() << endl;
            }

        } else if (opcion == 2) {
            cout << "Saliendo del programa. ¡Hasta luego!" << endl;
            break;
        } else {
            cout << "Opcion no valida. Intente nuevamente." << endl;
        }
    }
}
int main() {
    menu();
    return 0;
}
