#pragma once
#include "Common.h"
#include <iterator>

namespace Logic
{
   /// <summary>Forward only iterator for accessing the values in a map collection</summary>
   template <typename OBJ, typename COLL, typename ITER>
   class MapIterator : public std::iterator<std::forward_iterator_tag, OBJ>
   {
      // ------------------------ TYPES --------------------------
   
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create new iterator</summary>
      /// <param name="c">The collection</param>
      /// <param name="pos">The initial position</param>
      MapIterator(const COLL& c, ITER pos) : Collection(&c), Position(pos)
      {
      }

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------
   public:
      OBJ& operator*() const                      { return Position->second; }
      OBJ* operator->() const                     { return &Position->second; }
      bool operator==(const MapIterator& r) const { return Collection==r.Collection && Position==r.Position; }
      bool operator!=(const MapIterator& r) const { return Collection!=r.Collection || Position!=r.Position; }

      // ----------------------- MUTATORS ------------------------
   public:
      MapIterator& operator++() 
      { 
         ++Position;
         return *this;
      }

      MapIterator operator++(int) 
      {
         MapIterator tmp(*this); 
         operator++(); 
         return tmp;
      }
            
      // -------------------- REPRESENTATION ---------------------
   private:
      const COLL* Collection;
      ITER        Position;
   };
}
