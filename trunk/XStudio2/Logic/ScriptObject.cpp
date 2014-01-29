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
      ScriptObject::ScriptObject(UINT id, KnownPage page, const wstring& txt, GameVersion ver) 
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
         throw ArgumentException(HERE, L"type", GuiString(L"%s (%d) is not a valid script object datatype", GetString(type).c_str(), type));
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

      /// <summary>Get text formatted for display</summary>
      /// <returns>script object formatted for display</returns>
      GuiString  ScriptObject::GetDisplayText() const
      {
         switch (Group)
         {
         case ScriptObjectGroup::Operator:
            switch (Operator op = (Operator)ID)
            {
            // HACK: Substitute for octal entity
            case Operator::Add:
               return L"+";

            // Close bracket
            case Operator::CloseBracket:  
               return GuiString(L"%s ", Text.c_str());  

            // Unary
            case Operator::Minus:
            case Operator::LogicalNot:
            case Operator::BitwiseNot:
            case Operator::OpenBracket: 
               return GuiString(L" %s", Text.c_str());  

            // Binary 
            default:  
               return GuiString(L" %s ", Text.c_str());  
            }
            break;

         case ScriptObjectGroup::Constant:       
         case ScriptObjectGroup::DataType:       
         case ScriptObjectGroup::ParameterType:  
         case ScriptObjectGroup::StationSerial:  
         case ScriptObjectGroup::TransportClass: 
         case ScriptObjectGroup::FlightReturn:   
         case ScriptObjectGroup::ObjectClass:    
         case ScriptObjectGroup::ObjectCommand:  
         case ScriptObjectGroup::WingCommand:    
         case ScriptObjectGroup::Race:           
         case ScriptObjectGroup::Sector:         
         case ScriptObjectGroup::Relation:
            return GuiString(L"[%s]", Text.c_str());
         }

         throw ArgumentException(HERE, L"group", GuiString(L"'%s' (%d) is not a recognised script object group", GetString(Group).c_str(), Group) );
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
