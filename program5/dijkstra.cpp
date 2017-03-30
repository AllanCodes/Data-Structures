// Submitter: aeivazia(Eivazian, Allan)
#include <string>
#include <iostream>
#include <fstream>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "hash_graph.hpp"
#include "dijkstra.hpp"



std::string get_node_in_graph(const ics::DistGraph& g, std::string prompt, bool allow_QUIT) {
  std::string node;
  for(;;) {
    node = ics::prompt_string(prompt + " (must be in graph" + (allow_QUIT ? " or QUIT" : "") + ")");
    if ( (allow_QUIT && node == "QUIT") || g.has_node(node) )
      break;
  }
  return node;
}


int main() {
  try {

   std::string file = ics::prompt_string("Enter the name of the file", "flightdist.txt");
   std::ifstream file_in(file);
   ics::DistGraph g;
   g.load(file_in,";");
   std::cout << g << std::endl;

   std::string start_node = ics::prompt_string("Enter the start node (must be in graph)");
   while(!g.has_node(start_node)) {
    start_node = ics::prompt_string("Enter the start node (must be in graph)");
   }
   ics::CostMap d = ics::extended_dijkstra(g,start_node);
   std::cout << d << std::endl;

   for (;;) {
    std::string stop_node = ics::prompt_string("Enter stop node (must be in graph or QUIT)");
    if (stop_node == "QUIT") {
     break;
    }
    while (!g.has_node(stop_node)) {
     stop_node = ics::prompt_string("Enter stop node (must be in graph or QUIT)");
    }
    ics::ArrayQueue<std::string> path = ics::recover_path(d, stop_node);
    std::cout << "path is " << path << std::endl;


   }
  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
