#include "stdafx.h"
#include "ScriptObject.h"
#include "StringLibrary.h"
#include "ScriptParameter.h"

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
      ScriptObject::ScriptObject(UINT id, KnownPage page, const GuiString& txt, GameVersion ver) 
            : ID(id), Group(IdentifyGroup(page, id)), Text(txt), Version(ver)
      {
      }

      /// <summary>Copy move ctor</summary>
      /// <param name="r">The source</param>
      ScriptObject::ScriptObject(ScriptObject&& r)
         : ID(r.ID), Group(r.Group), Text(move(r.Text)), Version(r.Version)
      {
      }

      /// <summary>Creates copy of object with alternative text</summary>
      /// <param name="obj">The source object</param>
      /// <param name="newText">The new text.</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised page/id combination</exception>
      ScriptObject::ScriptObject(const ScriptObject& obj, const GuiString& newText)
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
         case KnownPage::OPERATORS:          return ScriptObjectGroup::Operator;
         case KnownPage::PARAMETER_TYPES:    return ScriptObjectGroup::ParameterType;
         case KnownPage::SECTORS:            return ScriptObjectGroup::Sector;
         case KnownPage::RACES:              return ScriptObjectGroup::Race;
         case KnownPage::RELATIONS:          return ScriptObjectGroup::Relation;
         case KnownPage::STATION_SERIALS:    return ScriptObjectGroup::StationSerial;
         case KnownPage::TRANSPORT_CLASSES:  return ScriptObjectGroup::TransportClass;
         case KnownPage::WING_COMMANDS:      return ScriptObjectGroup::WingCommand;
         }
         throw ArgumentException(HERE, L"page/id", GuiString(L"{%d:%d} is not a valid page/id combination", page, id));
      }

      /// <summary>Converts datatype to a script object group</summary>
      /// <param name="type">Datatype</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Unsupported datatype</exception>
      ScriptObjectGroup  ScriptObject::IdentifyGroup(DataType type)
      {
         switch (type)
         {
         case DataType::CONSTANT:         return ScriptObjectGroup::Constant;
         case DataType::DATATYPE:         return ScriptObjectGroup::DataType;
         case DataType::FLIGHTRETURN:     return ScriptObjectGroup::FlightReturn;
         case DataType::OBJECTCLASS:      return ScriptObjectGroup::ObjectClass;
         case DataType::OBJECTCOMMAND:    return ScriptObjectGroup::ObjectCommand;
         case DataType::OPERATOR:         return ScriptObjectGroup::Operator;
         case DataType::SCRIPTDEF:        return ScriptObjectGroup::ParameterType;
         case DataType::SECTOR:           return ScriptObjectGroup::Sector;
         case DataType::RACE:             return ScriptObjectGroup::Race;
         case DataType::RELATION:         return ScriptObjectGroup::Relation;
         case DataType::STATIONSERIAL:    return ScriptObjectGroup::StationSerial;
         case DataType::TRANSPORTCLASS:   return ScriptObjectGroup::TransportClass;
         case DataType::WINGCOMMAND:      return ScriptObjectGroup::WingCommand;
         }
         throw ArgumentException(HERE, L"type", GuiString(L"Cannot make script object from datatype '%s' (%d)", GetString(type).c_str(), type));
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

      /// <summary>Get data-type from group</summary>
      DataType  ScriptObject::GetDataType() const
      {
         switch (Group)
         {
         case ScriptObjectGroup::Constant:         return DataType::CONSTANT;
         case ScriptObjectGroup::DataType:         return DataType::DATATYPE;
         case ScriptObjectGroup::FlightReturn:     return DataType::FLIGHTRETURN;
         case ScriptObjectGroup::ObjectClass:      return DataType::OBJECTCLASS;
         case ScriptObjectGroup::ObjectCommand:    return DataType::OBJECTCOMMAND;
         case ScriptObjectGroup::Operator:         return DataType::OPERATOR;
         case ScriptObjectGroup::ParameterType:    return DataType::SCRIPTDEF;
         case ScriptObjectGroup::Sector:           return DataType::SECTOR;
         case ScriptObjectGroup::Race:             return DataType::RACE;
         case ScriptObjectGroup::Relation:         return DataType::RELATION;
         case ScriptObjectGroup::StationSerial:    return DataType::STATIONSERIAL;
         case ScriptObjectGroup::TransportClass:   return DataType::TRANSPORTCLASS;
         case ScriptObjectGroup::WingCommand:      return DataType::WINGCOMMAND;
         }
         throw ArgumentException(HERE, L"Group", GuiString(L"Unrecognised script object group %s", GetString(Group).c_str()));
      }


      /// <summary>Get text formatted for display</summary>
      /// <returns>script object formatted for display</returns>
      GuiString  ScriptObject::GetDisplayText() const
      {
         // Operators are translated by parameter because of spacing issues
         if (Group == ScriptObjectGroup::Operator)
            throw InvalidOperationException(HERE, L"Cannot get the display Text for operators");
         
         return GuiString(L"[%s]", Text.c_str());
      }

      /// <summary>Get whether object should be hidden from GUI</summary>
      bool  ScriptObject::IsHidden() const
      {
         switch (Group)
         {
         case ScriptObjectGroup::ParameterType:
            // Hide internal parameter types
            switch ((ParameterType)ID)
            {
            case ParameterType::LABEL_NAME:
            case ParameterType::LABEL_NUMBER:
            case ParameterType::PARAMETER:
            case ParameterType::CONDITION:
            case ParameterType::EXPRESSION:
            case ParameterType::COMMENT:
            case ParameterType::VARIABLE:    // Var
            case ParameterType::RETURN_VALUE:
            case ParameterType::RETURN_VALUE_IF:
            case ParameterType::RETURN_VALUE_IF_START:
            case ParameterType::INTERRUPT_RETURN_VALUE_IF:
            case ParameterType::REFERENCE_OBJECT:
               return true;
            }
         }

         return false;
      }

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
      /// <exception cref="Logic::InvalidValueException">Invalid sector string ID</exception>
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
            auto pt = SectorIDConverter::ToCoordinates(ID);
            return ScriptObject(*this, Text+GuiString(L" (%d,%d)", pt.first, pt.second));
         }
         default: 
            throw ArgumentException(HERE, L"operand", L"Incompatible language page ID");
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
