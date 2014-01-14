#pragma once

#include "Common.h"
#include "ParameterSyntax.h"
#include "ReturnValue.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;

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

  
      /// <summary>Defines current type of parameter value union</summary>
      enum class ValueType { String, Int };

      /// <summary>Holds a union value of String/integer</summary>
      class ParameterValue
      {
      public:
         ParameterValue(wstring sz) : Type(ValueType::String) { String = sz; }
         ParameterValue(int val) : Type(ValueType::Int) { Int = val; }

         ValueType Type;
         wstring   String;
         int       Int;
      };


      /// <summary>Represents a script command parameter</summary>
      class ScriptParameter
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptParameter(ParameterSyntax s, DataType t, ParameterValue val);
         ScriptParameter(ParameterSyntax s, DataType t, const wstring& val);
         ScriptParameter(ParameterSyntax s, DataType t, UINT val);
         virtual ~ScriptParameter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

      public:
         void  Translate(ScriptFile& f);
         void  Generate() {}

		   // -------------------- REPRESENTATION ---------------------

      public:
         DataType        Type;
         ParameterValue  Value;
         ParameterSyntax Syntax;
         wstring         Text;
      };

      /// <summary>Vector of script parameters</summary>
      typedef vector<ScriptParameter> ParameterArray;
   }
}

using namespace Logic::Scripts;
