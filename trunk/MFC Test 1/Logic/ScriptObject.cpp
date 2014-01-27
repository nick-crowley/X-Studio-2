#include "stdafx.h"
#include "ScriptObject.h"
#include "StringLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a script object</summary>
      /// <param name="id">string ID</param>
      /// <param name="page">page ID</param>
      /// <param name="txt">resolved text</param>
      /// <param name="ver">game version</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised page/id combination</exception>
      ScriptObject::ScriptObject(UINT id, KnownPage page, const wstring& txt, GameVersion ver) 
            : ID(id), Group(IdentifyGroup(page, id)), Text(txt), Version(ver)
      {
      }

      /// <summary>Creates copy of object with alternative text</summary>
      /// <param name="obj">The source object</param>
      /// <param name="newText">The new text.</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised page/id combination</exception>
      ScriptObject::ScriptObject(const ScriptObject& obj, const wstring& newText)
            : ID(obj.ID), Group(obj.Group), Text(newText), Version(obj.Version)
      {
      }


      ScriptObject::~ScriptObject()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Identifies the script object group</summary>
      /// <param name="page">Language page</param>
      /// <param name="id">String ID</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Unrecognised combination</exception>
      ScriptObjectGroup  ScriptObject::IdentifyGroup(KnownPage page, UINT id)
      {
         switch (page)
         {
         case KnownPage::CONSTANTS:          return ScriptObjectGroup::Constant;
         case KnownPage::DATA_TYPES:         return ScriptObjectGroup::DataType;
         case KnownPage::FLIGHT_RETURNS:     return ScriptObjectGroup::FlightReturn;
         case KnownPage::OBJECT_CLASSES:     return ScriptObjectGroup::ObjectClass;
         case KnownPage::OBJECT_COMMANDS:    return ScriptObjectGroup::ObjectCommand;
         case KnownPage::PARAMETER_TYPES:    return ScriptObjectGroup::ParameterType;
         case KnownPage::SECTORS:            return ScriptObjectGroup::Sector;
         case KnownPage::RACES:              return ScriptObjectGroup::Race;
         case KnownPage::RELATIONS:          return ScriptObjectGroup::Relation;
         case KnownPage::STATION_SERIALS:    return ScriptObjectGroup::StationSerial;
         case KnownPage::TRANSPORT_CLASSES:  return ScriptObjectGroup::TransportClass;
         case KnownPage::WING_COMMANDS:      return ScriptObjectGroup::WingCommand;
         }
         throw ArgumentException(HERE, L"p + id", L"Unrecognised page/id combination");
      }

      /// <summary>Get script object group string</summary>
      GuiString  GetString(const ScriptObjectGroup& g)
      {
         return GuiString(IDS_FIRST_SCRIPT_OBJECT_GROUP + (UINT)g);
      }

      /// <summary>Post-Increment script object group</summary>
      ScriptObjectGroup  operator++(ScriptObjectGroup& g, int)
      {
         ScriptObjectGroup ret = g;
         return (++g, ret);
      }

      /// <summary>Pre-Increment script object group</summary>
      ScriptObjectGroup&  operator++(ScriptObjectGroup& g)
      {
         return g = (ScriptObjectGroup)((UINT)g + 1);
      }

      /// <summary>Substract script object group as int</summary>
      UINT  operator-(const ScriptObjectGroup& a, const ScriptObjectGroup& b)
      {
         return (UINT)a - (UINT)b;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Appends an object ID</summary>
      /// <param name="id">The id</param>
      /// <returns>script object with new text</returns>
      ScriptObject  ScriptObject::operator+(UINT id)
      {
         return ScriptObject(*this, Text+GuiString(L" (%d)", id));
      }

      /// <summary>Appends a game version acronym</summary>
      /// <param name="v">The version</param>
      /// <returns>script object with new text</returns>
      ScriptObject  ScriptObject::operator+(GameVersion v)
      {
         return ScriptObject(*this, Text+GuiString(L" (%s)", VersionString(v,true).c_str()));
      }

      /// <summary>Appends a group acronym</summary>
      /// <param name="g">The group</param>
      /// <returns>script object with new text</returns>
      /// <exception cref="Logic::ArgumentException">Incompatible page</exception>
      ScriptObject  ScriptObject::operator+(ScriptObjectGroup g)
      {
         switch (g)
         {
         case ScriptObjectGroup::ObjectCommand:
         case ScriptObjectGroup::WingCommand:     return ScriptObject(*this, Text+L" (CMD)");
         case ScriptObjectGroup::Constant:        return ScriptObject(*this, Text+L" (CON)");
         case ScriptObjectGroup::DataType:        return ScriptObject(*this, Text+L" (DT)");
         case ScriptObjectGroup::FlightReturn:    return ScriptObject(*this, Text+L" (FLRET)");
         case ScriptObjectGroup::ObjectClass:     return ScriptObject(*this, Text+L" (OC)");
         case ScriptObjectGroup::ParameterType:   return ScriptObject(*this, Text+L" (TYPE)");               
         case ScriptObjectGroup::StationSerial:   return ScriptObject(*this, Text+L" (SS)");
         case ScriptObjectGroup::TransportClass:  return ScriptObject(*this, Text+L" (TC)");
         case ScriptObjectGroup::Sector:
         {
            int x = _ttoi(Text.substr(3,2).c_str());
            int y = _ttoi(Text.substr(5,2).c_str());
            return ScriptObject(*this, Text+GuiString(L" (%d,%d)", x, y));
         }
         default: 
            throw ArgumentException(HERE, L"operand", L"Incompatible language page ID");
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
