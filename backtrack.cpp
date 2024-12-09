#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
#include <chrono>
using namespace std;
using namespace std::chrono;

class Graph {
public:
    int V; // Número de vértices
    vector<vector<int>> adj; // Lista de adyacencia

    Graph(int V) : V(V), adj(V) {}

    void addEdge(int u, int v) {
        if (u < 0 || v < 0 || u >= V || v >= V) {
            cerr << "Error: Índices de vértices fuera de rango." << endl;
            return;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Versión mejorada del greedy coloring (ordenando vértices por grado decreciente)
    int improvedGreedyColoring(vector<int>& result) {
        // Crear un vector de pares (grado, vértice)
        vector<pair<int,int>> vertices;
        vertices.reserve(V);
        for (int i = 0; i < V; i++) {
            vertices.push_back({(int)adj[i].size(), i});
        }

        // Ordenar por grado decreciente
        sort(vertices.begin(), vertices.end(), [](auto &a, auto &b) {
            return a.first > b.first;
        });

        result.assign(V, -1);
        vector<bool> available(V, false);

        // Asignar el primer color al vértice con mayor grado
        int firstVertex = vertices[0].second;
        result[firstVertex] = 0;

        // Para cada vértice en el orden dado
        for (int idx = 1; idx < V; idx++) {
            int u = vertices[idx].second;

            // Restaurar array disponible
            fill(available.begin(), available.end(), false);

            // Marcar colores usados por los vecinos
            for (auto &w : adj[u]) {
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

        int maxColor = *max_element(result.begin(), result.end());
        return maxColor + 1;
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
    for (int i = 0; i < (int)saturation.size(); ++i) {
        if (!colored[i] && (saturation[i] > maxSaturation || 
            (saturation[i] == maxSaturation && degree[i] > maxDegree))) {
            maxSaturation = saturation[i];
            maxDegree = degree[i];
            vertex = i;
        }
    }
    return vertex;
}

// DSATUR con poda (branch and bound), modificada para medir tiempo
void dsaturBranchAndBound(Graph& graph, vector<int>& bestSolution, int& upperBound, int usedColors, int step, 
                          vector<int>& color, vector<int>& saturation, vector<bool>& colored, int maxClique,
                          const vector<int>& degree,
                          const steady_clock::time_point& startTime, bool& timeout) {
    // Verificar tiempo transcurrido
    auto now = steady_clock::now();
    auto elapsed = duration_cast<seconds>(now - startTime).count();
    if (elapsed > 30) { 
        // Si se exceden 30 segundos, marcar timeout para usar resultado de greedy mejorado
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
        dsaturBranchAndBound(graph, bestSolution, upperBound, max(usedColors, c + 1), step + 1, color, saturation, colored, maxClique, degree, startTime, timeout);

        if (timeout) return; // Si se produjo timeout en las llamadas recursivas, retornar

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

    vector<pair<int, int>> edges;
    int u, v;
    int maxVertex = 0;

    while (file >> u >> v) {
        if (u <= 0 || v <= 0) {
            cerr << "Error: Los vértices deben ser mayores a 0." << endl;
            exit(EXIT_FAILURE);
        }
        edges.emplace_back(u - 1, v - 1); // Convertir a índice base 0
        maxVertex = max({maxVertex, u, v});
    }

    file.close();

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

            string rutaArchivo = "grafos/" + filename;

            try {
                // Leer grafo desde el archivo
                Graph g = readGraphFromFile(rutaArchivo);

                // Medir tiempo de ejecución total
                auto start = steady_clock::now();

                // Preparar variables para branch and bound
                vector<int> color(g.V, -1);
                vector<int> saturation(g.V, 0);
                vector<bool> colored(g.V, false);
                vector<int> bestSolution;
                int upperBound = g.V; // Cota superior inicial (peor caso)

                // Calcular grados para DSATUR
                vector<int> degree(g.V, 0);
                for (int i = 0; i < g.V; i++) {
                    degree[i] = (int)g.adj[i].size();
                }

                int maxClique = approximateMaxClique(g);

                bool timeout = false; // Para controlar si se excede el tiempo

                // Ejecutar DSATUR con poda
                dsaturBranchAndBound(g, bestSolution, upperBound, 0, 0, color, saturation, colored, maxClique, degree, start, timeout);

                auto end = steady_clock::now();
                auto totalElapsed = duration_cast<milliseconds>(end - start).count();

                if (timeout) {
                    cout << "Tiempo excedido (mas de 30 segundos). Se utilizara el resultado del coloreo greedy mejorado." << endl;

                    vector<int> greedyColors;
                    int greedyCount = g.improvedGreedyColoring(greedyColors);

                    cout << "Coloreo Greedy Mejorado:" << endl;
                    for (int i = 0; i < (int)greedyColors.size(); ++i) {
                        cout << "Vertice " << i + 1 << ": Color " << greedyColors[i] << endl;
                    }
                    cout << "Numero de colores usados (Greedy Mejorado): " << greedyCount << endl;

                } else {
                    // Mostrar los colores asignados por DSATUR con poda
                    cout << "Mejor solucion encontrada (DSATUR con poda):" << endl;
                    for (int i = 0; i < (int)bestSolution.size(); ++i) {
                        cout << "Vertice " << i + 1 << ": Color " << bestSolution[i] << endl;
                    }
                    cout << "Numero minimo de colores encontrados: " << upperBound << endl;
                }

                cout << "Tiempo total de ejecucion: " << totalElapsed << " ms" << endl;

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
