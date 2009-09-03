CC = g++
CPPFLAGS = -g

all: utils.o graph.o main.o scan.o scan_graph.o edge.o
	$(CC) $(CPPFLAGS) -o scan utils.o main.o graph.o scan.o scan_graph.o edge.o

scan_graph: scan_graph.cpp scan_graph.hpp
	$(CC) $(CPPFLAGS) -c scan_graph.cpp scan_graph.o

graph: graph.cpp graph.hpp
	$(CC) $(CPPFLAGS) -c graph.cpp graph.o

edge: edge.cpp edge.hpp
	$(CC) $(CPPFLAGS) -c edge.cpp edge.o

main: main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp main.o

scan: scan.cpp scan.hpp
	$(CC) $(CPPFLAGS) -c scan.cpp scan.o

utils: utils.cpp utils.hpp
	$(CC) $(CPPFLAGS) -c utils.cpp utils.o

teste: teste.cpp graph.o scan_graph.o utils.o edge.o
	$(CC) $(CPPFLAGS) -c teste.cpp 
	$(CC) $(CPPFLAGS) -o teste utils.o scan_graph.o teste.o edge.o graph.o

clean:
	rm *.o scan teste
