#include "stdafx.h"
#include "ParameterSyntax.h"
#include "StringLibrary.h"
#include "ScriptObjectLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      
      /// <summary>Infix expression parameter. [OUTPUT]</summary>
      const ParameterSyntax  ParameterSyntax::InfixParameter(ParameterType::EXPRESSION_INFIX, 0, 0);

      /// <summary>Expression parameter, immune from static type checking. [INPUT/OUTPUT]</summary>
      const ParameterSyntax  ParameterSyntax::ExpressionParameter(ParameterType::EXPRESSION, 1, 1);

      /// <summary>Goto/gosub label name syntax. [INPUT]</summary>
      const ParameterSyntax  ParameterSyntax::LabelNameParameter(ParameterType::LABEL_NAME, 0, 0);

      /// <summary>Unconditional jump destination syntax. [OUTPUT]</summary>
      const ParameterSyntax  ParameterSyntax::LabelNumberParameter(ParameterType::LABEL_NUMBER, 0, 0);

      /// <summary>Expression parameter count or script-call argument count. [OUTPUT]</summary>
      const ParameterSyntax  ParameterSyntax::StructuralCount(ParameterType::STRUCTURAL_COUNT, 0, 0);
      
      /// <summary>Variable argument script call argument syntax. [INPUT/OUTPUT]</summary>
      const ParameterSyntax  ParameterSyntax::ScriptCallArgument(ParameterType::PARAMETER, 3, 3);

      /// <summary>Variable argument script call argument syntax. [INPUT/OUTPUT]</summary>
      //const ParameterSyntax  ParameterSyntax::ScriptCallVArgument(ParameterType::VALUE, 3, 3);

      /// <summary>Syntax for parameters of an unrecognised command. [UNUSED]</summary>
      const ParameterSyntax  ParameterSyntax::UnrecognisedCommand(ParameterType::UNRECOGNISED, 0, 0);
      
      // -------------------------------- CONSTRUCTION --------------------------------
      
      /// <summary>Private ctor used by sentinel values</summary>
      /// <param name="t">Parameter type</param>
      /// <param name="physical">physical index</param>
      /// <param name="display">display index</param>
      ParameterSyntax::ParameterSyntax(ParameterType t, UINT physical, UINT display)
         : Type(t), PhysicalIndex(physical), DisplayIndex(display), Ordinal(0), Usage(ParameterUsage::None)
      {
      }

      /// <summary>Create syntax from a declaration</summary>
      /// <param name="d">The declaration</param>
      ParameterSyntax::ParameterSyntax(Declaration& d)
         : Type(d.Type), PhysicalIndex(d.PhysicalIndex), DisplayIndex(d.DisplayIndex), Ordinal(d.Ordinal), Usage(d.Usage)
      {
      }

      ParameterSyntax::~ParameterSyntax()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get parameter type string</summary>
      LogicExport GuiString  GetString(ParameterType t)
      {
         const ScriptObject* str=nullptr;
         return ScriptObjectLib.TryFind(ScriptObjectGroup::ParameterType, (UINT)t, str) ? str->Text : GuiString::Missing((UINT)t);
      }

      /// <summary>Write parameter syntax to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ParameterSyntax& ps)
      {
         return c << "{ParameterSyntax: Type=" << GetString(ps.Type) << "}";
      }

      /// <summary>Verifies a parameter type against a parameter data-type</summary>
      /// <param name="pt">Parameter type</param>
      /// <param name="dt">Datatype</param>
      /// <returns></returns>
      bool  ParameterSyntax::Verify(ParameterType pt, DataType dt) 
      {
         // [NULL] Must not be RetVar
         if (dt == DataType::Null)
            switch (pt)
            {
            case ParameterType::INTERRUPT_RETURN_VALUE_IF:
            case ParameterType::RETURN_VALUE_IF_START:
            case ParameterType::RETURN_VALUE:
            case ParameterType::RETURN_VALUE_IF:   
               return false;

            default: 
               return true;
            }
         
         // Determine whether datatype matches syntax
         switch (pt)
         {
         // [INVALID] Not used
	      case ParameterType::UNRECOGNISED:
         case ParameterType::STRUCTURAL_COUNT:
         case ParameterType::EXPRESSION_INFIX:
         case ParameterType::CONDITION:  
            throw InvalidOperationException(HERE, VString(L"Cannot verify '%s' parameter syntax", GetString(pt).c_str()));

         // [USER CUSTOMIZED] Unable to determine, allow anything
         default:
            return true;

         // [UNIVERSAL] These can be any datatype
         case ParameterType::VALUE:
         case ParameterType::EXPRESSION:
         case ParameterType::PARAMETER: 
            return true;

         // [RETURN VALUE] Always held as variables
	      case ParameterType::RETURN_VALUE:
	      case ParameterType::RETURN_VALUE_IF:
	      case ParameterType::RETURN_VALUE_IF_START:
	      case ParameterType::INTERRUPT_RETURN_VALUE_IF:
            return dt == DataType::VARIABLE;

         // [FLIGHT RETURN] Stored using their own datatypes
	      case ParameterType::FLIGHTRETCODE:
            return dt == DataType::FLIGHTRETURN || dt == DataType::VARIABLE;

         // [LABEL NUMBER] Goto/gosub - displayed as string
         case ParameterType::LABEL_NUMBER: 
            return dt == DataType::STRING;

         // [LABEL NAME, SCRIPT NAME, STRING, COMMENT] Always held as strings
         case ParameterType::LABEL_NAME:
	      case ParameterType::SCRIPT_NAME:
         case ParameterType::COMMENT:
         case ParameterType::STRING:
            return dt == DataType::STRING;

         // [NUMBER] Integers only
         case ParameterType::NUMBER:
            return dt == DataType::INTEGER;

         // [REFERENCE OBJECT] Constants and Variables only
	      case ParameterType::REFERENCE_OBJECT:
            return dt == DataType::VARIABLE || dt == DataType::CONSTANT;
   
         // [RELATIONS] Stored using their own datatypes
	      case ParameterType::RELATION:
            return dt == DataType::RELATION || dt == DataType::VARIABLE;

         // [SCRIPT REFERENCE TYPE]
         case ParameterType::SCRIPT_REFERENCE_TYPE:
            return dt == DataType::SCRIPTDEF || dt == DataType::VARIABLE;

         // [SECTOR POSITION] What about DataType::ARRAY?
	      case ParameterType::SECTOR_POSITION:
            return dt == DataType::VARIABLE;

         // [VARIABLE]
	      case ParameterType::VARIABLE:
            return dt == DataType::VARIABLE;



         
         // [ARRAY / VARIABLE]
	      case ParameterType::ARRAY:
            return dt == DataType::VARIABLE;

         // [BOOLEAN / VARIABLE]
         case ParameterType::VARIABLE_BOOLEAN:  /// Compatible with: [TRUE] and [FALSE]
            return dt == DataType::CONSTANT || dt == DataType::VARIABLE;

         // [CONSTANT / VARIABLE]
	      case ParameterType::VARIABLE_CONSTANT:
            return dt == DataType::CONSTANT || dt == DataType::VARIABLE;

         // [COMMAND / SIGNAL] 
	      case ParameterType::OBJECTCOMMAND:     
	      case ParameterType::OBJECTCOMMAND_OBJECTSIGNAL:
	      case ParameterType::OBJECTSIGNAL:
            return dt == DataType::OBJECTCOMMAND || dt == DataType::VARIABLE;

         // [DATATYPE / VARIABLE]
	      case ParameterType::VARIABLE_DATATYPE:
            return dt == DataType::DATATYPE || dt == DataType::VARIABLE;

         // [ENVIRONMENT / VARIABLE] 
         case ParameterType::VARIABLE_ENVIRONMENT: /// Compatible with: [ENVIRONMENT]
            return dt == DataType::CONSTANT || dt == DataType::VARIABLE;

         // [GLOBAL PARAMETER] Stored as string
         case ParameterType::VARIABLE_GLOBAL_PARAMETER:     /// NEW: Work in progress
            return dt == DataType::STRING || dt == DataType::VARIABLE;

         // [NUMBER / VARIABLE] Integer, Constant or Variable
         case ParameterType::VARIABLE_NUMBER:      /// Compatible with: [TRUE], [FALSE], [MAX]?
            return dt == DataType::INTEGER || dt == DataType::CONSTANT || dt == DataType::VARIABLE;

         // [OBJECT CLASS / VARIABLE] Stored using it's own datatypes
	      case ParameterType::VARIABLE_CLASS:
            return dt == DataType::OBJECTCLASS || dt == DataType::VARIABLE; // || dt == DataType::CONSTANT;

         // [PASSENGER / VARIABLE] What about DataType::PASSENGER?
         case ParameterType::VARIABLE_PASSENGER:
         case ParameterType::VARIABLE_PASSENGER_OF_SHIP:
            return dt == DataType::VARIABLE;

         // [QUEST / VARIABLE]      
         case ParameterType::VARIABLE_QUEST:    /// Compatible with: Quest.xxx constants
            return dt == DataType::VARIABLE || dt == DataType::QUEST || dt == DataType::CONSTANT;

         // [RACE / VARIABLE]
         case ParameterType::VARIABLE_RACE:     /// Possibly related: [OWNER]
            return dt == DataType::VARIABLE || dt == DataType::RACE || dt == DataType::CONSTANT;

         // [SECTOR / VARIABLE] 
         case ParameterType::VARIABLE_SECTOR:   /// Compatible with: [SECTOR]
            return dt == DataType::VARIABLE || dt == DataType::SECTOR || dt == DataType::CONSTANT;

         // [SHIP TYPE / STATION TYPE]
         case ParameterType::VARIABLE_SHIPTYPE:
	      case ParameterType::VARIABLE_SHIPTYPE_STATIONTYPE:
         case ParameterType::VARIABLE_STATIONTYPE:
            return dt == DataType::OBJECTCLASS || dt == DataType::VARIABLE || dt == DataType::WARE;

         // [SHIP / STATION / VARIABLE] Ship, station, constant or variable
         case ParameterType::VARIABLE_SHIP:              /// Compatible with: [SHIP]
         case ParameterType::VARIABLE_PLAYER_SHIP:
	      case ParameterType::VARIABLE_SHIP_STATION:
         case ParameterType::VARIABLE_PLAYER_SHIP_STATION:
	      case ParameterType::STATION_CARRIER:
	      case ParameterType::STATION_CARRIERDOCKAT:
         case ParameterType::VARIABLE_PLAYER_STATION_CARRIER:
	      case ParameterType::VARIABLE_PLAYER_STATION:
	      case ParameterType::VARIABLE_STATION:
	      case ParameterType::VARIABLE_STATIONPRODUCT:
	      case ParameterType::VARIABLE_STATIONRESOURCE:
	         return dt == DataType::SHIP || dt == DataType::STATION || dt == DataType::CONSTANT || 
                   dt == DataType::WARE || dt == DataType::VARIABLE;

         // [STATION SERIAL / VARAIBLE]
         case ParameterType::VARIABLE_STATIONSERIAL:
            return dt == DataType::VARIABLE || dt == DataType::STATIONSERIAL;

         // [STRING / VARIABLE] Variable, string or constant
         case ParameterType::VARIABLE_STRING:
            return dt == DataType::STRING || dt == DataType::VARIABLE || dt == DataType::CONSTANT;

         // [TRANSPORT CLASS / VARIABLE]
	      case ParameterType::VARIABLE_TRANSPORTCLASS:
            return dt == DataType::VARIABLE || dt == DataType::TRANSPORTCLASS; 

         // [UNIVERSE OBJECTS]
	      case ParameterType::VARIABLE_ASTEROID:
	      case ParameterType::VARIABLE_WARPGATE:
            return dt == DataType::VARIABLE;

         // [WARE / VARIABLE]
	      case ParameterType::VARIABLE_STATIONWARE:
	      case ParameterType::VARIABLE_HOMEBASEPRODUCT:
	      case ParameterType::VARIABLE_HOMEBASERESOURCE:
	      case ParameterType::VARIABLE_HOMEBASEWARE:
         case ParameterType::VARIABLE_ALLWARES:
	      case ParameterType::VARIABLE_FLYINGWARE:
	      case ParameterType::VARIABLE_WARE:
	      case ParameterType::VARIABLE_WARE_OF_SHIP:
            return dt == DataType::WARE || dt == DataType::VARIABLE;

         // [WING / VARIABLE]
         case ParameterType::VARIABLE_WING:     /// Compatible with Wing.xxx constants
            return dt == DataType::CONSTANT || dt == DataType::VARIABLE;
   
         // [WING COMMAND / VARIABLE]
         case ParameterType::VARIABLE_WING_COMMAND:
            return dt == DataType::WINGCOMMAND || dt == DataType::VARIABLE;




         // [SHIP AND WARE/PASSENGER] 
	      case ParameterType::VARIABLE_SHIP_AND_WARE:
         case ParameterType::VARAIBLE_SHIP_AND_PASSENGER:
         // [JUMP-DRIVE GATE/SECTOR] 
         case ParameterType::VARIABLE_JUMPDRIVE_GATE:
         case ParameterType::VARIABLE_JUMPDRIVE_SECTOR:
         // [FLEET COMMANDER] 
         case ParameterType::VARIABLE_FLEET_COMMANDER:   
            return dt == DataType::WARE || dt == DataType::VARIABLE;
         }
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Verifies this syntax against a data-type</summary>
      /// <returns></returns>
      bool  ParameterSyntax::Verify(DataType t) const
      {
         return Verify(Type, t);
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
