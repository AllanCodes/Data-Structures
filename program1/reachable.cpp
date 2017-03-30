// Submitter: Allan Eivazian 93668453
// Partner: Tommy Le 52999842
// We certify that we worked cooperatively on this programming
// assignment, according to the rules for pair programming.
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"


typedef ics::ArraySet<std::string>          NodeSet;
typedef ics::pair<std::string,NodeSet>      GraphEntry;

bool graph_entry_gt (const GraphEntry& a, const GraphEntry& b)
{return a.first<b.first;}

typedef ics::ArrayPriorityQueue<GraphEntry,graph_entry_gt> GraphPQ;
typedef ics::ArrayMap<std::string,NodeSet>  Graph;


//Read an open file of edges (node names separated by semicolons, with an
//  edge going from the first node name to the second node name) and return a
//  Graph (Map) of each node name associated with the Set of all node names to
//  which there is an edge from the key node name.
Graph read_graph(std::ifstream &file) {
    Graph graph;

    std::string line; //the current line

    while (std::getline(file,line)) {

        std::vector<std::string> storeNodes = ics::split(line, ";");
        NodeSet ns; //this is the tmp set we use for storeNodes
        ns.insert(storeNodes[1]);
        graph[storeNodes[0]].insert_all(ns);
        ns.clear();
    }

    return graph;


}


//Print a label and all the entries in the Graph in alphabetical order
//  (by source node).
//Use a "->" to separate the source node name from the Set of destination
//  node names to which it has an edge.
void print_graph(const Graph& graph) {

    GraphPQ sortPQ(graph, graph_entry_gt);

    std::cout << "Graph: source node -> set[destination nodes]" << std::endl;

    for(auto &kv : sortPQ) {
        std::cout << kv.first << " -> " << kv.second << std::endl;
    }

}


//Return the Set of node names reaching in the Graph starting at the
//  specified (start) node.
//Use a local Set and a Queue to respectively store the reachable nodes and
//  the nodes that are being explored.
NodeSet reachable(const Graph& graph, std::string start) {

    NodeSet reached;
    ics::ArrayQueue<std::string> explore({start});
    std::string tmp = start;

    while(!explore.empty()) {

        tmp = explore.dequeue();

        reached.insert(tmp);

        for (auto &kv : graph) {
            if (kv.first == tmp) {
                for (auto i = kv.second.begin(); i != kv.second.end(); ++i) {
                    explore.enqueue(*i);
                }
            }
        }
    }

    return reached;


}

//Prompt the user for a file, create a graph from its edges, print the graph,
//  and then repeatedly (until the user enters "quit") prompt the user for a
//  starting node name and then either print an error (if that the node name
//  is not a source node in the graph) or print the Set of node names
//  reachable from it by using the edges in the Graph.
int main() {
    try {
        std::string node = "";
        std::ifstream file;
        ics::safe_open(file,"Enter file name", "../input files/graph1.txt");
        Graph graph = read_graph(file);
        print_graph(graph);

        while(true) {
            std::cout << "Enter some starting node name (else quit): ";
            std::getline(std::cin, node);
            if (node == "quit" || node == "QUIT" || node == "Quit") {
                break;
            }
            if (graph.has_key(node)) {
                std::cout << "From " << node << " the reachable nodes are " << reachable(graph, node) << std::endl;
                std::cout << std::endl;
            }
            else {
                std::cout << node << " is not a source node name in the graph" << std::endl;
                std::cout << std::endl;
            }
        }


        file.close();

    } catch (ics::IcsError& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
