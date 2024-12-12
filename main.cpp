#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "Graph.h"
#include "Coloring.h"
using namespace std;
using namespace std::chrono;

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

                // Crear objeto Coloring
                Coloring colorador;

                // Medir tiempo de ejecución total
                auto start = steady_clock::now();

                // Preparar variables para branch and bound
                vector<int> bestSolution;
                int upperBound = g.V; // Cota superior inicial (peor caso)
                bool timeout = false; // Para controlar si se excede el tiempo

                // Ejecutar DSATUR con poda
                colorador.dsaturBranchAndBound(g, bestSolution, upperBound, start, timeout);

                auto end = steady_clock::now();
                auto totalElapsed = duration_cast<milliseconds>(end - start).count();

                if (timeout) {
                    cout << "Tiempo excedido (mas de 30 segundos). Se utilizara el resultado del coloreo greedy." << endl;

                    vector<int> greedyColors;
                    int greedyCount = colorador.improvedGreedyColoring(g, greedyColors);

                    cout << "Coloreo Greedy Mejorado:" << endl;
                    for (int i = 0; i < (int)greedyColors.size(); ++i) {
                        cout << "Vertice " << i + 1 << ": Color " << greedyColors[i] << endl;
                    }
                    cout << "Numero de colores usados (Greedy): " << greedyCount << endl;

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