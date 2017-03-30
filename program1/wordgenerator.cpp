//// Submitter: Allan Eivazian 93668453
//// Partner: Tommy Le 52999842
//// We certify that we worked cooperatively on this programming
//// assignment, according to the rules for pair programming.
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <limits>                           //I used std::numeric_limits<int>::max()
//#include "ics46goody.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//
//typedef ics::ArrayQueue<std::string>         WordQueue;
//typedef ics::ArraySet<std::string>           FollowSet;
//typedef ics::pair<WordQueue,FollowSet>       CorpusEntry;
//typedef ics::ArrayPriorityQueue<CorpusEntry> CorpusPQ;     //Convenient to supply gt at construction
//typedef ics::ArrayMap<WordQueue,FollowSet>   Corpus;
//
//
////Return a random word in the words set (use in produce_text)
//std::string random_in_set(const FollowSet& words) {
//  int index = ics::rand_range(1, words.size());
//  int i = 0;
//  for (const std::string& s : words)
//    if (++i == index)
//      return s;
//  return "?";
//}
//
//
////Read an open file of lines of words (separated by spaces) and return a
////  Corpus (Map) of each sequence (Queue) of os (Order-Statistic) words
////  associated with the Set of all words that follow them somewhere in the
////  file.
//Corpus read_corpus(int os, std::ifstream &file) {
//    Corpus corp;
//    WordQueue words;
//    std::string line;
//
//    while(getline(file,line)) { //outer
//        std::vector<std::string> vec = ics::split(line," ");
//
//        for (auto i = vec.begin(); i != vec.end(); ++i) {
//            if (words.size() < os) {
//                words.enqueue(*i);
//            }
//            else {
//                corp[words].insert(*i);
//                words.dequeue();
//                words.enqueue(*i);
//            }
//        }
//    }
//    //std::cout << corp << std::endl;
//
//    return corp;
//}
//
//
////Print "Corpus" and all entries in the Corpus, in lexical alphabetical order
////  (with the minimum and maximum set sizes at the end).
////Use a "can be followed by any of" to separate the key word from the Set of words
////  that can follow it.
//
////One queue is lexically greater than another, if its first value is smaller; or if
////  its first value is the same and its second value is smaller; or if its first
////  and second values are the same and its third value is smaller...
////If any of its values is greater than the corresponding value in the other queue,
////  the first queue is not greater.
////Note that the queues sizes are the same: each stores Order-Statistic words
////Important: Use iterators for examining the queue values: DO NOT CALL DEQUEUE.
//
//bool queue_gt(const CorpusEntry& a, const CorpusEntry& b) {
//
//    for (auto i = a.first.begin(); i != a.first.end(); ++i) {
//        for (auto it = b.first.begin(); it != b.first.end(); ++it) {
//            if (*i != *it) { //regular just sort alpha
//                return *i < *it;
//            }
//        }
//    }
//}
//
//void print_corpus(const Corpus& corpus) {
//
//    CorpusPQ sortPQ(corpus,queue_gt);
//    Corpus corp;
//    corp.put_all(sortPQ);
//
//
//    std::cout << "Corpus had " << corpus.size() << " Entries" << std::endl;
//    int max = 0;
//    int min = std::numeric_limits<int>::max();
//
//    for (auto &kv : corp) {
//        std::cout << kv.first << " -> " << kv.second << std::endl;
//        if (kv.second.size() < min) {
//            min = kv.second.size();
//        }
//        if (kv.second.size() > max) {
//            max = kv.second.size();
//        }
//    }
//
//    std::cout << "Corpus had " << corpus.size() << " Entries" << std::endl;
//    std::cout << "max/min = " << max << "/" << min << std::endl;
//}
//
//
////Return a Queue of words, starting with those in start and including count more
////  randomly selected words using corpus to decide which word comes next.
////If there is no word that follows the previous ones, put "None" into the queue
////  and return immediately this list (whose size is <= start.size() + count).
//WordQueue produce_text(const Corpus& corpus, const WordQueue& start, int count) {
//
//    WordQueue nWords = start, genWords = start;
//    std::string rand = "";
//    int hold = count;
//    while (count != 0) {
//        if (hold - corpus.size() == count ) {
//            genWords.enqueue("None");
//            return genWords;
//        }
//
//            rand = random_in_set(corpus[nWords]);
//            nWords.enqueue(rand);
//            nWords.dequeue();
//            genWords.enqueue(rand);
//
//        count--;
//    }
//
//    return genWords;
//}
//
//
//
////Prompt the user for (a) the order statistic and (b) the file storing the text.
////Read the text as a Corpus and print it appropriately.
////Prompt the user for order statistic words from the text.
////Prompt the user for number of random words to generate
////Call the above functions to solve the problem, and print the appropriate information
//int main() {
//  try {
//      std::cout << "Enter some order statistic: ";
//      int num = 0;
//      int numWords;
//      WordQueue wq2;
//      WordQueue wq1;
//      std::cin >> num;
//      std::cin.ignore();
//      std::ifstream file;
//      ics::safe_open(file, "Enter some file name to process","../input files/wginput1.txt");
//      Corpus c = read_corpus(num,file);
//      print_corpus(c);
//
//      std::cout << "Enter " << num << " words for starting" << std::endl;
//
//      for (int i = 1; i <= num; i++) {
//          std::cout << "Enter word " << i << ": ";
//          std::string word;
//          getline(std::cin,word);
//          wq1.enqueue(word);
//      }
//
//        std::cout << "Enter # of words to generate: ";
//        std::cin >> numWords;
//        wq2 = produce_text(c, wq1,numWords);
//      std::cout << "Random text = " << wq2 << std::endl;
//        file.close();
//
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
