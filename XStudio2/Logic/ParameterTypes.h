#pragma once
#include "Common.h"

namespace Logic
{
   namespace Scripts
   {
      
      /// <summary>Modifiers added to various types or values</summary>
      #define   DTF_ENCODED           0x20000
      #define   DTF_OBJECT_DATA       0x10000
      #define   DTF_UNARY_OPERATOR    0x10000

      /// <summary>Defines the type element of a X3 script {type,value} pair</summary>
      enum class DataType : UINT
      {    
         Null            = 0,   UNKNOWN        = 1,   VARIABLE      = 2,
         CONSTANT        = 3,   INTEGER        = 4,   STRING        = 5,
         SHIP            = 6,   STATION        = 7,   SECTOR        = 8,
         WARE            = 9,   RACE           = 10,  STATIONSERIAL = 11,
         OBJECTCLASS     = 12,  TRANSPORTCLASS = 13,  RELATION      = 14,
         OPERATOR        = 15,  EXPRESSION     = 16,  OBJECT        = 17,
         OBJECTCOMMAND   = 18,  FLIGHTRETURN   = 19,  DATATYPE      = 20,
         ARRAY           = 21,  QUEST          = 22,
         // New in Terran Conflict
         WING            = 23,  SCRIPTDEF      = 24,  WINGCOMMAND   = 25,
         PASSENGER       = 26,
         // Live Types
         LIVE_CONSTANT  = CONSTANT | DTF_ENCODED,
         LIVE_VARIABLE  = VARIABLE | DTF_ENCODED,
         LIVE_OBJECT    = OBJECT   | DTF_OBJECT_DATA,
         LIVE_SECTOR    = SECTOR   | DTF_OBJECT_DATA,
         LIVE_SHIP      = SHIP     | DTF_OBJECT_DATA,
         LIVE_STATION   = STATION  | DTF_OBJECT_DATA,
         LIVE_WING      = WING     | DTF_OBJECT_DATA,
         UNARY_OPERATOR = OPERATOR | DTF_UNARY_OPERATOR    
      };
      
      /// <summary>Get datatype string</summary>
      GuiString GetString(DataType d);

      
      /// <summary>Defines operator values</summary>
      enum class Operator : UINT { Equal, NotEqual, Greater, Less, GreaterEqual, LessEqual, BitwiseAnd, BitwiseOr, BitwiseXor, LogicalAnd, LogicalOr, Add, Subtract, Multiply, Divide, Modulus, OpenBracket, CloseBracket, BitwiseNot, Minus, LogicalNot };

      /// <summary>Defines relation values</summary>
      enum class Relation : UINT { Foe=(UINT)-1, Neutral=0, Friend=1 };

      /// <summary>Defines station serials</summary>
      enum class Serial : UINT { alpha, beta, gamma, delta, epsilon, zeta, eta, theta, iota, kappa, lambda, mu, nu, xi, omicron, pi, rho, sigma, tau, upsilon, phi, chi, psi, omega1, omega2 };
      
      /// <summary>Converts between sector IDs and co-ordinates</summary>
      class SectorIDConverter
      {
      public:
         /// <summary>Convert string ID to coordinates.</summary>
         /// <param name="stringID">The string identifier.</param>
         /// <returns></returns>
         /// <exception cref="Logic::InvalidValueException">Invalid string ID</exception>
         static pair<int, int>  ToCoordinates(UINT stringID)
         {
            VString id(L"%d", stringID);

            // Ensure valid
            if (id.length() != 7 || id.substr(0,3) != L"102")
                throw InvalidValueException(HERE, VString(L"%d is not a valid sector string ID", stringID));

            // Extract '102xxyy'
            int x = _ttoi(id.substr(3,2).c_str());
            int y = _ttoi(id.substr(5,2).c_str());
            return pair<int,int>(x,y);
         }
      };

      /// <summary>Converts between station serials and string IDs</summary>
      class StationSerialConverter
      {
      public:
         /// <summary>Convert string ID to station serial.</summary>
         /// <param name="stringID">The string identifier.</param>
         /// <returns></returns>
         static Serial  ToSerial(UINT stringID)
         {
            return Serial(stringID - 100);
         }

         /// <summary>Convert station serial to string ID.</summary>
         /// <param name="stringID">The station serial.</param>
         /// <returns></returns>
         static UINT  ToStringID(Serial serial)
         {
            return (UINT)serial + 100;
         }
      };
      
      /// <summary>Converts between relation enum and string IDs</summary>
      class RelationIDConverter
      {
      public:
         /// <summary>Convert string ID to relation.</summary>
         /// <param name="stringID">The string identifier.</param>
         /// <returns>Relation</returns>
         /// <exception cref="Logic::InvalidValueException">Invalid string ID</exception>
         static Relation  ToRelation(UINT stringID)
         {
            switch (stringID)
            {
            case 1102423:  return Relation::Foe;
            case 1102424:  return Relation::Neutral;
            case 1102422:  return Relation::Friend;
            default:
               throw InvalidValueException(HERE, VString(L"%d is not a valid relation string ID", stringID));
            }
         }

         /// <summary>Convert relation to string ID</summary>
         /// <param name="r">relation</param>
         /// <returns>The string identifier</returns>
         /// <exception cref="Logic::InvalidValueException">Invalid relation</exception>
         static UINT  ToStringID(Relation r)
         {
            switch (r)
            {
            case Relation::Foe:      return 1102423; 
            case Relation::Neutral:  return 1102424; 
            case Relation::Friend:   return 1102422; 
            default:
               throw InvalidValueException(HERE, VString(L"%d is not a valid relation value", r));
            }
         }
      };


      /// <summary>Defines the parameter types available in vanilla x3 albion prelude</summary>
      enum class ParameterType : UINT
      {   
         // Special
         UNRECOGNISED                   = 128,     // Added by me for use instead of NULL
         EXPRESSION_INFIX               = 127,
         STRUCTURAL_COUNT               = 126,
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
         PARAMETER                      = 5,       // Used for varg ScriptCall argument sentinel syntax

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


   }
}

using namespace Logic::Scripts;
