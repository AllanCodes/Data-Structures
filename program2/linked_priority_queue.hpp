// Submitter: Allan Eivazian 93668453
// Partner: Tommy Le 52999842
// We certify that we worked cooperatively on this programming
// assignment, according to the rules for pair programming.
#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "array_stack.hpp"      //See operator <<


namespace ics {


#ifndef undefinedgtdefined
#define undefinedgtdefined
    template<class T>
    bool undefinedgt (const T& a, const T& b) {return false;}
#endif /* undefinedgtdefined */

//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to undefinedgt in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedgt value supplied by tgt/cgt is stored in the instance variable gt.
    template<class T, bool (*tgt)(const T& a, const T& b) = undefinedgt<T>> class LinkedPriorityQueue {

        typedef bool (*gtfunc) (const T& a, const T& b);
    public:
        //Destructor/Constructors
        ~LinkedPriorityQueue();

        LinkedPriorityQueue          (bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
        LinkedPriorityQueue          (const LinkedPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
        explicit LinkedPriorityQueue (const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template <class Iterable>
        explicit LinkedPriorityQueue (const Iterable& i, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);


        //Queries
        bool empty      () const;
        int  size       () const;
        T&   peek       () const;
        std::string str () const; //supplies useful debugging information; contrast to operator <<


        //Commands
        int  enqueue (const T& element);
        T    dequeue ();
        void clear   ();

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template <class Iterable>
        int enqueue_all (const Iterable& i);


        //Operators
        LinkedPriorityQueue<T,tgt>& operator = (const LinkedPriorityQueue<T,tgt>& rhs);
        bool operator == (const LinkedPriorityQueue<T,tgt>& rhs) const;
        bool operator != (const LinkedPriorityQueue<T,tgt>& rhs) const;

        template<class T2, bool (*gt2)(const T2& a, const T2& b)>
        friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T2,gt2>& pq);



    private:
        class LN;

    public:
        class Iterator {
        public:
            //Private constructor called in begin/end, which are friends of LinkedPriorityQueue<T,tgt>
            ~Iterator();
            T           erase();
            std::string str  () const;
            LinkedPriorityQueue<T,tgt>::Iterator& operator ++ ();
            LinkedPriorityQueue<T,tgt>::Iterator  operator ++ (int);
            bool operator == (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const;
            bool operator != (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const;
            T& operator *  () const;
            T* operator -> () const;
            friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T,tgt>::Iterator& i) {
             outs << i.str(); //Use the same meaning as the debugging .str() method
             return outs;
            }
            friend Iterator LinkedPriorityQueue<T,tgt>::begin () const;
            friend Iterator LinkedPriorityQueue<T,tgt>::end   () const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            LN*             prev;            //initialize prev to the header
            LN*             current;         //current == prev->next
            LinkedPriorityQueue<T,tgt>* ref_pq;
            int             expected_mod_count;
            bool            can_erase = true;

            //Called in friends begin/end
            Iterator(LinkedPriorityQueue<T,tgt>* iterate_over, LN* initial);
        };


        Iterator begin () const;
        Iterator end   () const;


    private:
        class LN {
        public:
            LN ()                      {}
            LN (const LN& ln)          : value(ln.value), next(ln.next){}
            LN (T v,  LN* n = nullptr) : value(v), next(n){}

            T   value;
            LN* next = nullptr;
        };


        bool (*gt) (const T& a, const T& b); // The gt used by enqueue (from template or constructor)
        LN* front     =  new LN();
        int used      =  0;                  //Cache count of nodes in linked list
        int mod_count =  0;                  //Allows sensing concurrent modification

        //Helper methods
        void delete_list(LN*& front);        //Deallocate all LNs, and set front's argument to nullptr;
    };





////////////////////////////////////////////////////////////////////////////////
//
//LinkedPriorityQueue class and related definitions

//Destructor/Constructors
    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>::~LinkedPriorityQueue() {
     delete_list(front);
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(bool (*cgt)(const T& a, const T& b)) : gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt) {
     if (gt == undefinedgt<T>)
      throw TemplateFunctionError("LinkedPriorityQueue::default constructor: neither specified");
     if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
      throw TemplateFunctionError("LinkedPriorityQueue::default constructor: both specified and different");

     //nothing to create here
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const LinkedPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b))

            : gt(tgt != undefinedgt<T> ? tgt : cgt) {
     if (gt == undefinedgt<T>)
      gt = to_copy.gt;//throw TemplateFunctionError("ArrayPriorityQueue::copy constructor: neither specified");
     if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
      throw TemplateFunctionError("LinkedPriorityQueue::copy constructor: both specified and different");

     if (gt == to_copy.gt) {
      used = to_copy.used;
      LN* f = front;
      for (LN* p = to_copy.front->next; p != nullptr; p=p->next) {
       f->next = new LN(p->value);
       f = f->next;
      }
     }else
      for (LN* p = to_copy.front->next; p != nullptr; p = p->next) {
       enqueue(p->value);
      }

    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
            : gt(tgt != undefinedgt<T> ? tgt : cgt) {
     if (gt == undefinedgt<T>)
      throw TemplateFunctionError("LinkedPriorityQueue::initializer_list constructor: neither specified");
     if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
      throw TemplateFunctionError("LinkedPriorityQueue::initializer_list constructor: both specified and different");


     LN* f = front;

     for (const T& elem : il) {
      enqueue(elem);
     }
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    template<class Iterable>
    LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
            : gt(tgt != undefinedgt<T> ? tgt : cgt) {
     if (gt == undefinedgt<T>)
      throw TemplateFunctionError("LinkedPriorityQueue::Iterable constructor: neither specified");
     if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
      throw TemplateFunctionError("LinkedPriorityQueue::Iterable constructor: both specified and different");

     for (const T& elem : i) {
      enqueue(elem);
     }
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template<class T, bool (*tgt)(const T& a, const T& b)>
    bool LinkedPriorityQueue<T,tgt>::empty() const {

     return used == 0;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    int LinkedPriorityQueue<T,tgt>::size() const {

     return used;
    }



    template<class T, bool (*tgt)(const T& a, const T& b)>
    T& LinkedPriorityQueue<T,tgt>::peek () const {
     if (empty())
      throw EmptyError("LinkedPriorityQueue::peek");

     return front->next->value;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    std::string LinkedPriorityQueue<T,tgt>::str() const {
     std::ostringstream answer;
     answer << "LinkedPriorityQueue[HEADER->";

     if (used != 0) {
      for (auto p = this->front->next; p != nullptr; p=p->next) {
       answer << p->value;
       if (p->next != nullptr) {
        answer << "->";
       }
      }
     }


     answer << "](used=" << used << ",front" << front << ",mod_count=" << mod_count << ")";
     return answer.str();
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template<class T, bool (*tgt)(const T& a, const T& b)>
    int LinkedPriorityQueue<T,tgt>::enqueue(const T& element) {

     LN* f = front;
     for (; f->next != nullptr; f = f->next) {
      if (gt(f->next->value, element)) {
              continue;
      }
      else break;
     }

     f->next = new LN(element, f->next);



     ++mod_count;
     ++used;
     return 1;

    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    T LinkedPriorityQueue<T,tgt>::dequeue() {
     if (this->empty())
      throw EmptyError("ArrayPriorityQueue::dequeue");

     T to_return = front->next->value;
     LN* to_delete = front->next;
     front->next = front->next->next;
     delete to_delete;
     --used;
     ++mod_count;

     return to_return;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    void LinkedPriorityQueue<T,tgt>::clear() {
     delete_list(front->next);
     front->next = nullptr;
     used = 0;
     ++mod_count;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    template <class Iterable>
    int LinkedPriorityQueue<T,tgt>::enqueue_all (const Iterable& i) {
     int count = 0;
     for (const T& v : i) {
      count++;
      enqueue(v);
     }

     return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators
 //WORK ON THIS
    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>& LinkedPriorityQueue<T,tgt>::operator = (const LinkedPriorityQueue<T,tgt>& rhs) {
     if (this == &rhs)
      return *this;

     gt = rhs.gt;
     if (this->used == 0 && rhs.used == 0) {
      return *this;
     }

     LN* l = front;
     if (this->used != 0) {
      for (LN* r = rhs.front->next; r!= nullptr; r = r->next) {
       if (l->next == nullptr) {
           l->next = new LN(r->value);
           l = l->next;
        continue;
       }
       else {
        l->next->value = r->value;
        if (l->next->next != nullptr && r->next == nullptr) {
         //std::cout << l << std::endl;
         delete_list(l->next->next);
         l->next->next = nullptr;
         r = r->next;
         break;
        }
        l = l->next;
       }

      }
     }
     else {
      LN* f = front;
       for (LN* r = rhs.front->next; r != nullptr; r= r->next) {
        f->next = new LN(r->value);
        f = f->next;
        }
     }
     this->used = rhs.used;
     ++mod_count;
     return *this;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    bool LinkedPriorityQueue<T,tgt>::operator == (const LinkedPriorityQueue<T,tgt>& rhs) const {
     if (this == &rhs)
      return true;

     if (gt != rhs.gt)
      return false;

     if (used != rhs.size())
      return false;

     LinkedPriorityQueue<T, tgt>::Iterator rhs_i = rhs.begin();

     for (LN* f = front->next; f != nullptr; f = f->next, ++rhs_i) {
      if (f->value != *rhs_i) {
       return false;
      }
     }

     return true;

    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    bool LinkedPriorityQueue<T,tgt>::operator != (const LinkedPriorityQueue<T,tgt>& rhs) const {
     return !(*this == rhs);
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T,tgt>& pq) {
     outs << "priority_queue[";
     ics::ArrayStack<T> stack;

     if (!pq.empty()) {
      for (auto p = pq.front->next; p != nullptr; p = p->next) {
       stack.push(p->value);
      }
      outs << stack.pop();

     // outs << pq.front->next->value;
      while(!stack.empty()) {
       outs << "," << stack.pop();
      }
     }

     outs << "]:highest";
     return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors


    template<class T, bool (*tgt)(const T& a, const T& b)>
    auto LinkedPriorityQueue<T,tgt>::begin () const -> LinkedPriorityQueue<T,tgt>::Iterator {
     return  Iterator(const_cast<LinkedPriorityQueue<T,tgt>*> (this), front->next);
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    auto LinkedPriorityQueue<T,tgt>::end () const -> LinkedPriorityQueue<T,tgt>::Iterator {
     return Iterator(const_cast<LinkedPriorityQueue<T,tgt>*> (this), nullptr);
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template<class T, bool (*tgt)(const T& a, const T& b)>
    void LinkedPriorityQueue<T,tgt>::delete_list(LN*& front) {

     for (LN* p = front; p != nullptr;) {
      LN* to_delete = p;
      p = p->next;
      delete to_delete;
     }
     front = nullptr;
    }

////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>::Iterator::Iterator(LinkedPriorityQueue<T,tgt>* iterate_over, LN* initial)
            : current(initial), ref_pq(iterate_over), expected_mod_count(ref_pq->mod_count) ,prev(iterate_over->front) {
    };


    template<class T, bool (*tgt)(const T& a, const T& b)>
    LinkedPriorityQueue<T,tgt>::Iterator::~Iterator()
    {}


    template<class T, bool (*tgt)(const T& a, const T& b)>
    T LinkedPriorityQueue<T,tgt>::Iterator::erase() {
     if (expected_mod_count != ref_pq->mod_count)
      throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::erase");
     if (!can_erase)
      throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");
     if (current == nullptr)
      throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

     can_erase = false;
     T to_return = current->value;
     //ref_pq->erase_at(current);
     LN* to_delete = current;
     current = prev->next = current->next;
     delete to_delete;
     ++ref_pq->mod_count;
     expected_mod_count = ref_pq->mod_count;
     (ref_pq->used)--;
     return to_return;

    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    std::string LinkedPriorityQueue<T,tgt>::Iterator::str() const {
     std::ostringstream answer;
     answer << ref_pq->str() << "/current=" << current << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
     return answer.str();
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    auto LinkedPriorityQueue<T,tgt>::Iterator::operator ++ () -> LinkedPriorityQueue<T,tgt>::Iterator& {
     if (expected_mod_count != ref_pq->mod_count)
      throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++");

     if (current == nullptr)
      return *this;

     if (can_erase) {
      prev = current;         //decreasing priority goes backward in array, towards 0
      current = current->next;
     }
     else
      can_erase = true;  //current already indexes "one beyond" deleted value

     return *this;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    auto LinkedPriorityQueue<T,tgt>::Iterator::operator ++ (int) -> LinkedPriorityQueue<T,tgt>::Iterator {
     if (expected_mod_count != ref_pq->mod_count)
      throw ConcurrentModificationError("linkedPriorityQueue::Iterator::operator ++(int)");

     if (current == nullptr)
      return *this;

     Iterator to_return(*this);
     if (can_erase) {
      prev = current;         //decreasing priority goes backward in array, towards 0
      current = prev->next;
     }
     else
      can_erase = true;  //current already indexes "one beyond" deleted value

     return to_return;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    bool LinkedPriorityQueue<T,tgt>::Iterator::operator == (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const {
     const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
     if (rhsASI == 0)
      throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator ==");
     if (expected_mod_count != ref_pq->mod_count)
      throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ==");
     if (ref_pq != rhsASI->ref_pq)
      throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator ==");

     return current == rhsASI->current;
    }


    template<class T, bool (*tgt)(const T& a, const T& b)>
    bool LinkedPriorityQueue<T,tgt>::Iterator::operator != (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const {
     const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
     if (rhsASI == 0)
      throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator !=");
     if (expected_mod_count != ref_pq->mod_count)
      throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator !=");
     if (ref_pq != rhsASI->ref_pq)
      throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator !=");

     return current != rhsASI->current;
    }

    template<class T, bool (*tgt)(const T& a, const T& b)>
    T& LinkedPriorityQueue<T,tgt>::Iterator::operator *() const {
     if (expected_mod_count != ref_pq->mod_count)
      throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator *");
     if (!can_erase || current == nullptr) {
      std::ostringstream where;
      where << current << " when size = " << ref_pq->size();
      throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
     }

     return current->value;
    }

    template<class T, bool (*tgt)(const T& a, const T& b)>
    T* LinkedPriorityQueue<T,tgt>::Iterator::operator ->() const {
     if (expected_mod_count !=  ref_pq->mod_count)
      throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ->");
     if (!can_erase || current == nullptr) {
      std::ostringstream where;
      where << current << " when size = " << ref_pq->size();
      throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator -> Iterator illegal: "+where.str());
     }

     return &current->value;
    }


}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */