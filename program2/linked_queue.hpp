// Submitter: Allan Eivazian 93668453
// Partner: Tommy Le 52999842
// We certify that we worked cooperatively on this programming
// assignment, according to the rules for pair programming.
#ifndef LINKED_QUEUE_HPP_
#define LINKED_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"


namespace ics {


    template<class T>
    class LinkedQueue {
    public:
        //Destructor/Constructors
        ~LinkedQueue();

        LinkedQueue();

        LinkedQueue(const LinkedQueue<T> &to_copy);

        explicit LinkedQueue(const std::initializer_list<T> &il);

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template<class Iterable>
        explicit LinkedQueue(const Iterable &i);


        //Queries
        bool empty() const;

        int size() const;

        T &peek() const;

        std::string str() const; //supplies useful debugging information; contrast to operator <<


        //Commands
        int enqueue(const T &element);

        T dequeue();

        void clear();

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template<class Iterable>
        int enqueue_all(const Iterable &i);


        //Operators
        LinkedQueue<T> &operator=(const LinkedQueue<T> &rhs);

        bool operator==(const LinkedQueue<T> &rhs) const;

        bool operator!=(const LinkedQueue<T> &rhs) const;

        template<class T2>
        friend std::ostream &operator<<(std::ostream &outs, const LinkedQueue<T2> &q);


    private:
        class LN;

    public:
        class Iterator {
        public:
            //Private constructor called in begin/end, which are friends of LinkedQueue<T>
            ~Iterator();

            T erase();

            std::string str() const;

            LinkedQueue<T>::Iterator &operator++();

            LinkedQueue<T>::Iterator operator++(int);

            bool operator==(const LinkedQueue<T>::Iterator &rhs) const;

            bool operator!=(const LinkedQueue<T>::Iterator &rhs) const;

            T &operator*() const;

            T *operator->() const;

            friend std::ostream &operator<<(std::ostream &outs, const LinkedQueue<T>::Iterator &i) {
             outs << i.str(); //Use the same meaning as the debugging .str() method
             return outs;
            }

            friend Iterator LinkedQueue<T>::begin() const;

            friend Iterator LinkedQueue<T>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            LN *prev = nullptr;  //if nullptr, current at front of list
            LN *current;         //current == prev->next (if prev != nullptr)
            LinkedQueue<T> *ref_queue;
            int expected_mod_count;
            bool can_erase = true;

            //Called in friends begin/end
            Iterator(LinkedQueue<T> *iterate_over, LN *initial);
        };


        Iterator begin() const;

        Iterator end() const;


    private:
        class LN {
        public:
            LN() {
            }

            LN(const LN &ln)
                    : value(ln.value), next(ln.next) {
            }

            LN(T v, LN *n = nullptr)
                    : value(v), next(n) {
            }

            T value;
            LN *next = nullptr;
        };


        LN *front = nullptr;
        LN *rear = nullptr;
        int used = 0;            //Cache count of nodes in linked list
        int mod_count = 0;            //Alllows sensing concurrent modification

        //Helper methods
        void delete_list(LN *&front);  //Deallocate all LNs, and set front's argument to nullptr;

    };





    ////////////////////////////////////////////////////////////////////////////////
    //
    //LinkedQueue class and related definitions

    //Destructor/Constructors

    template<class T>
    LinkedQueue<T>::~LinkedQueue() {
     delete_list(front);

    }


    template<class T>
    LinkedQueue<T>::LinkedQueue() {

    }


    template<class T>
    LinkedQueue<T>::LinkedQueue(const LinkedQueue<T> &to_copy) {
     for (LN *p = to_copy.front; p != nullptr; p = p->next) {
      enqueue(p->value);
     }
     //    front = to_copy.front;
     //    rear = to_copy.rear;
     //    used = to_copy.size();

    }


    template<class T>
    LinkedQueue<T>::LinkedQueue(const std::initializer_list<T> &il) {
     for (const T &n : il) {
      enqueue(n);
     }
    }


    template<class T>
    template<class Iterable>
    LinkedQueue<T>::LinkedQueue(const Iterable &i) {
     for (const T &n : i) {
      enqueue(n);
     }
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    //Queries

    template<class T>
    bool LinkedQueue<T>::empty() const {
     if (!used) {
      return true;
     }

     return false;
    }


    template<class T>
    int LinkedQueue<T>::size() const {
     return used;
    }


    template<class T>
    T &LinkedQueue<T>::peek() const {

     if (this->empty()) {
      throw EmptyError("LinkedQueue::peek");
     }

     return front->value;
    }


    template<class T>
    std::string LinkedQueue<T>::str() const {
     std::ostringstream answer;
     answer << "LinkedQueue[";

     if (used != 0) {
      for (auto p = this->front; p != nullptr; p = p->next) {
       answer << p->value;
       if (p->next != nullptr) {
        answer << "->";
       }
      }
      // answer << "]";
     }

     answer << "](used=" << used << ",front=" << front << ",rear=" << rear << ",mod_count=" << mod_count << ")";
     return answer.str();
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    //Commands

    template<class T>
    int LinkedQueue<T>::enqueue(const T &element) {

     if (front == nullptr) {
      front = rear = new LN(element);
     } else {
      rear = rear->next = new LN(element);
     }

     used++;
     mod_count++;


     return 1;
    }


    template<class T>
    T LinkedQueue<T>::dequeue() {

     if (this->empty())
      throw EmptyError("LinkedQueue::dequeue");


     T tmp = front->value;
     LN *to_delete = front;
     front = front->next;
     delete to_delete;
     --used;
     mod_count++;

     return tmp;
    }


    template<class T>
    void LinkedQueue<T>::clear() {
     delete_list(this->front);
     front = nullptr;
     rear = nullptr;
     used = 0;
     ++mod_count;

    }

    //get this working - WORKS AFTER ITERATORS ARE DEFINED
    template<class T>
    template<class Iterable>
    int LinkedQueue<T>::enqueue_all(const Iterable &i) {
     int count = 0;
     for (const T &n : i) {
      count++;
      enqueue(n);
     }

     return count;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    //Operators

    //get this working
    template<class T>
    LinkedQueue<T> &LinkedQueue<T>::operator=(const LinkedQueue<T> &rhs) {
     if (this == &rhs)
      return *this;

     LN *r = rhs.front;
     LN *l = this->front;
     LN *prev = this->front;
     for (; r != nullptr; r = r->next) { //how many are on the right side

      if (l == this->front && this->front == nullptr) {
       l = new LN(r->value);
       this->front = l;

       prev = rear = l;
       l = l->next = nullptr;
       continue;
      } else if (l != nullptr) {
       l->value = r->value;
       prev = l;
       rear = prev;
       l = l->next;
       if (r->next == nullptr && l != nullptr) {
        //                 std::cout << l << std::endl;
        //        std::cout << this->front->next->next << std::endl;
        rear->next = nullptr;
        delete_list(l);
        break;
       }
       continue;
      } else if (l == nullptr) {
       l = new LN(r->value);
       prev = prev->next = l;
       rear = l;
       l = l->next;
       continue;
      }
     }

     //std::cout << this->front->next << std::endl;
     // delete_list(this->front->next->next);

     //     delete_list(this->front->next->next);
     //std::cout << l->value << std::endl;
     //     if (l != nullptr && r == nullptr) {
     //        delete_list(this->front->next->next);
     //     }
     //     std::cout << this->front->value << std::endl;
     //     std::cout << this->front->next->value << std::endl;
     //     std::cout << this->front->next << std::endl;
     // std::cout << this->front->next->next->value << std::endl;
     ++mod_count;
     used = rhs.size();

     return *this;

    }


    template<class T>
    bool LinkedQueue<T>::operator==(const LinkedQueue<T> &rhs) const {
     if (this == &rhs)
      return true;

     int used = this->size();
     if (used != rhs.size())
      return false;

     LinkedQueue<T>::Iterator rhs_i = rhs.begin();
     for (LN *p = this->front; p != nullptr; p = p->next, ++rhs_i) {
      if (p->value != *rhs_i) {
       return false;
      }
     }

     return true;
    }


    template<class T>
    bool LinkedQueue<T>::operator!=(const LinkedQueue<T> &rhs) const {
     return !(*this == rhs);
    }


    template<class T>
    std::ostream &operator<<(std::ostream &outs, const LinkedQueue<T> &q) {
     outs << "queue[";

     if (!q.empty()) {
      outs << q.front->value;
      for (typename LinkedQueue<T>::LN *p = q.front->next; p != nullptr; p = p->next) {
       outs << "," << p->value;
      }
     }

     outs << "]:rear";
     return outs;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    //Iterator constructors

    template<class T>
    auto LinkedQueue<T>::begin() const -> LinkedQueue<T>::Iterator {
     return Iterator(const_cast<LinkedQueue<T> *>(this), front);
    }

    template<class T>
    auto LinkedQueue<T>::end() const -> LinkedQueue<T>::Iterator {
     return Iterator(const_cast<LinkedQueue<T> *>(this), rear->next); //must be next for nullptr
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    //Private helper methods

    template<class T>
    void LinkedQueue<T>::delete_list(LN *&front) {

     for (LN *p = front; p != nullptr;) {
      LN *to_delete = p;
      p = p->next;
      delete to_delete;
     }
     front = nullptr;

    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    //Iterator class definitions

    template<class T>
    LinkedQueue<T>::Iterator::Iterator(LinkedQueue<T> *iterate_over, LN *initial)
            : current(initial), ref_queue(iterate_over), expected_mod_count(ref_queue->mod_count) {
    }


    template<class T>
    LinkedQueue<T>::Iterator::~Iterator() {
    }

    //work on this, try increasing mod counts, missed any?
    template<class T>
    T LinkedQueue<T>::Iterator::erase() {
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::erase");
     if (!can_erase)
      throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor already erased");
     if (current == nullptr)
      throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor beyond data structure");

     can_erase = false;
     T to_return = current->value; //erased

     if (ref_queue->front == current) {
      ref_queue->front = ref_queue->front->next;
      delete current;
      current = ref_queue->front;
      ref_queue->mod_count++; //same as erase_at has to mod_count
     } else if (ref_queue->rear == current) {
      prev->next = ref_queue->rear->next; //should be nullptr?
      delete current;
      ref_queue->rear = prev;
      current = prev->next; //same as prev->next below
      ref_queue->mod_count++;
     } else {
      prev->next = current->next;
      delete current;
      current = prev->next;
      ref_queue->mod_count++;
     }

     expected_mod_count = ref_queue->mod_count;
     (ref_queue->used)--;
     return to_return;


    }


    template<class T>
    std::string LinkedQueue<T>::Iterator::str() const {
     std::ostringstream answer;
     answer << ref_queue->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count
            << ",can_erase=" << can_erase << ")";
     return answer.str();
    }


    template<class T>
    auto LinkedQueue<T>::Iterator::operator++() -> LinkedQueue<T>::Iterator & {
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++");

     if (current == nullptr)
      return *this;

     if (can_erase) {
      prev = current;
      current = current->next;
     } else
      can_erase = true;

     return *this;
    }


    template<class T>
    auto LinkedQueue<T>::Iterator::operator++(int) -> LinkedQueue<T>::Iterator {
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++(int)");

     if (current == nullptr)
      return *this;

     Iterator to_return(*this);
     if (can_erase) {
      prev = current;
      current = current->next;
     } else
      can_erase = true;

     return to_return;
    }


    template<class T>
    bool LinkedQueue<T>::Iterator::operator==(const LinkedQueue<T>::Iterator &rhs) const {
     const Iterator *rhsASI = dynamic_cast<const Iterator *>(&rhs);
     if (rhsASI == 0)
      throw IteratorTypeError("LinkedQueue::Iterator::operator ==");
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::operator ==");
     if (ref_queue != rhsASI->ref_queue)
      throw ComparingDifferentIteratorsError("LinkedQueue::Iterator::operator ==");

     return current == rhsASI->current;
    }


    template<class T>
    bool LinkedQueue<T>::Iterator::operator!=(const LinkedQueue<T>::Iterator &rhs) const {
     const Iterator *rhsASI = dynamic_cast<const Iterator *>(&rhs);
     if (rhsASI == 0)
      throw IteratorTypeError("LinkedQueue::Iterator::operator !=");
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::operator !=");
     if (ref_queue != rhsASI->ref_queue)
      throw ComparingDifferentIteratorsError("LinkedQueue::Iterator::operator !=");

     return current != rhsASI->current;
    }


    template<class T>
    T &LinkedQueue<T>::Iterator::operator*() const {
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::operator *");
     if (!can_erase || current == nullptr) { //must be empty
      std::ostringstream where;
      where << current << " when front = " << ref_queue->front << " and rear = " << ref_queue->rear->value;
      throw IteratorPositionIllegal("LinkedQueue::Iterator::operator * Iterator illegal: " + where.str());
     }

     return current->value;
    }

    template<class T>
    T *LinkedQueue<T>::Iterator::operator->() const {
     if (expected_mod_count != ref_queue->mod_count)
      throw ConcurrentModificationError("LinkedQueue::Iterator::operator ->");
     if (!can_erase || current == nullptr) {
      std::ostringstream where;
      where << current << " when front = " << ref_queue->front << " and rear = " << ref_queue->rear;
      throw IteratorPositionIllegal("LinkedQueue::Iterator::operator -> Iterator illegal: " + where.str());
     }

     return &current->value;
    }


}

#endif /* LINKED_QUEUE_HPP_ */