#include "stdafx.h"
#include "ScriptObject.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a script object from a language string</summary>
      /// <param name="p">The source page</param>
      /// <param name="s">The string</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised page/id combination</exception>
      ScriptObject::ScriptObject(KnownPage p, const LanguageString& s) : Page(p), Group(IdentifyGroup(p, s.ID)), LanguageString(s)
      {
      }

      /// <summary>Creates copy of object with alternative text</summary>
      /// <param name="obj">The source object</param>
      /// <param name="newText">The new text.</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised page/id combination</exception>
      ScriptObject::ScriptObject(const ScriptObject& obj, const wstring& newText)
            : Page(obj.Page), Group(IdentifyGroup(obj.Page, obj.ID)), LanguageString(obj.ID, newText, obj.Version)
      {
      }


      ScriptObject::~ScriptObject()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Identifies the script object group</summary>
      /// <param name="p">Language page</param>
      /// <param name="id">String ID</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Unrecognised combination</exception>
      ScriptObjectGroup  ScriptObject::IdentifyGroup(KnownPage p, UINT id)
      {
         switch (p)
         {
         case KnownPage::CONSTANTS:          return ScriptObjectGroup::Constant;
         case KnownPage::DATA_TYPES:         return ScriptObjectGroup::DataType;
         case KnownPage::FLIGHT_RETURNS:     return ScriptObjectGroup::FlightReturn;
         case KnownPage::OBJECT_CLASSES:     return ScriptObjectGroup::ObjectClass;
         case KnownPage::OBJECT_COMMANDS:    return ScriptObjectGroup::ObjectCommand;
         case KnownPage::PARAMETER_TYPES:    return ScriptObjectGroup::ParameterType;
         case KnownPage::SECTORS:            return ScriptObjectGroup::Sector;
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
