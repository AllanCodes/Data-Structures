// Submitter: aeivazia(Eivazian, Allan)
#ifndef DIJKSTRA_HPP_
#define DIJKSTRA_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
#include "array_queue.hpp"
#include "array_stack.hpp"
#include "heap_priority_queue.hpp"
#include "hash_graph.hpp"


namespace ics {


class Info {
  public:
    Info() { }

    Info(std::string a_node) : node(a_node) { }

    bool operator==(const Info &rhs) const { return cost == rhs.cost && from == rhs.from; }

    bool operator!=(const Info &rhs) const { return !(*this == rhs); }

    friend std::ostream &operator<<(std::ostream &outs, const Info &i) {
      outs << "Info[" << i.node << "," << i.cost << "," << i.from << "]";
      return outs;
    }

    //Public instance variable definitions
    std::string node = "?";
    int cost = std::numeric_limits<int>::max();
    std::string from = "?";
  };

    int hash_str(const std::string& s) {
     std::hash<std::string> str_hash;
     return str_hash(s);
    }

  bool gt_info(const Info &a, const Info &b) { return a.cost < b.cost; }

  typedef ics::HashGraph<int>                            DistGraph;
  typedef ics::HeapPriorityQueue<Info, gt_info>          CostPQ;
  typedef ics::HashMap<std::string, Info,hash_str>       CostMap;
  typedef ics::pair<std::string, Info>                   CostMapEntry;


//Return the final_map as specified in the lecture-note description of
//  extended Dijkstra algorithm
  CostMap extended_dijkstra(const DistGraph &g, std::string start_node) {
     //declare
     CostMap answer_map;
     CostMap info_map;


     for (auto &kv : g.all_nodes()) {
      if (kv.first == start_node) {
       info_map[kv.first] = Info(kv.first);
       info_map[kv.first].cost = 0;
       continue;
      }
          info_map[kv.first] = Info(kv.first); //fill info map: it holds nodes and has an empty Info class
     }
     //declare
     CostPQ info_pq;
     for (auto &kv : info_map) {
      info_pq.enqueue(kv.second);
     }

     while(!info_pq.empty()) {
      Info current = info_pq.dequeue();
      if (current.cost == std::numeric_limits<int>::max()) {
       break;
      }
      if (answer_map.has_key(current.node)) {
       continue;
      }
      std::string min_node = current.node;
      int min_cost = current.cost;

      info_map.erase(min_node);
      answer_map[min_node] = current;

      for (auto &kv : g.out_nodes(min_node)) {
       if (!answer_map.has_key(kv)) {
        if (info_map[kv].cost == std::numeric_limits<int>::max() || info_map[kv].cost > g.edge_value(min_node,kv) + min_cost) {
          info_map[kv].cost = g.edge_value(min_node,kv) + min_cost;
          info_map[kv].from = min_node;
         info_pq.enqueue(info_map[kv]);
        }
       }
      }


     }

     return answer_map;
  }


//Return a queue whose front is the start node (implicit in answer_map) and whose
//  rear is the end node
  ArrayQueue <std::string> recover_path(const CostMap &answer_map, std::string end_node) {

     int total_cost = answer_map[end_node].cost;
     ArrayQueue<std::string> path;
     ArrayStack<std::string> tmpPath;
     //std::string temp = answer_map[end_node].from;
     std::string temp = end_node;
     tmpPath.push(end_node);

     for (;;) {
      tmpPath.push(answer_map[temp].from);
      temp = answer_map[temp].from;
      if (answer_map[temp].from == "?")
       break;
     }

     while (!tmpPath.empty()) {
      path.enqueue(tmpPath.pop());
     }

     std::cout << "Cost is " << answer_map[end_node].cost << "; ";

     return path;
  }


}

#endif /* DIJKSTRA_HPP_ */
