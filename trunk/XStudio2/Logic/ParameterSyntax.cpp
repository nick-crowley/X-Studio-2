#include "stdafx.h"
#include "ParameterSyntax.h"
#include "StringLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Expression parameter syntax</summary>
      const ParameterSyntax  ParameterSyntax::ExpressionParameter(ParameterType::EXPRESSION, 1, 1);

      /// <summary>Variable argument script call argument syntax</summary>
      const ParameterSyntax  ParameterSyntax::ScriptCallArgument(ParameterType::PARAMETER, 3, 3);

      /// <summary>Goto/gosub label name syntax</summary>
      const ParameterSyntax  ParameterSyntax::LabelDeclaration(ParameterType::LABEL_NAME, 0, 0);

      /// <summary>Syntax for parameters of an unrecognised command</summary>
      const ParameterSyntax  ParameterSyntax::UnrecognisedCommand(ParameterType::UNDETERMINED, 0, 0);
      
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
      GuiString  GetString(ParameterType t)
      {
         const LanguageString* str;
         return StringLib.TryFind(KnownPage::PARAMETER_TYPES, (UINT)t, str) ? str->Text : GuiString(L"Error: Missing string %d", (UINT)t);
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Verifies a data-type against the syntax.</summary>
      /// <returns></returns>
      bool  ParameterSyntax::Verify(DataType t) const
      {
         // [NULL] Must not be RetVar
         if (t == DataType::Null)
            return !IsRetVar();

         // Determine whether datatype matches syntax
         switch (Type)
         {
         // [INVALID] Not used
	      case ParameterType::UNDETERMINED:
         case ParameterType::CONDITION:  
         case ParameterType::LABEL_NUMBER:
            throw InvalidOperationException(HERE, GuiString(L"Cannot verify '%s' parameter syntax", GetString(Type).c_str()));


         // [USER CUSTOMIZED] Unable to determine, allow anything
         default:
            return true;


         // [RETURN VALUE] Always held as variables
	      case ParameterType::RETURN_VALUE:
	      case ParameterType::RETURN_VALUE_IF:
	      case ParameterType::RETURN_VALUE_IF_START:
	      case ParameterType::INTERRUPT_RETURN_VALUE_IF:
            return t == DataType::VARIABLE;


         // [UNIVERSAL] These can be any datatype
         case ParameterType::VALUE:
         case ParameterType::EXPRESSION:
         case ParameterType::PARAMETER: 
            return true;

         // [FLIGHT RETURN] Stored using their own datatypes
	      case ParameterType::FLIGHTRETCODE:
            return t == DataType::FLIGHTRETURN || t == DataType::VARIABLE;

         // [LABEL NAME, SCRIPT NAME, STRING, COMMENT] Always held as strings
	      case ParameterType::LABEL_NAME:
	      case ParameterType::SCRIPT_NAME:
         case ParameterType::COMMENT:
         case ParameterType::STRING:
            return t == DataType::STRING;

         // [NUMBER] Integers only
         case ParameterType::NUMBER:
            return t == DataType::INTEGER;

         // [REFERENCE OBJECT] Constants and Variables only
	      case ParameterType::REFERENCE_OBJECT:
            return t == DataType::VARIABLE || t == DataType::CONSTANT;
   
         // [RELATIONS] Stored using their own datatypes
	      case ParameterType::RELATION:
            return t == DataType::RELATION || t == DataType::VARIABLE;

         // [SCRIPT REFERENCE TYPE]
         case ParameterType::SCRIPT_REFERENCE_TYPE:
            return t == DataType::SCRIPTDEF || t == DataType::VARIABLE;

         // [SECTOR POSITION] What about DataType::ARRAY?
	      case ParameterType::SECTOR_POSITION:
            return t == DataType::VARIABLE;

         // [VARIABLE]
	      case ParameterType::VARIABLE:
            return t == DataType::VARIABLE;



         
         // [ARRAY / VARIABLE]
	      case ParameterType::ARRAY:
            return t == DataType::VARIABLE;

         // [BOOLEAN / VARIABLE]
         case ParameterType::VARIABLE_BOOLEAN:  /// Compatible with: [TRUE] and [FALSE]
            return t == DataType::CONSTANT || t == DataType::VARIABLE;

         // [CONSTANT / VARIABLE]
	      case ParameterType::VARIABLE_CONSTANT:
            return t == DataType::CONSTANT || t == DataType::VARIABLE;

         // [COMMAND / SIGNAL] 
	      case ParameterType::OBJECTCOMMAND:     
	      case ParameterType::OBJECTCOMMAND_OBJECTSIGNAL:
	      case ParameterType::OBJECTSIGNAL:
            return t == DataType::OBJECTCOMMAND || t == DataType::VARIABLE;

         // [DATATYPE / VARIABLE]
	      case ParameterType::VARIABLE_DATATYPE:
            return t == DataType::DATATYPE || t == DataType::VARIABLE;

         // [ENVIRONMENT / VARIABLE] 
         case ParameterType::VARIABLE_ENVIRONMENT: /// Compatible with: [ENVIRONMENT]
            return t == DataType::CONSTANT || t == DataType::VARIABLE;

         // [GLOBAL PARAMETER] Stored as string
         case ParameterType::VARIABLE_GLOBAL_PARAMETER:     /// NEW: Work in progress
            return t == DataType::STRING || t == DataType::VARIABLE;

         // [NUMBER / VARIABLE] Integer, Constant or Variable
         case ParameterType::VARIABLE_NUMBER:      /// Compatible with: [TRUE], [FALSE], [MAX]?
            return t == DataType::INTEGER || t == DataType::CONSTANT || t == DataType::VARIABLE;

         // [OBJECT CLASS / VARIABLE] Stored using it's own datatypes
	      case ParameterType::VARIABLE_CLASS:
            return t == DataType::OBJECTCLASS || t == DataType::VARIABLE; // || t == DataType::CONSTANT;

         // [PASSENGER / VARIABLE] What about DataType::PASSENGER?
         case ParameterType::VARIABLE_PASSENGER:
         case ParameterType::VARIABLE_PASSENGER_OF_SHIP:
            return t == DataType::VARIABLE;

         // [QUEST / VARIABLE]      
         case ParameterType::VARIABLE_QUEST:    /// Compatible with: Quest.xxx constants
            return t == DataType::VARIABLE || t == DataType::QUEST || t == DataType::CONSTANT;

         // [RACE / VARIABLE]
         case ParameterType::VARIABLE_RACE:     /// Possibly related: [OWNER]
            return t == DataType::VARIABLE || t == DataType::RACE || t == DataType::CONSTANT;

         // [SECTOR / VARIABLE] 
         case ParameterType::VARIABLE_SECTOR:   /// Compatible with: [SECTOR]
            return t == DataType::VARIABLE || t == DataType::SECTOR || t == DataType::CONSTANT;

         // [SHIP TYPE / STATION TYPE]
         case ParameterType::VARIABLE_SHIPTYPE:
	      case ParameterType::VARIABLE_SHIPTYPE_STATIONTYPE:
         case ParameterType::VARIABLE_STATIONTYPE:
            return t == DataType::OBJECTCLASS || t == DataType::VARIABLE || t == DataType::WARE;

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
	         return t == DataType::SHIP || t == DataType::STATION || t == DataType::CONSTANT || 
                   t == DataType::WARE || t == DataType::VARIABLE;

         // [STATION SERIAL / VARAIBLE]
         case ParameterType::VARIABLE_STATIONSERIAL:
            return t == DataType::VARIABLE || t == DataType::STATIONSERIAL;

         // [STRING / VARIABLE] Variable, string or constant
         case ParameterType::VARIABLE_STRING:
            return t == DataType::STRING || t == DataType::VARIABLE || t == DataType::CONSTANT;

         // [TRANSPORT CLASS / VARIABLE]
	      case ParameterType::VARIABLE_TRANSPORTCLASS:
            return t == DataType::VARIABLE || t == DataType::TRANSPORTCLASS; 

         // [UNIVERSE OBJECTS]
	      case ParameterType::VARIABLE_ASTEROID:
	      case ParameterType::VARIABLE_WARPGATE:
            return t == DataType::VARIABLE;

         // [WARE / VARIABLE]
	      case ParameterType::VARIABLE_STATIONWARE:
	      case ParameterType::VARIABLE_HOMEBASEPRODUCT:
	      case ParameterType::VARIABLE_HOMEBASERESOURCE:
	      case ParameterType::VARIABLE_HOMEBASEWARE:
         case ParameterType::VARIABLE_ALLWARES:
	      case ParameterType::VARIABLE_FLYINGWARE:
	      case ParameterType::VARIABLE_WARE:
	      case ParameterType::VARIABLE_WARE_OF_SHIP:
            return t == DataType::WARE || t == DataType::VARIABLE;

         // [WING / VARIABLE]
         case ParameterType::VARIABLE_WING:     /// Compatible with Wing.xxx constants
            return t == DataType::CONSTANT || t == DataType::VARIABLE;
   
         // [WING COMMAND / VARIABLE]
         case ParameterType::VARIABLE_WING_COMMAND:
            return t == DataType::WINGCOMMAND || t == DataType::VARIABLE;




         // [SHIP AND WARE/PASSENGER] 
	      case ParameterType::VARIABLE_SHIP_AND_WARE:
         case ParameterType::VARAIBLE_SHIP_AND_PASSENGER:
         // [JUMP-DRIVE GATE/SECTOR] 
         case ParameterType::VARIABLE_JUMPDRIVE_GATE:
         case ParameterType::VARIABLE_JUMPDRIVE_SECTOR:
         // [FLEET COMMANDER] 
         case ParameterType::VARIABLE_FLEET_COMMANDER:   
            return t == DataType::WARE || t == DataType::VARIABLE;
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
