#pragma once

#include "Common.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Defines script object group</summary>
      enum class ScriptObjectGroup
      {
         Constant, DataType, FlightReturn, ObjectClass, Race, Relation, ParameterType, 
         Sector, StationSerial, ObjectCommand, WingCommand, Signal, TransportClass, Operator
      };

      /// <summary>Convenience script object group global functions</summary>
      ScriptObjectGroup operator++(ScriptObjectGroup& g, int);
      ScriptObjectGroup& operator++(ScriptObjectGroup& g);
      UINT operator-(const ScriptObjectGroup& a, const ScriptObjectGroup& b);
      
      /// <summary>Get script object group name</summary>
      GuiString GetString(const ScriptObjectGroup& g);

      /// <summary>Represents a script object</summary>
      class ScriptObject : public LanguageString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptObject(UINT id, KnownPage page, const wstring& txt, GameVersion ver);
         ~ScriptObject();

      private:
         ScriptObject(const ScriptObject& obj, const wstring& newText);
         

      public:
         DEFAULT_COPY(ScriptObject);	// Default copy semantics
		   DEFAULT_MOVE(ScriptObject);	// Default move semantics

         // ------------------------ STATIC -------------------------

         /// <summary>Identifies the script object group</summary>
         static ScriptObjectGroup  IdentifyGroup(KnownPage page, UINT id);

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Appends an object ID</summary>
         /// <param name="id">The id</param>
         /// <returns>New script object</returns>
         ScriptObject operator+(UINT id)
         {
            return ScriptObject(*this, Text+GuiString(L" (%d)", id));
         }

         /// <summary>Appends a game version acronym</summary>
         /// <param name="v">The version</param>
         /// <returns>New script object</returns>
         ScriptObject operator+(GameVersion v)
         {
            return ScriptObject(*this, Text+GuiString(L" (%s)", VersionString(v,true).c_str()));
         }

         /// <summary>Appends an page/category acronym</summary>
         /// <param name="p">The page</param>
         /// <returns>New script object</returns>
         /// <exception cref="Logic::ArgumentException">Incompatible page</exception>
         ScriptObject operator+(KnownPage p)
         {
            const wchar* acronym = nullptr;
            switch (p)
            {
            case KnownPage::OBJECT_COMMANDS:
            case KnownPage::WING_COMMANDS:  
               return ScriptObject(*this, Text+L" (CMD)");
            case KnownPage::CONSTANTS:
               return ScriptObject(*this, Text+L" (CON)");
            case KnownPage::DATA_TYPES:
               return ScriptObject(*this, Text+L" (DT)");
            case KnownPage::FLIGHT_RETURNS:
               return ScriptObject(*this, Text+L" (FLRET)");
            case KnownPage::OBJECT_CLASSES:
               return ScriptObject(*this, Text+L" (OC)");
            case KnownPage::PARAMETER_TYPES:
               return ScriptObject(*this, Text+L" (TYPE)");               
            case KnownPage::STATION_SERIALS:
               return ScriptObject(*this, Text+L" (SS)");
            case KnownPage::TRANSPORT_CLASSES:
               return ScriptObject(*this, Text+L" (TC)");
            case KnownPage::SECTORS:
            {
               int x = _ttoi(Text.substr(3,2).c_str());
               int y = _ttoi(Text.substr(5,2).c_str());
               return ScriptObject(*this, Text+GuiString(L" (%d,%d)", x, y));
            }
            default: throw ArgumentException(HERE, L"operand", L"Incompatible language page ID");
            }
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         const KnownPage Page;
         const ScriptObjectGroup Group;
      };


      /// <summary>Vector of script objects</summary>
      typedef vector<ScriptObject>  ScriptObjectArray;
   
   }
}

using namespace Logic::Scripts;
