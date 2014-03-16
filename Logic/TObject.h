#pragma once


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

      /// <summary>Convenience main type global functions</summary>
      MainType& operator++(MainType& m);
      MainType operator++(MainType& m, int);
      UINT operator-(const MainType& a, const MainType& b);
      
      /// <summary>Get main type string</summary>
      GuiString  GetString(const MainType& m);

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
         wstring GetFullName() const;
      
      protected:
         virtual wstring GetInternalName() const;

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      public:
         MainType  Type;

         // Header
         GuiString       id;
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
