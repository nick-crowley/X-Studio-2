#pragma once
#include "Common.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Defines the parameter types available in vanilla x3 albion prelude</summary>
      enum class ParameterType : UINT
      {   
         // Special
         UNDETERMINED                   = 128,     // Added by me for use instead of NULL
         CONDITION                      = 4,       // Unused

         // Scripting
         REFERENCE_OBJECT               = 20,
         RETURN_VALUE                   = 15,
         RETURN_VALUE_IF                = 26,
         RETURN_VALUE_IF_START          = 28,
         INTERRUPT_RETURN_VALUE_IF      = 25,

         COMMENT                        = 8,
         LABEL_NAME                     = 1,
         LABEL_NUMBER                   = 6,
         SCRIPT_NAME                    = 7,
         VARIABLE_DATATYPE              = 44,
         VARIABLE_ENVIRONMENT           = 43,
         EXPRESSION                     = 30,      // [TRANSLATION] A single node used by infix expressions
                                                      // [GENERATION]  Every (non-ReturnObject) parameter in an expression
         PARAMETER                      = 5,       // [TRANSLATION] A node-tuple used by script-call tuples and post-fix expression tuples
                                                      // [GENERATION]  ScriptCall argument value
         // Basic Types
         ARRAY                          = 45,
         NUMBER                         = 3,
         STRING                         = 2,
         VALUE                          = 9,
         VARIABLE                       = 0,
         VARIABLE_CONSTANT              = 53,
         VARIABLE_NUMBER                = 10,
         VARIABLE_STRING                = 11,
         VARIABLE_BOOLEAN               = 63,
   
         // Ships / Stations
         VARIABLE_SHIP                  = 21,
         VARIABLE_WING                  = 61,
         VARIABLE_SHIP_STATION          = 22,
         VARIABLE_PLAYER_SHIP           = 35,
         VARIABLE_PLAYER_SHIP_STATION   = 38,
         VARIABLE_SHIPTYPE              = 19,
         VARIABLE_SHIPTYPE_STATIONTYPE  = 27,

         // Stations
         STATION_CARRIER                = 40,
         STATION_CARRIERDOCKAT          = 41,
         VARIABLE_PLAYER_STATION_CARRIER= 37,
         VARIABLE_PLAYER_STATION        = 36,
         VARIABLE_STATION               = 12,
         VARIABLE_STATIONPRODUCT        = 47,
         VARIABLE_STATIONRESOURCE       = 46,
         VARIABLE_STATIONSERIAL         = 18,
         VARIABLE_STATIONTYPE           = 17,
         VARIABLE_STATIONWARE           = 48,
         VARIABLE_HOMEBASEPRODUCT       = 50,
         VARIABLE_HOMEBASERESOURCE      = 49,
         VARIABLE_HOMEBASEWARE          = 51,

         // Passengers
         VARIABLE_PASSENGER             = 66,
         VARIABLE_PASSENGER_OF_SHIP     = 68,
         VARAIBLE_SHIP_AND_PASSENGER    = 67,
   
         // Wares
         VARIABLE_ALLWARES              = 42,
         VARIABLE_FLYINGWARE            = 55,
         VARIABLE_WARE                  = 13,
         VARIABLE_WARE_OF_SHIP          = 58,
         VARIABLE_SHIP_AND_WARE         = 57,

         // Special objects
         VARIABLE_ASTEROID              = 54,
         VARIABLE_JUMPDRIVE_GATE        = 56,
         VARIABLE_JUMPDRIVE_SECTOR      = 69,
         VARIABLE_WARPGATE              = 39,

         // Albion Prelude
         VARIABLE_FLEET_COMMANDER       = 70,
         VARIABLE_GLOBAL_PARAMETER      = 71,
   
         // Constants
         FLIGHTRETCODE                  = 34,
         OBJECTCOMMAND                  = 31,
         OBJECTCOMMAND_OBJECTSIGNAL     = 33,
         OBJECTSIGNAL                   = 32,
         RELATION                       = 29,
         SECTOR_POSITION                = 52,
         VARIABLE_CLASS                 = 23,
         VARIABLE_QUEST                 = 59,
         VARIABLE_RACE                  = 16,
         VARIABLE_SECTOR                = 14,
         VARIABLE_TRANSPORTCLASS        = 24,
         VARIABLE_WING_COMMAND          = 65,
         SCRIPT_REFERENCE_TYPE          = 62
      };

      /// <summary>Defines the purpose of the parameter</summary>
      enum class ParameterUsage { None, PageID, StringID, ScriptName };

      /// <summary></summary>
      class ParameterSyntax
      {
      public:
         class Declaration
         {
         public:
            /// <summary>Creates a declaration use by modern format reader</summary>
            //Declaration() : PhysicalIndex(0), DisplayIndex(0), Ordinal(0), PageID(false), StringID(false), Optional(false) {}

            /// <summary>Creates a declaration from legacy syntax components</summary>
            Declaration(ParameterType t, UINT physical, UINT display, UINT ord, ParameterUsage use) 
               : Type(t), PhysicalIndex(physical), DisplayIndex(display), Ordinal(ord), Usage(use)
            {}

            ParameterType   Type;
            UINT            PhysicalIndex,
                            DisplayIndex,
                            Ordinal;
            ParameterUsage  Usage;
            //bool            Optional;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         ParameterSyntax(Declaration& d);
         virtual ~ParameterSyntax();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
         /// <summary>Determines whether parameter is reference object</summary>
         /// <returns></returns>
         bool  IsRefObj() const
         {
            return Type == ParameterType::REFERENCE_OBJECT;
         }

         /// <summary>Determines whether parameter is return value</summary>
         /// <returns></returns>
         bool  IsRetVar() const
         {
            switch (Type)
            {
            case ParameterType::INTERRUPT_RETURN_VALUE_IF:
            case ParameterType::RETURN_VALUE_IF_START:
            case ParameterType::RETURN_VALUE:
            case ParameterType::RETURN_VALUE_IF:   return true;
            default: return false;
            }
         }

		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      public:
         const ParameterType  Type;
         const UINT           DisplayIndex,
                              PhysicalIndex,
                              Ordinal;
         const ParameterUsage Usage;
      };

      /// <summary>Vector of parameter syntax</summary>
      typedef vector<ParameterSyntax>  ParamSyntaxArray;
      

      // Define helpers for iterating through syntax
      //
      #define      FIRST_PARAMETER_SYNTAX            PS_VARIABLE
      #define      LAST_PARAMETER_SYNTAX             PS_VARIABLE_GLOBAL_PARAMETER
      #define      PARAMETER_SYNTAX_COUNT            72
   }
}

using namespace Logic::Scripts;
