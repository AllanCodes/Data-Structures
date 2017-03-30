// Submitter: aeivazia(Eivazian, Allan)
// Partner  : tommyll(Le, Tommy)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming
#ifndef HASH_MAP_HPP_
#define HASH_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"


namespace ics {


#ifndef undefinedhashdefined
#define undefinedhashdefined

    template<class T>
    int undefinedhash(const T &a) { return 0; }

#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
    template<class KEY, class T, int (*thash)(const KEY &a) = undefinedhash<KEY>>
    class HashMap {
    public:
        typedef ics::pair<KEY, T> Entry;

        typedef int (*hashfunc)(const KEY &a);

        //Destructor/Constructors
        ~HashMap();

        HashMap(double the_load_threshold = 1.0, int (*chash)(const KEY &a) = undefinedhash<KEY>);

        explicit HashMap(int initial_bins, double the_load_threshold = 1.0,
                         int (*chash)(const KEY &k) = undefinedhash<KEY>);

        HashMap(const HashMap<KEY, T, thash> &to_copy, double the_load_threshold = 1.0,
                int (*chash)(const KEY &a) = undefinedhash<KEY>);

        explicit HashMap(const std::initializer_list<Entry> &il, double the_load_threshold = 1.0,
                         int (*chash)(const KEY &a) = undefinedhash<KEY>);

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template<class Iterable>
        explicit
        HashMap(const Iterable &i, double the_load_threshold = 1.0, int (*chash)(const KEY &a) = undefinedhash<KEY>);


        //Queries
        bool empty() const;

        int size() const;

        bool has_key(const KEY &key) const;

        bool has_value(const T &value) const;

        std::string str() const; //supplies useful debugging information; contrast to operator <<


        //Commands
        T put(const KEY &key, const T &value);

        T erase(const KEY &key);

        void clear();

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template<class Iterable>
        int put_all(const Iterable &i);


        //Operators

        T &operator[](const KEY &);

        const T &operator[](const KEY &) const;

        HashMap<KEY, T, thash> &operator=(const HashMap<KEY, T, thash> &rhs);

        bool operator==(const HashMap<KEY, T, thash> &rhs) const;

        bool operator!=(const HashMap<KEY, T, thash> &rhs) const;

        template<class KEY2, class T2, int (*hash2)(const KEY2 &a)>
        friend std::ostream &operator<<(std::ostream &outs, const HashMap<KEY2, T2, hash2> &m);


    private:
        class LN;

    public:
        class Iterator {
        public:
            typedef pair<int, LN *> Cursor;

            //Private constructor called in begin/end, which are friends of HashMap<T>
            ~Iterator();

            Entry erase();

            std::string str() const;

            HashMap<KEY, T, thash>::Iterator &operator++();

            HashMap<KEY, T, thash>::Iterator operator++(int);

            bool operator==(const HashMap<KEY, T, thash>::Iterator &rhs) const;

            bool operator!=(const HashMap<KEY, T, thash>::Iterator &rhs) const;

            Entry &operator*() const;

            Entry *operator->() const;

            friend std::ostream &operator<<(std::ostream &outs, const HashMap<KEY, T, thash>::Iterator &i) {
             outs << i.str(); //Use the same meaning as the debugging .str() method
             return outs;
            }

            friend Iterator HashMap<KEY, T, thash>::begin() const;

            friend Iterator HashMap<KEY, T, thash>::end() const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            Cursor current; //Bin Index + LN* pointer; stops if LN* == nullptr
            HashMap<KEY, T, thash> *ref_map;
            int expected_mod_count;
            bool can_erase = true;

            //Helper methods
            void advance_cursors();

            //Called in friends begin/end
            Iterator(HashMap<KEY, T, thash> *iterate_over, bool from_begin);
        };


        Iterator begin() const;

        Iterator end() const;


    private:
        class LN {
        public:
            LN() : next(nullptr) {}

            LN(const LN &ln) : value(ln.value), next(ln.next) {}

            LN(Entry v, LN *n = nullptr) : value(v), next(n) {}

            Entry value;
            LN *next;
        };

        int (*hash)(const KEY &k);  //Hashing function used (from template or constructor)
        LN **map = nullptr;    //Pointer to array of pointers: each bin stores a list with a trailer node
        double load_threshold;      //used/bins <= load_threshold
        int bins = 1;          //# bins in array (should start >= 1 so hash_compress doesn't divide by 0)
        int used = 0;          //Cache for number of key->value pairs in the hash table
        int mod_count = 0;          //For sensing concurrent modification


        //Helper methods
        int hash_compress(const KEY &key) const;  //hash function ranged to [0,bins-1]
        LN *find_key(const KEY &key) const;           //Returns reference to key's node or nullptr
        LN *copy_list(LN *l) const;  //Copy the keys/values in a bin (order irrelevant)
        LN **
        copy_hash_table(LN **ht, int bins) const;  //Copy the bins/keys/values in ht tree (order in bins irrelevant)

        void ensure_load_threshold(int new_used);                   //Reallocate if load_factor > load_threshold
        void
        delete_hash_table(LN **&ht, int bins);             //Deallocate all LN in ht (and the ht itself; ht == nullptr)
    };





////////////////////////////////////////////////////////////////////////////////
//
//HashMap class and related definitions

//Destructor/Constructors

    template<class KEY, class T, int (*thash)(const KEY &a)>
    HashMap<KEY, T, thash>::~HashMap() {
     delete_hash_table(map, bins);
    }


    template<class KEY, class T, int (*thash)(const KEY &a)>
    HashMap<KEY, T, thash>::HashMap(double the_load_threshold, int (*chash)(const KEY &k))
     : hash(thash != undefinedhash<KEY> ? thash : chash) {
     if (hash == hashfunc(undefinedhash<KEY>))
      throw TemplateFunctionError("HashMAP::default constructor: neither specified");

     if (thash != hashfunc(undefinedhash<KEY>) && chash != hashfunc(undefinedhash<KEY>) && thash != chash)
      throw TemplateFunctionError("HashMAP::default constructor: both specified and different");

     map = new LN *[bins];
     load_threshold = the_load_threshold;

     for (int i = 0; i < bins; i++) {
      map[i] = new LN();
     }

    }


    template<class KEY, class T, int (*thash)(const KEY &a)>
    HashMap<KEY, T, thash>::HashMap(int initial_bins, double the_load_threshold, int (*chash)(const KEY &k))
            : hash(thash != undefinedhash<KEY> ? thash : chash) {
     if (hash == hashfunc(undefinedhash<KEY>))
      throw TemplateFunctionError("HashMAP::length constructor: neither specified");

     if (thash != hashfunc(undefinedhash<KEY>) && chash != hashfunc(undefinedhash<KEY>) && thash != chash)
      throw TemplateFunctionError("HashMAP::length constructor: both specified and different");

     bins = (initial_bins <= 0 ? 1 : initial_bins);
     load_threshold = the_load_threshold;
     map = new LN *[bins];

     for (int i = 0; i < bins; i++)
      map[i] = new LN();
    }

template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::HashMap(const HashMap<KEY,T,thash>& to_copy, double the_load_threshold, int (*chash)(const KEY& a))
:hash(thash != undefinedhash<KEY> ? thash : chash) {
 if (thash != hashfunc(undefinedhash<KEY>) && chash != hashfunc(undefinedhash<KEY>) && thash != chash)
  throw TemplateFunctionError("HashMAP::copy constructor: both specified and different");

 hash = (hash == hashfunc(undefinedhash<KEY>) ? to_copy.hash : hash);
 load_threshold = the_load_threshold;
 bins = to_copy.bins;

 if (hash == to_copy.hash) {
  used = to_copy.used;
  this->map = copy_hash_table(to_copy.map,bins);
  return;
 }

  map = new LN*[bins];
  for (int i = 0; i < bins; i++ )
   map[i] = new LN();
  //hash
  for (int i = 0; i < bins; i++ )
   for (LN* l = to_copy.map[i]; l->next != nullptr; l=l->next)
    put(l->value.first,l->value.second);

//update bins?
 }

template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::HashMap(const std::initializer_list<Entry>& il, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != undefinedhash<KEY> ? thash : chash) {
 if (hash == hashfunc(undefinedhash<KEY>))
  throw TemplateFunctionError("HashMAP::initializer_list constructor: neither specified");

 if (thash != hashfunc(undefinedhash<KEY>) && chash != hashfunc(undefinedhash<KEY>) && thash != chash)
  throw TemplateFunctionError("HashMAP::initializer_list constructor: both specified and different");

 load_threshold = the_load_threshold;
 map = new LN*[bins];
 for (int i = 0; i < bins; i++)
  map[i] = new LN();

 for (auto &kv : il)
  put(kv.first,kv.second);

}


template<class KEY,class T, int (*thash)(const KEY& a)>
template <class Iterable>
HashMap<KEY,T,thash>::HashMap(const Iterable& i, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != undefinedhash<KEY> ? thash : chash){
 if (hash == hashfunc(undefinedhash<KEY>))
  throw TemplateFunctionError("HashMAP::Iterable constructor: neither specified");

 if (thash != hashfunc(undefinedhash<KEY>) && chash != hashfunc(undefinedhash<KEY>) && thash != chash)
  throw TemplateFunctionError("HashMAP::Iterable constructor: both specified and different");

load_threshold = the_load_threshold;
map = new LN*[bins];
for (int i = 0; i < bins; i++ )
 map[i] = new LN();

 for (auto &kv : i)
  put(kv.first, kv.second);
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::empty() const {
 return used == 0;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashMap<KEY,T,thash>::size() const {
 return used;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::has_key (const KEY& key) const {
 return find_key(key) != nullptr;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::has_value (const T& value) const {
 for (int i = 0; i < bins; i++ )
  for (LN* l = map[i]; l->next != nullptr; l = l->next)
   if (l->value.second == value)
    return true;

 return false;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashMap<KEY,T,thash>::str() const {
 std::ostringstream answer;
 answer << "HashMAP[";

 if (bins != 0){
  for (int i = 0; i < bins; i++ ){
   answer << "bin[" << i << "]: ";
   for (LN* l = map[i]; l->next != nullptr; l=l->next)
    answer << l->value.first << "->" << l->value.second << " -> ";

    answer << "TRAILER" << std::endl;
  }
 }
 answer << "(load" << load_threshold << "bins=" << bins << ",used=" << used << ",mod_count=" << mod_count << ")";
 return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class KEY,class T, int (*thash)(const KEY& a)>
T HashMap<KEY,T,thash>::put(const KEY& key, const T& value) {
 int i = hash_compress(key);
 LN* k = find_key(key);
 T ret = value;
 if (k != nullptr) {
  T to_return = k->value.second;
  k->value.second = value;
  ++mod_count;
  return to_return;
 }

  ensure_load_threshold(used + 1);
  i = hash_compress(key);
  map[i] = new LN(Entry(key,value),map[i]); //points to trailer (if first)
 ++used;
 ++mod_count;
 return ret;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
T HashMap<KEY,T,thash>::erase(const KEY& key) {
 auto i = hash_compress(key);
 LN* t = find_key(key);

 if (t == nullptr) {
  std::ostringstream answer;
  answer << "HashMAP::erase: key(" << key << ") not in Map";
  throw KeyError(answer.str());
 }

 T to_return = t->value.second;
 LN* to_delete = t->next;
 *t = *to_delete;
 delete to_delete;
 --used;
 ++mod_count;
 return to_return;
}

//here
template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::clear() {
 for (int i = 0; i < bins; i++ ) {
  for (LN *l = map[i]; l->next != nullptr;) {
   LN *to_delete = l->next;
   *l = *to_delete;
   delete to_delete;
  }
 }
     ++mod_count;
     used = 0;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template<class Iterable>
int HashMap<KEY,T,thash>::put_all(const Iterable& i) {
 int count = 0;
 for (auto &kv : i) {
  put(kv.first,kv.second);
  ++count;
 }

 return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class KEY,class T, int (*thash)(const KEY& a)>
T& HashMap<KEY,T,thash>::operator [] (const KEY& key) {
 if (this->has_key(key)) {
  return find_key(key)->value.second;
 }

 put(key,T());
 return find_key(key)->value.second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
const T& HashMap<KEY,T,thash>::operator [] (const KEY& key) const {
 if (this->has_key(key)) {
  return find_key(key)->value.second;
 }

 std::ostringstream answer;
 answer << "HashMap::operator []: key(" << key << ") not in Map";
 throw KeyError(answer.str());
}

//this
template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>& HashMap<KEY,T,thash>::operator = (const HashMap<KEY,T,thash>& rhs) {
 if (this == &rhs)
  return *this;

 delete_hash_table(map,bins);
 load_threshold = rhs.load_threshold;

 bins = rhs.bins;
 used = rhs.used;
 hash = rhs.hash;
 map = copy_hash_table(rhs.map, rhs.bins);

 ++mod_count;
 return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::operator == (const HashMap<KEY,T,thash>& rhs) const {
 if (this == &rhs)
  return true;

 if (used != rhs.size())
  return false;

 for (int i = 0; i < bins; i++ ) {
  for (LN* l = map[i]; l->next != nullptr; l = l->next) {
   if (l->value.second != rhs[l->value.first]) {
    return false;
   }
  }
 }
 return true;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::operator != (const HashMap<KEY,T,thash>& rhs) const {
 return !(*this == rhs);
}

//here here here
template<class KEY,class T, int (*thash)(const KEY& a)>
std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T,thash>& m) {
 outs << "map[";

     for (int i = 0; i < m.bins; i++) {
      for(auto l = m.map[i]; l->next != nullptr; l = l->next) {
       outs << l->value.first << "->" << l->value.second;
       if (l->next->next == nullptr) {
        outs << "]";
        return outs;
       }
       else {
        outs << ",";
       }
      }
     }
     outs << "]";
return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashMap<KEY,T,thash>::begin () const -> HashMap<KEY,T,thash>::Iterator {
 return Iterator(const_cast<HashMap<KEY,T,thash>*>(this),true);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashMap<KEY,T,thash>::end () const -> HashMap<KEY,T,thash>::Iterator {
 return Iterator(const_cast<HashMap<KEY,T,thash>*>(this),false);
}


///////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class KEY,class T, int (*thash)(const KEY& a)>
int HashMap<KEY,T,thash>::hash_compress (const KEY& key) const {
 return std::abs(hash(key)) % bins;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
typename HashMap<KEY,T,thash>::LN* HashMap<KEY,T,thash>::find_key (const KEY& key) const {
 for (LN* l = map[hash_compress(key)]; l->next != nullptr; l = l->next) {
  if (l->value.first == key) {
   return l;
  }
 }
 return nullptr;
}

//here test if this works or I have to do it like notes
template<class KEY,class T, int (*thash)(const KEY& a)>
typename HashMap<KEY,T,thash>::LN* HashMap<KEY,T,thash>::copy_list (LN* l) const {
     if (l == nullptr)
      return nullptr;
     else
  return new LN(l->value, copy_list(l->next));

}


template<class KEY,class T, int (*thash)(const KEY& a)>
typename HashMap<KEY,T,thash>::LN** HashMap<KEY,T,thash>::copy_hash_table (LN** ht, int bins) const {
 LN** new_map = new LN*[bins];
 for (int i = 0; i < bins; i++ ){
  new_map[i] = copy_list(ht[i]);
 }

 return new_map;
}

//here
template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::ensure_load_threshold(int new_used) {
 double new_threshold = new_used/bins;
if (new_threshold <= load_threshold)
 return;

 int saveBins = bins;
 bins = 2 * saveBins;
 LN** old = copy_hash_table(map,saveBins);

 //start reconstructing
 map = new LN*[bins];

 for (int i = 0; i < bins; i++ ){ //constructor
map[i] = new LN(); //trail
 }
//used = new_used;

 for (int i = 0; i < saveBins; i++ ){
  for (LN* l = old[i]; l->next != nullptr; l = l->next) {
   int i = hash_compress(l->value.first);
   map[i] = new LN(Entry(l->value.first,l->value.second),map[i]);
  }
 }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::delete_hash_table (LN**& ht, int bins) {
     for (int i = 0; i < bins; i++ ){
      for (LN* l = ht[i]; l->next != nullptr; ) {
       LN* to_delete = l;
       l = l->next;
       delete to_delete;
      }
     }

 delete [] ht;
 ht = nullptr;
}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template<class KEY,class T, int (*thash)(const KEY& a)>
    void HashMap<KEY,T,thash>::Iterator::advance_cursors() {

     LN* node = current.second; //lets keep track of this

     if (current.second != nullptr) { //we can advance
      current.second  = current.second->next;
     }
     if (current.second == nullptr || current.second->next == nullptr) {
      int nex = current.first + 1;

      while (nex < ref_map->bins) {
       if (ref_map->map[nex]->next != nullptr) {
        current.first = nex;
        current.second = ref_map->map[nex];
        return;
       }
       nex += 1;
      }
      current.first = -1;
      current.second = nullptr;
      //refer to p4 for refresh on justification
     }
    }

template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::Iterator::Iterator(HashMap<KEY,T,thash>* iterate_over, bool from_begin)
: ref_map(iterate_over), expected_mod_count(ref_map->mod_count) {
 current = Cursor(-1,nullptr); //justification in p4
 expected_mod_count = ref_map->mod_count;
 if (from_begin)
  advance_cursors();
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::Iterator::~Iterator()
{}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashMap<KEY,T,thash>::Iterator::erase() -> Entry {
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::erase");
 if (!can_erase)
  throw CannotEraseError("HashMap::Iterator::erase Iterator cursor already erased");
 if (current.second == nullptr) //past trail
  throw CannotEraseError("HashMap::Iterator::erase Iterator cursor beyond data structure");

 can_erase = false;
 Entry to_return = current.second->value;
 expected_mod_count = ref_map->mod_count;
 ref_map->erase(current.second->value.first);
 expected_mod_count++;
 return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashMap<KEY,T,thash>::Iterator::str() const {
 std::ostringstream answer;
 answer << ref_map->str() << "(current<Entry>" << current.first << "," << current.second <<  ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
 return answer.str();
}

template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashMap<KEY,T,thash>::Iterator::operator ++ () -> HashMap<KEY,T,thash>::Iterator& {
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::operator ++");

 if (current.second == nullptr)
  return *this;

 if (can_erase || current.second->next == nullptr) //this is for end
  advance_cursors();

 can_erase = true;
 return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashMap<KEY,T,thash>::Iterator::operator ++ (int) -> HashMap<KEY,T,thash>::Iterator {
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::operator ++(int)");

 if (current.second == nullptr)
  return *this;

 Iterator to_return(*this);
 if (can_erase && current.second != nullptr)
  advance_cursors();

can_erase = true;
 return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::Iterator::operator == (const HashMap<KEY,T,thash>::Iterator& rhs) const {
 const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
 if (rhsASI == 0)
  throw IteratorTypeError("HashMap::Iterator::operator ==");
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::operator ==");
 if (ref_map != rhsASI->ref_map)
  throw ComparingDifferentIteratorsError("HashMap::Iterator::operator ==");

 return current.first == rhsASI->current.first && current.second == rhsASI->current.second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::Iterator::operator != (const HashMap<KEY,T,thash>::Iterator& rhs) const {
 const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
 if (rhsASI == 0)
  throw IteratorTypeError("HashMap::Iterator::operator !=");
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::operator !=");
 if (ref_map != rhsASI->ref_map)
  throw ComparingDifferentIteratorsError("HashMap::Iterator::operator !=");

 return current.first != rhsASI->current.first && current.second != rhsASI->current.second;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>& HashMap<KEY,T,thash>::Iterator::operator *() const {
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::operator *");
 if (!can_erase || current.second == nullptr) {
  std::ostringstream where;
  where << current << " when size = " << ref_map->size();
  throw IteratorPositionIllegal("HashMap::Iterator::operator * Iterator illegal: "+where.str());
 }

 return current.second->value;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>* HashMap<KEY,T,thash>::Iterator::operator ->() const {
 if (expected_mod_count != ref_map->mod_count)
  throw ConcurrentModificationError("HashMap::Iterator::operator *");
 if (!can_erase || current.second == nullptr) {
  std::ostringstream where;
  where << current << " when size = " << ref_map->size();
  throw IteratorPositionIllegal("HashMap::Iterator::operator * Iterator illegal: "+where.str());
 }

 return &current.second->value;
}


}

#endif /* HASH_MAP_HPP_ */
