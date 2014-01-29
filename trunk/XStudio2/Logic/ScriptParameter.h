#pragma once

#include "Common.h"
#include "ParameterSyntax.h"
#include "ReturnValue.h"
#include "ScriptToken.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;

      
      /// <summary>Defines current type of parameter value union</summary>
      enum class ValueType { String, Int };

      /// <summary>Holds a union value of String/integer</summary>
      class ParameterValue
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ParameterValue() : Type(ValueType::Int), Int(0) 
         {}
         ParameterValue(const wstring& sz) : Type(ValueType::String) { 
            String = sz; 
         }
         ParameterValue(int val) : Type(ValueType::Int) { 
            Int = val; 
         }

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(int,HighWord,GetHighWord);
         PROPERTY_GET(int,LowWord,GetLowWord);

         // ---------------------- ACCESSORS ------------------------			

         int  GetHighWord() const
         {
            return HIWORD(Int);
         }
         int  GetLowWord() const
         {
            return LOWORD(Int);
         }

		   // ----------------------- MUTATORS ------------------------

         /// <summary>Integer assignment</summary>
         ParameterValue& operator=(int val)
         {
            Type = ValueType::Int;
            Int = val;
            return *this;
         }

         /// <summary>String assignment</summary>
         ParameterValue& operator=(const wstring& sz)
         {
            Type = ValueType::String;
            String = sz;
            return *this;
         }

         // -------------------- REPRESENTATION ---------------------

         ValueType Type;
         wstring   String;
         int       Int;
      };


      /// <summary>Represents a script command parameter</summary>
      class ScriptParameter
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptParameter(ParameterSyntax s, const ScriptToken& t);
         ScriptParameter(ParameterSyntax s, Conditional c);
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
         ScriptToken     Token;
      };

      /// <summary>Vector of script parameters</summary>
      class ParameterArray : public vector<ScriptParameter> 
      {
      public:
         ParameterArray()
         {}
         ParameterArray(const ScriptParameter& p)
         {
            push_back(p);
         }
         ParameterArray& operator +=(const ScriptParameter& p)
         {
            push_back(p);
            return *this;
         }
      };
   }
}

using namespace Logic::Scripts;
