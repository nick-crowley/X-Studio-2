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
         : LanguageString(id, txt, ver), Group(IdentifyGroup(page, id)), Page(page)
      {
      }

      /// <summary>Creates copy of object with alternative text</summary>
      /// <param name="obj">The source object</param>
      /// <param name="newText">The new text.</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised page/id combination</exception>
      ScriptObject::ScriptObject(const ScriptObject& obj, const wstring& newText)
            : LanguageString(obj.ID, newText, obj.Version),
              Group(IdentifyGroup(obj.Page, obj.ID)),
              Page(obj.Page)
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
      GuiString GetString(const ScriptObjectGroup& g)
      {
         return GuiString(IDS_FIRST_SCRIPT_OBJECT_GROUP + (UINT)g);
      }

      /// <summary>Post-Increment script object group</summary>
      ScriptObjectGroup operator++(ScriptObjectGroup& g, int)
      {
         ScriptObjectGroup ret = g;
         return (++g, ret);
      }

      /// <summary>Pre-Increment script object group</summary>
      ScriptObjectGroup& operator++(ScriptObjectGroup& g)
      {
         return g = (ScriptObjectGroup)((UINT)g + 1);
      }

      /// <summary>Operator on script object group as int</summary>
      UINT operator-(const ScriptObjectGroup& a, const ScriptObjectGroup& b)
      {
         return (UINT)a - (UINT)b;
      }
      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
