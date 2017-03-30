// Submitter: Allan Eivazian 93668453
// Partner: Tommy Le 52999842
// We certify that we worked cooperatively on this programming
// assignment, according to the rules for pair programming.
#ifndef LINKED_SET_HPP_
#define LINKED_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"


namespace ics {


template<class T> class LinkedSet {
  public:
    //Destructor/Constructors
    ~LinkedSet();

    LinkedSet          ();
    explicit LinkedSet (int initialLength);
    LinkedSet          (const LinkedSet<T>& to_copy);
    explicit LinkedSet (const std::initializer_list<T>& il);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit LinkedSet (const Iterable& i);


    //Queries
    bool empty      () const;
    int  size       () const;
    bool contains   (const T& element) const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    bool contains_all (const Iterable& i) const;


    //Commands
    int  insert (const T& element);
    int  erase  (const T& element);
    void clear  ();

    //Iterable class must support "for" loop: .begin()/.end() and prefix ++ on returned result

    template <class Iterable>
    int insert_all(const Iterable& i);

    template <class Iterable>
    int erase_all(const Iterable& i);

    template<class Iterable>
    int retain_all(const Iterable& i);


    //Operators
    LinkedSet<T>& operator = (const LinkedSet<T>& rhs);
    bool operator == (const LinkedSet<T>& rhs) const;
    bool operator != (const LinkedSet<T>& rhs) const;
    bool operator <= (const LinkedSet<T>& rhs) const;
    bool operator <  (const LinkedSet<T>& rhs) const;
    bool operator >= (const LinkedSet<T>& rhs) const;
    bool operator >  (const LinkedSet<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedSet<T2>& s);



  private:
    class LN;

  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of LinkedSet<T>
        ~Iterator();
        T           erase();
        std::string str  () const;
        LinkedSet<T>::Iterator& operator ++ ();
        LinkedSet<T>::Iterator  operator ++ (int);
        bool operator == (const LinkedSet<T>::Iterator& rhs) const;
        bool operator != (const LinkedSet<T>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const LinkedSet<T>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator LinkedSet<T>::begin () const;
        friend Iterator LinkedSet<T>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        LN*           current;  //if can_erase is false, this value is unusable
        LinkedSet<T>* ref_set;
        int           expected_mod_count;
        bool          can_erase = true;

        //Called in friends begin/end
        Iterator(LinkedSet<T>* iterate_over, LN* initial);
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
        LN* next   = nullptr;
    };


    LN* front     = new LN();
    LN* trailer   = front;         //Always point to the special trailer LN
    int used      =  0;            //Cache the number of values in linked list
    int mod_count = 0;             //For sensing concurrent modification

    //Helper methods
    int  erase_at   (LN* p);
    void delete_list(LN*& front);  //Deallocate all LNs (but trailer), and set front's argument to trailer;
};





////////////////////////////////////////////////////////////////////////////////
//
//LinkedSet class and related definitions

//Destructor/Constructors

template<class T>
LinkedSet<T>::~LinkedSet() {
    delete_list(front);
    delete trailer; //trailer will NOT be deleting in delete_list
}


template<class T>
LinkedSet<T>::LinkedSet() {
}


template<class T>
LinkedSet<T>::LinkedSet(const LinkedSet<T>& to_copy) {
    for (const T& elem : to_copy) {
        insert(elem);
    }
}


template<class T>
LinkedSet<T>::LinkedSet(const std::initializer_list<T>& il) {
    for (const T& elem : il) {
        insert(elem);
    }
}


template<class T>
template<class Iterable>
LinkedSet<T>::LinkedSet(const Iterable& i) {
    for (const T& elem : i) {
        insert(elem);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T>
bool LinkedSet<T>::empty() const {
    return used == 0;
}


template<class T>
int LinkedSet<T>::size() const {
    return used;
}


template<class T>
bool LinkedSet<T>::contains (const T& element) const {

    LN* f = front;

    for (LN* f = front; f != trailer; f = f->next) {
        if (f->value == element) {
            return true;
        }
    }

    return false;
}


template<class T>
std::string LinkedSet<T>::str() const {
    std::ostringstream answer;
    answer << "LinkedSet[";

    if (used != 0) {
        answer << front->value;
        for (auto p = front->next; p != trailer; p = p->next) {
            answer << "->" << p->value;
        }
    }

    answer << "->TRAILER](used=" << used << ",front=" << front << ",trailer=" << trailer <<  ",mod_count=" << mod_count << ")";
    return answer.str();
}


template<class T>
template<class Iterable>
bool LinkedSet<T>::contains_all (const Iterable& i) const {
    for (const T& elem : i) {
        if (!contains(elem)) {
            return false;
        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands


template<class T>
int LinkedSet<T>::insert(const T& element) {
    if (this->contains(element)) {
        return 0;
    }

    if (used == 0) {
        front->value = element;
        trailer = front->next = new LN();
        ++used;
        ++mod_count;
        return 1;
    }
    front = new LN(element, front);
    //trailer = new LN(element, new LN()); hmm..

    ++used;
    ++mod_count;
    return 1;
}


template<class T>
int LinkedSet<T>::erase(const T& element) {

    for (LN* f = front; f != trailer; f=f->next) {
        if (f->value == element) {
            return erase_at(f);
        }
    }

    return 0;
}


template<class T>
void LinkedSet<T>::clear() {

    delete_list(front);
    used = 0;
    ++mod_count;
}


template<class T>
template<class Iterable>
int LinkedSet<T>::insert_all(const Iterable& i) {
    int count = 0;
    for (const T& elem : i) {
        count += insert(elem);
    }

    return count;
}


template<class T>
template<class Iterable>
int LinkedSet<T>::erase_all(const Iterable& i) {
    int count = 0;
    for (const T& elem : i)
    count += erase(elem);

    return count;
}


template<class T>
template<class Iterable>
int LinkedSet<T>::retain_all(const Iterable& i) {
    LinkedSet s(i); //Can use LinkedSet
    int count = 0;

    for (LN* f = front; f != trailer; ) {
        if (!s.contains(f->value)) {
            erase_at(f);
            ++count;
        }
        else f = f->next;
    }

    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T>
LinkedSet<T>& LinkedSet<T>::operator = (const LinkedSet<T>& rhs) {
    if (this == &rhs)
        return *this;

    LN* l= front;
    if (this->used != 0) {
        for (LN* r = rhs.front; r != rhs.trailer; r = r->next) {
//            if (l->next == trailer) {
//                l->next = new LN(r->value,l->next);
//                trailer = l->next;
//                l = l->next;
//                continue;
//            }
            if (l == trailer) {
                trailer->value = r->value;
                trailer = trailer->next = new LN();
                l = l->next;
                continue;
            }
            else {
                l->value = r->value;
                if (l->next != trailer && r->next == rhs.trailer) {
                    delete_list(l->next);
                    l->next = trailer;
                    break;
                }
                l = l->next;
            }
        }
    }
    else {
        LN* f = this->front;
        for (LN* r = rhs.front; r != rhs.trailer; r=r->next) {
            if (used == 0) {
                f->value = r->value;
                trailer = f->next = new LN();
                used++;
                continue;
            }
            trailer->value = r->value;
            trailer = trailer->next = new LN();

        }
    }


    this->used = rhs.used;
    ++mod_count;
    return *this;
//    clear();
//    for (auto p : rhs){
//        this->insert(p);
//    }
//    return *this;
}


template<class T>
bool LinkedSet<T>::operator == (const LinkedSet<T>& rhs) const {
    if (this == &rhs)
        return true;

    if (used != rhs.size())
        return false;

     for (LN* f = front; f != trailer; f = f->next){
        if (!rhs.contains(f->value)) {
            return false;
        }
    }

    return true;
}


template<class T>
bool LinkedSet<T>::operator != (const LinkedSet<T>& rhs) const {
    return !(*this == rhs);
}


template<class T>
bool LinkedSet<T>::operator <= (const LinkedSet<T>& rhs) const {
    if (this == &rhs)
        return true;

    if (used > rhs.size())
        return false;

    for (LN* f = front; f != trailer; f = f->next) {
        if (!rhs.contains(f->value)) {
            return false;
        }
    }
    return true;
}


template<class T>
bool LinkedSet<T>::operator < (const LinkedSet<T>& rhs) const {
    if (this == &rhs)
        return false;

    if (used >= rhs.size())
        return false;

    for (LN* f = front; f != trailer; f = f->next) {
        if (!rhs.contains(f->value)) {
            return false;
        }
    }
    return true;
}


template<class T>
bool LinkedSet<T>::operator >= (const LinkedSet<T>& rhs) const {
    return rhs <= *this;
}


template<class T>
bool LinkedSet<T>::operator > (const LinkedSet<T>& rhs) const {
    return rhs < *this;
}


template<class T>
std::ostream& operator << (std::ostream& outs, const LinkedSet<T>& s) {
    outs << "set[";


    if (!s.empty()) {
        outs << s.front->value;
        for (auto f = s.front->next; f != s.trailer; f = f->next) {
            outs << "," << f->value;
        }
    }

    outs << "]";
    return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T>
auto LinkedSet<T>::begin () const -> LinkedSet<T>::Iterator {
    return Iterator(const_cast<LinkedSet<T>*>(this),front);
}


template<class T>
auto LinkedSet<T>::end () const -> LinkedSet<T>::Iterator {
    return Iterator(const_cast<LinkedSet<T>*>(this),trailer);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T>
int LinkedSet<T>::erase_at(LN* p) {
    if (p->next == trailer) {
        *p = *trailer;
        delete trailer;
        trailer = p;
        --used;
        ++mod_count;
        return 1;
    }
    LN* to_delete = p->next;
    *p = *to_delete;
    delete to_delete;

    --used;
    ++mod_count;
    return 1;
} //can optimize this and remove first if/add check for trailer before incremening inst vars


template<class T>
void LinkedSet<T>::delete_list(LN*& front) {
    LN* f = front;
    for (LN* f = front; f != trailer; ) {
        front = f->next;
        delete f;
        f = front;
    }
    front = trailer;
}





////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T>
LinkedSet<T>::Iterator::Iterator(LinkedSet<T>* iterate_over, LN* initial)
        : current(initial), ref_set(iterate_over), expected_mod_count(ref_set->mod_count) {
}


template<class T>
LinkedSet<T>::Iterator::~Iterator()
{}


template<class T>
T LinkedSet<T>::Iterator::erase() {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor already erased");
    if (current == ref_set->trailer)
        throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;
    T to_return = current->value;
    ref_set->erase_at(current);
    expected_mod_count = ref_set->mod_count;
    return to_return;
}


template<class T>
std::string LinkedSet<T>::Iterator::str() const {
    std::ostringstream answer;
    answer << ref_set->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
    return answer.str();
}


template<class T>
auto LinkedSet<T>::Iterator::operator ++ () -> LinkedSet<T>::Iterator& {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator ++");

    if (current == ref_set->trailer)
        return *this;

    if (can_erase)
        current = current->next;
    else
        can_erase = true;  //current already indexes "one beyond" erased value

    return *this;
}


template<class T>
auto LinkedSet<T>::Iterator::operator ++ (int) -> LinkedSet<T>::Iterator {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator ++(int)");

    if (current == ref_set->trailer)
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        current = current->next;
    else
        can_erase = true;  //current already indexes "one beyond" erased value

    return to_return;
}


template<class T>
bool LinkedSet<T>::Iterator::operator == (const LinkedSet<T>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("LinkedSet::Iterator::operator ==");
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator ==");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("LinkedSet::Iterator::operator ==");

    return current == rhsASI->current;
}


template<class T>
bool LinkedSet<T>::Iterator::operator != (const LinkedSet<T>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("LinkedSet::Iterator::operator !=");
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator !=");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("LinkedSet::Iterator::operator !=");

    return current != rhsASI->current;
}


template<class T>
T& LinkedSet<T>::Iterator::operator *() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator *");
    if (!can_erase || current == ref_set->trailer) {
        std::ostringstream where;
        where << current << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("LinkedSet::Iterator::operator * Iterator illegal: "+where.str());
    }

    return current->value;
}


template<class T>
T* LinkedSet<T>::Iterator::operator ->() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("LinkedSet::Iterator::operator ->");
    if (!can_erase || current == ref_set->trailer) {
        std::ostringstream where;
        where << current << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("LinkedSet::Iterator::operator -> Iterator illegal: "+where.str());
    }

    return &current->value;
}


}

#endif /* LINKED_SET_HPP_ */
