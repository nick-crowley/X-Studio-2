#pragma once

#include "Common.h"

namespace Logic
{
   namespace Types
   {
      /// <summary>Ware sizes</summary>
      enum class WareSize : UINT { Tiny, Small, Medium, Large, ExtraLarge, Station };

      /// <summary>Vector</summary>
      /// <typeparam name="T"></typeparam>
      template<typename T>
      struct Vector3
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         Vector3(T x, T y, T z) : x(x), y(y), z(z)
         {}
         
         // -------------------- REPRESENTATION ---------------------
      public:
         T  x, y, z;
      };

      /// <summary></summary>
      class TObject
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TObject();
         virtual ~TObject();

         DEFAULT_COPY(TObject);	// Default copy semantics
         DEFAULT_MOVE(TObject);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         // Header
         wstring         id;
         Vector3<float>  rotation;
	      wstring         bodyFile;
	      int             pictureID;
	      wstring         subtype;
         //XStringResource name;  
         UINT            name;
   	
         // Footer
	      int             volume;
	      int             relativeValue;
	      int             priceModifier1;
	      int             priceModifier2;
	      WareSize        size;
	      int             relativeValuePlayer;
	      int             minimumNotoriety;
	      int             videoID;
	      int             skinIndex;
      };
   
   }
}

using namespace Logic::Types;
