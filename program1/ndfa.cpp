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


typedef ics::ArraySet<std::string>                     States;
typedef ics::ArrayQueue<std::string>                   InputsQueue;
typedef ics::ArrayMap<std::string,States>              InputStatesMap;

typedef ics::ArrayMap<std::string,InputStatesMap>       NDFA;
typedef ics::pair<std::string,InputStatesMap>           NDFAEntry;

bool gt_NDFAEntry (const NDFAEntry& a, const NDFAEntry& b)
{return a.first<b.first;}

typedef ics::ArrayPriorityQueue<NDFAEntry,gt_NDFAEntry> NDFAPQ;

typedef ics::pair<std::string,States>                   Transitions;
typedef ics::ArrayQueue<Transitions>                    TransitionsQueue;


//Read an open file describing the non-deterministic finite automaton (each
//  line starts with a state name followed by pairs of transitions from that
//  state: (input followed by a new state, all separated by semicolons), and
//  return a Map whose keys are states and whose associated values are another
//  Map with each input in that state (keys) and the resulting set of states it
//  can lead to.
const NDFA read_ndfa(std::ifstream &file) {
    NDFA ndfa;
    std::string line;

    while(getline(file,line)) {
        std::vector<std::string> vec = ics::split(line,";");
        InputStatesMap tmp;
        for (int i = 1; i < vec.size(); ) {
            if (i % 2 != 0) {
                tmp[vec[i]].insert(vec[i+1]);
                i += 2;
            }
            else i++;
        }
        ndfa[vec[0]] = tmp;
    }


    return ndfa;
}

//Print a label and all the entries in the finite automaton Map, in
//  alphabetical order of the states: each line has a state, the text
//  "transitions:" and the Map of its transitions.
void print_ndfa(const NDFA& ndfa) {
    NDFAPQ sortPQ(ndfa,gt_NDFAEntry);
//    NDFA n;
//    n.put_all(sortPQ);

    std::cout << "The Non-Deterministic Finite Automaton Description" << std::endl;
    for (auto &kv : sortPQ) {
        std::cout << kv.first << " transitions: " << kv.second << std::endl;
    }
    std::cout << std::endl;
}


//Return a queue of the calculated transition pairs, based on the non-deterministic
//  finite automaton, initial state, and queue of inputs; each pair in the returned
//  queue is of the form: input, set of new states.
//The first pair contains "" as the input and the initial state.
//If any input i is illegal (does not lead to any state in the non-deterministic finite
//  automaton), ignore it.
TransitionsQueue process(const NDFA& ndfa, std::string state, const InputsQueue& inputs) { //start;a;a;b;c;d //
    TransitionsQueue tq; //typedef ics::pair<std::string,States>    typedef ics::ArrayQueue<Transitions>
    States states({state});
    Transitions t("",states); //string to a set pair
    tq.enqueue(t);
    States ch = states;
    states.clear();
    //int count = 0;
   // std::cout << "here : " << ndfa << std::endl;

    /*
     * start;a;1;a;2;b;2;c;3
        1;a;1;a;2
        2;c;3
        3;d;start;d;last
        last
     */
    for (auto i = inputs.begin(); i != inputs.end(); ++i) {
        for (auto &kv : ch) { //current states
            for (auto &p : ndfa) {
                if (p.first == kv) {
                    for (auto &it : p.second) {
                    if (it.first == *i) {
                        states.insert_all(it.second); //temp states
                        break;
                    }
                    }
                }
            }
        }
        t.first = *i;
        t.second = states;
        tq.enqueue(t);
        ch = states;
        states.clear();
    }





//    for (auto i = inputs.begin(); i != inputs.end(); ++i) {
//        for (auto &kv : ndfa) {
//        if () {
//            if (kv.second.has_key(*i)) {
//                t.first = *i;
//                for (auto it = kv.second.begin(); it != kv.second.end(); ++it) {
//                    if ((*it).first == *i) {
//                        t.second.insert_all((*it).second);
//                    }
//                   // std::cout << *it << std::endl;
//
//                }
//               // t.second = states;
//                tq.enqueue(t);
//                states.clear();
//            }
//        }
//           break;
//        }
//    }

    return tq;
}


//Print a TransitionsQueue (the result of calling process) in a nice form.
//Print the Start state on the first line; then print each input and the
//  resulting new states indented on subsequent lines; on the last line, print
//  the Stop state.
void interpret(TransitionsQueue& tq) {  //or TransitionsQueue or TransitionsQueue&&
        States state;

        for (auto i = tq.begin(); i != tq.end(); ++i) {
            state.clear();
            if (i == tq.begin()) {
                std::cout << "Start state = " << (*i).second << std::endl;
                continue;
            }
            std::cout << "Input = " << (*i).first << "; new possible states = " << (*i).second << std::endl;
            state.insert_all((*i).second);
        }

    std::cout << "Stop state(s) = " << state << std::endl;
    std::cout << std::endl;

}



//Prompt the user for a file, create a finite automaton Map, and print it.
//Prompt the user for a file containing any number of simulation descriptions
//  for the finite automaton to process, one description per line; each
//  description contains a start state followed by its inputs, all separated by
//  semicolons.
//Repeatedly read a description, print that description, put each input in a
//  Queue, process the Queue and print the results in a nice form.
int main() {
  try {
      std::string line;
      InputsQueue iq;
    std::ifstream file;
      ics::safe_open(file,"Enter some non-deterministic finite automaton file name", "../input files/ndfaendin01.txt");
      NDFA ndfa = read_ndfa(file);
      print_ndfa(ndfa);
      //std::cout << ndfa << std::endl;

      std::ifstream file2;
      ics::safe_open(file2, "Enter some file name with start-state and inputs","../input files/ndfainputendin01.txt");
      std::cout << std::endl;
      while(getline(file2, line)) {

          std::vector<std::string> vec = ics::split(line,";");
          std::string start = vec[0];
          for (int i = 1; i < vec.size(); i++) {
              iq.enqueue(vec[i]);
          }

          TransitionsQueue tq = process(ndfa, start, iq);
            //std::cout << tq << std::endl;
        std::cout << "Starting up a new simulation with description: ";
          for (int i = 0; i < vec.size(); i++) {
              std::cout << vec[i];
              if (i != vec.size() - 1) {
                  std::cout << ";";
              }
              else std::cout << "\n";
          }
          interpret(tq);
          iq.clear();

      }

      file.close();
      file2.close();

  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
