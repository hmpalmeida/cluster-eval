CC = g++
CPPFLAGS = -g #-Wno-deprecated -Wall  -O2 

all: utils.o graph.o main.o scan.o scan_graph.o edge.o cluster_evaluator.o
	$(CC) $(CPPFLAGS) -o scan utils.o main.o graph.o scan.o scan_graph.o edge.o cluster_evaluator.o

scan_graph: scan_graph.cpp scan_graph.hpp
	$(CC) $(CPPFLAGS) -c scan_graph.cpp scan_graph.o

graph: graph.cpp graph.hpp
	$(CC) $(CPPFLAGS) -c graph.cpp graph.o

edge: edge.cpp edge.hpp
	$(CC) $(CPPFLAGS) -c edge.cpp edge.o

node: node.cpp node.hpp
	$(CC) $(CPPFLAGS) -c node.cpp node.o

main: main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp main.o

scan: scan.cpp scan.hpp
	$(CC) $(CPPFLAGS) -c scan.cpp scan.o

utils: utils.cpp utils.hpp
	$(CC) $(CPPFLAGS) -c utils.cpp utils.o

proximity: proximity.cpp proximity.hpp
	$(CC) $(CPPFLAGS) -c proximity.cpp proximity.o

naming: naming.cpp naming.hpp
	$(CC) $(CPPFLAGS) -c naming.cpp naming.o

ocurrence: ocurrence.cpp ocurrence.hpp
	$(CC) $(CPPFLAGS) -c ocurrence.cpp ocurrence.o

multiple_naming: multiple_naming.cpp multiple_naming.hpp
	$(CC) $(CPPFLAGS) -c multiple_naming.cpp multiple_naming.o

intersection_naming: intersection_naming.cpp intersection_naming.hpp
	$(CC) $(CPPFLAGS) -c intersection_naming.cpp intersection_naming.o

cluster_evaluator: cluster_evaluator.cpp cluster_evaluator.hpp
	$(CC) $(CPPFLAGS) -c cluster_evaluator.cpp cluster_evaluator.o

teste: teste.cpp utils.o naming.o graph.o edge.o multiple_naming.o ocurrence.o intersection_naming.o
	$(CC) $(CPPFLAGS) -c teste.cpp 
	$(CC) $(CPPFLAGS) -o teste utils.o teste.o naming.o graph.o edge.o multiple_naming.o ocurrence.o intersection_naming.o

clean:
	rm *.o scan

clean_teste:
	rm *.o teste

clear_results:
	rm results/*outliers.txt results/*clusters.txt results/*hubs.txt

