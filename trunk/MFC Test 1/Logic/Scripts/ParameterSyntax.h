#pragma once
#include "../Common.h"

namespace Library
{
   namespace Scripts
   {
      /// <summary>Defines the parameter types available in vanilla x3 albion prelude</summary>
      enum class ParameterType : UINT; 

      /// <summary></summary>
      class ParameterSyntax
      {
      public:
         class Declaration
         {
         public:
            Declaration() 
               : PhysicalIndex(0), DisplayIndex(0), PageID(false), StringID(false), Optional(false) {}
            Declaration(ParameterType t, UINT physical, UINT display) 
               : Type(t), PhysicalIndex(physical), DisplayIndex(display), PageID(false), StringID(false), Optional(false) {}

            ParameterType   Type;
            UINT            PhysicalIndex,
                            DisplayIndex;
            bool            PageID,
                            StringID,
                            ScriptName,
                            Optional;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         ParameterSyntax(Declaration& d);
         virtual ~ParameterSyntax();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      public:
         const ParameterType  Type;
         const UINT           DisplayIndex,
                              PhysicalIndex;
         const bool           PageID,
                              StringID,
                              ScriptName,
                              Optional;
      };


      
      /// <summary>Defines the parameter types available in vanilla x3 albion prelude</summary>
      enum class ParameterType : UINT
      {   
         // Special
         PS_UNDETERMINED                   = 128,     // Added by me for use instead of NULL
         PS_CONDITION                      = 4,       // Unused

         // Scripting
         PS_REFERENCE_OBJECT               = 20,
         PS_RETURN_OBJECT                  = 15,
         PS_RETURN_OBJECT_IF               = 26,
         PS_RETURN_OBJECT_IF_START         = 28,
         PS_INTERRUPT_RETURN_OBJECT_IF     = 25,

         PS_COMMENT                        = 8,
         PS_LABEL_NAME                     = 1,
         PS_LABEL_NUMBER                   = 6,
         PS_SCRIPT_NAME                    = 7,
         PS_VARIABLE_DATATYPE              = 44,
         PS_VARIABLE_ENVIRONMENT           = 43,
         PS_EXPRESSION                     = 30,      // [TRANSLATION] A single node used by infix expressions
                                                      // [GENERATION]  Every (non-ReturnObject) parameter in an expression
         PS_PARAMETER                      = 5,       // [TRANSLATION] A node-tuple used by script-call tuples and post-fix expression tuples
                                                      // [GENERATION]  ScriptCall argument value
         // Basic Types
         PS_ARRAY                          = 45,
         PS_NUMBER                         = 3,
         PS_STRING                         = 2,
         PS_VALUE                          = 9,
         PS_VARIABLE                       = 0,
         PS_VARIABLE_CONSTANT              = 53,
         PS_VARIABLE_NUMBER                = 10,
         PS_VARIABLE_STRING                = 11,
         PS_VARIABLE_BOOLEAN               = 63,
   
         // Ships / Stations
         PS_VARIABLE_SHIP                  = 21,
         PS_VARIABLE_WING                  = 61,
         PS_VARIABLE_SHIP_STATION          = 22,
         PS_VARIABLE_PLAYER_SHIP           = 35,
         PS_VARIABLE_PLAYER_SHIP_STATION   = 38,
         PS_VARIABLE_SHIPTYPE              = 19,
         PS_VARIABLE_SHIPTYPE_STATIONTYPE  = 27,

         // Stations
         PS_STATION_CARRIER                = 40,
         PS_STATION_CARRIERDOCKAT          = 41,
         PS_VARIABLE_PLAYER_STATION_CARRIER= 37,
         PS_VARIABLE_PLAYER_STATION        = 36,
         PS_VARIABLE_STATION               = 12,
         PS_VARIABLE_STATIONPRODUCT        = 47,
         PS_VARIABLE_STATIONRESOURCE       = 46,
         PS_VARIABLE_STATIONSERIAL         = 18,
         PS_VARIABLE_STATIONTYPE           = 17,
         PS_VARIABLE_STATIONWARE           = 48,
         PS_VARIABLE_HOMEBASEPRODUCT       = 50,
         PS_VARIABLE_HOMEBASERESOURCE      = 49,
         PS_VARIABLE_HOMEBASEWARE          = 51,

         // Passengers
         PS_VARIABLE_PASSENGER             = 66,
         PS_VARIABLE_PASSENGER_OF_SHIP     = 68,
         PS_VARAIBLE_SHIP_AND_PASSENGER    = 67,
   
         // Wares
         PS_VARIABLE_ALLWARES              = 42,
         PS_VARIABLE_FLYINGWARE            = 55,
         PS_VARIABLE_WARE                  = 13,
         PS_VARIABLE_WARE_OF_SHIP          = 58,
         PS_VARIABLE_SHIP_AND_WARE         = 57,

         // Special objects
         PS_VARIABLE_ASTEROID              = 54,
         PS_VARIABLE_JUMPDRIVE_GATE        = 56,
         PS_VARIABLE_JUMPDRIVE_SECTOR      = 69,
         PS_VARIABLE_WARPGATE              = 39,

         // Albion Prelude
         PS_VARIABLE_FLEET_COMMANDER       = 70,
         PS_VARIABLE_GLOBAL_PARAMETER      = 71,
   
         // Constants
         PS_FLIGHTRETCODE                  = 34,
         PS_OBJECTCOMMAND                  = 31,
         PS_OBJECTCOMMAND_OBJECTSIGNAL     = 33,
         PS_OBJECTSIGNAL                   = 32,
         PS_RELATION                       = 29,
         PS_SECTOR_POSITION                = 52,
         PS_VARIABLE_CLASS                 = 23,
         PS_VARIABLE_QUEST                 = 59,
         PS_VARIABLE_RACE                  = 16,
         PS_VARIABLE_SECTOR                = 14,
         PS_VARIABLE_TRANSPORTCLASS        = 24,
         PS_VARIABLE_WING_COMMAND          = 65,
         PS_SCRIPT_REFERENCE_TYPE          = 62
      };

      // Define helpers for iterating through syntax
      //
      #define      FIRST_PARAMETER_SYNTAX            PS_VARIABLE
      #define      LAST_PARAMETER_SYNTAX             PS_VARIABLE_GLOBAL_PARAMETER
      #define      PARAMETER_SYNTAX_COUNT            72
   }
}
