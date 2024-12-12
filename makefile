CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2

# Archivos fuente principales
SOURCES = main.cpp Graph.cpp Coloring.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = main

# Archivos de prueba
TEST_SOURCES = testGraph.cpp testColoring.cpp
TEST_EXECUTABLES = testGraph testColoring

all: $(EXEC) $(TEST_EXECUTABLES)

# Ejecutable principal
$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC)

# Regla para compilar Graph.cpp
Graph.o: Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c Graph.cpp

# Regla para compilar Coloring.cpp
Coloring.o: Coloring.cpp Coloring.h
	$(CXX) $(CXXFLAGS) -c Coloring.cpp

# Regla para compilar main.cpp
main.o: main.cpp Graph.h Coloring.h
	$(CXX) $(CXXFLAGS) -c main.cpp

# Reglas para los ejecutables de prueba
testGraph: testGraph.cpp Graph.cpp
	$(CXX) $(CXXFLAGS) testGraph.cpp Graph.cpp -o testGraph

testColoring: testColoring.cpp Graph.cpp Coloring.cpp
	$(CXX) $(CXXFLAGS) testColoring.cpp Graph.cpp Coloring.cpp -o testColoring

# Limpiar
clean:
	rm -f $(OBJECTS) $(EXEC) $(TEST_EXECUTABLES)

.PHONY: all clean
