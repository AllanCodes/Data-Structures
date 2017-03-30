//// Submitter: Allan Eivazian 93668453
//// Partner: Tommy Le 52999842
//// We certify that we worked cooperatively on this programming
//// assignment, according to the rules for pair programming.
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include "ics46goody.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//
//typedef ics::ArrayQueue<std::string>                InputsQueue;
//typedef ics::ArrayMap<std::string,std::string>      InputStateMap;
//
//typedef ics::ArrayMap<std::string,InputStateMap>    FA;
//typedef ics::pair<std::string,InputStateMap>        FAEntry;
//
//bool gt_FAEntry (const FAEntry& a, const FAEntry& b)
//{return a.first<b.first;}
//
//typedef ics::ArrayPriorityQueue<FAEntry,gt_FAEntry> FAPQ;
//
//typedef ics::pair<std::string,std::string>          Transition;
//typedef ics::ArrayQueue<Transition>                 TransitionQueue;
//
//
////Read an open file describing the finite automaton (each line starts with
////  a state name followed by pairs of transitions from that state: (input
////  followed by new state, all separated by semicolons), and return a Map
////  whose keys are states and whose associated values are another Map with
////  each input in that state (keys) and the resulting state it leads to.
//const FA read_fa(std::ifstream &file) {
//
//    FA map;
//    std::string line = ""; //current line
//    while(getline(file,line)) {
//
//        std::vector<std::string> vec = ics::split(line,";");
//        InputStateMap tmp;
//        for (auto i = 1; i < vec.size();) {
//            if (i % 2 != 0) {
//                tmp[vec[i]] = vec[i + 1];
//                i += 2;
//            }
//            else i++;
//        }
//        map[vec[0]] = tmp;
//        tmp.clear();
//    }
//
//    return map;
//}
//
//
////Print a label and all the entries in the finite automaton Map, in
////  alphabetical order of the states: each line has a state, the text
////  "transitions:" and the Map of its transitions.
//void print_fa(const FA& fa) {
//    std::cout << "The Finite Automaton Description" << std::endl;
//
//    for (auto &kv : fa) {
//        std::cout << kv.first << " transitions: " << kv.second << std::endl;
//    }
//}
//
//
////Return a queue of the calculated transition pairs, based on the finite
////  automaton, initial state, and queue of inputs; each pair in the returned
////  queue is of the form: input, new state.
////The first pair contains "" as the input and the initial state.
////If any input i is illegal (does not lead to a state in the finite
////  automaton), then the last pair in the returned queue is i,"None".
//TransitionQueue process(const FA& fa, std::string state, const InputsQueue& inputs) { //aq<string>
//    TransitionQueue tq; //tq<pair<string,string>>
//    Transition t({"",state});
//    tq.enqueue(t);
//
//    bool ch = false;
//    for (auto st = inputs.begin(); st != inputs.end(); ++st) {
//        for (auto &kv : fa) {
//            if (kv.first == state) {
//                if (!kv.second.has_key(*st)) {
//                    t.first = *st;
//                    t.second = "None";
//                    tq.enqueue(t);
//                    ch = true;
//                    break;
//                }
//                else {
//                    t.first = *st;
//                    for (auto &p : kv.second) {
//                        if (*st == p.first) {
//                            state = p.second;
//                            t.second = p.second;
//                            tq.enqueue(t);
//                            break;
//                        }
//                    }
//                    break;
//                }
//            }
//        }
//        if (ch) {
//            break;
//        }
//    }
//    return tq;
//
//}
//
//
////Print a TransitionQueue (the result of calling the process function above)
//// in a nice form.
////Print the Start state on the first line; then print each input and the
////  resulting new state (or "illegal input: terminated", if the state is
////  "None") indented on subsequent lines; on the last line, print the Stop
////  state (which may be "None").
//void interpret(TransitionQueue& tq) {  //or TransitionQueue or TransitionQueue&&
//    std::string state = "";
//    for (auto i = tq.begin(); i != tq.end(); ++i) {
//        state = (*i).second;
//
//        if (i == tq.begin()) {
//            std::cout << "Start state = " << (*i).second << std::endl;
//        }
//        else if((*i).second == "None") {
//        std::cout << "Input = " << (*i).first << "; illegal input: simulation terminated" << std::endl;
//        }
//        else {
//            std::cout << "Input = " << (*i).first << "; new state = " << (*i).second << std::endl;
//        }
//    }
//    std::cout << "Stop state = " << state << std::endl;
//    std::cout << "\n";
//}
//
//
//
////Prompt the user for a file, create a finite automaton Map, and print it.
////Prompt the user for a file containing any number of simulation descriptions
////  for the finite automaton to process, one description per line; each
////  description contains a start state followed by its inputs, all separated by
////  semicolons.
////Repeatedly read a description, print that description, put each input in a
////  Queue, process the Queue and print the results in a nice form.
//int main() {
//  try {
//    std::string line;
//      InputsQueue iq;
//    std::ifstream file;
//    ics::safe_open(file, "Enter some finite automaton file name", "../input files/faparity.txt");
//    FA fa = read_fa(file);
//    print_fa(fa);
//      std::cout << std::endl;
//
//      std::ifstream file2;
//      ics::safe_open(file2, "Enter some file name with start-state and inputs", "../input files/fainputparity.txt");
//      while(getline(file2,line)) {
//
//          std::vector<std::string> vec = ics::split(line,";");
//          std::string start = vec[0];
//            for (int i = 1; i < vec.size(); i++) {
//                iq.enqueue(vec[i]);
//            }
//
//          TransitionQueue tq = process(fa,start,iq);
//
//          std::cout << "Starting up a new simulation with description: ";
//          for (int i = 0; i < vec.size(); i++) {
//              std::cout << vec[i];
//              if (i != vec.size() - 1) {
//                  std::cout << ";";
//              }
//              else std::cout << "\n";
//          }
//          interpret(tq);
//          iq.clear();
//      }
//
//    file.close();
//      file2.close();
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
