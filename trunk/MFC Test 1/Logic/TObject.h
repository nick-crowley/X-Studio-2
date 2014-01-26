#pragma once

#include "Common.h"
#include "LookupString.h"

namespace Logic
{
   namespace Types
   {
      /// <summary>Defines type definition categories</summary>
      enum class MainType : UINT
      { 
         Bullet=0, Sector, Background, Sun, Planet, Dock, Factory, Ship, Laser,
         Shield, Missile, EnergyWare, NaturalWare, BioWare, FoodWare, 
         MineralWare, TechWare, Asteroid, Gate, Camera, Special, Nebula,
         InStation, Dummy, Command, Cockpit, Datatype, Unknown, Debris,
         DockWreck, FactoryWreck, ShipWreck
      };

      MainType& operator++(MainType& m);
      MainType operator++(MainType& m, int);
      UINT operator-(const MainType& a, const MainType& b);
      
      /// <summary>Get main type name</summary>
      const wchar* GetString(const MainType& m);

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
         TObject(MainType t);
         virtual ~TObject();

         DEFAULT_COPY(TObject);	// Default copy semantics
         DEFAULT_MOVE(TObject);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(wstring,FullName,GetFullName);

         // ---------------------- ACCESSORS ------------------------			
      public:
         // MOVE TO TSHIP
         /*bool IsPirate() const
         {
            return subtype.Right(2) == L"_P" || subtype.Right(4) == L"_P_1" || subtype.Right(4) == L"_P_2" || subtype.Right(4) == L"_P_3";
         }*/

         virtual wstring GetFullName() const;

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         MainType        Type;

         // Header
         wstring         id;
         Vector3<float>  rotation;
	      wstring         bodyFile;
	      int             pictureID;
	      GuiString       subtype;
         LookupString    name; 
         LookupString    description;
   	
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
