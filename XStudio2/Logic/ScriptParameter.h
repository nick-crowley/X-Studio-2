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
         /// <summary>Create zero integer</summary>
         ParameterValue() : Type(ValueType::Int), Int(0) 
         {}
         /// <summary>Create from integer value</summary>
         ParameterValue(const wstring& sz) : Type(ValueType::String) { 
            String = sz; 
         }
         /// <summary>Create from string value</summary>
         ParameterValue(int val) : Type(ValueType::Int) { 
            Int = val; 
         }

         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(int,HighWord,GetHighWord);
         PROPERTY_GET(int,LowWord,GetLowWord);

         // ---------------------- ACCESSORS ------------------------			

         /// <summary>Get HO word of integer value</summary>
         int  GetHighWord() const
         {
            return HIWORD(Int);
         }

         /// <summary>Get LO word of integer value</summary>
         int  GetLowWord() const
         {
            return LOWORD(Int);
         }

		   // ----------------------- MUTATORS ------------------------

         /// <summary>Assign integer value</summary>
         ParameterValue& operator=(int val)
         {
            Type = ValueType::Int;
            Int = val;
            return *this;
         }

         /// <summary>Assign string value</summary>
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
         ScriptParameter(const ParameterSyntax& s, const ScriptToken& t);
         ScriptParameter(const ParameterSyntax& s, Conditional c);
         ScriptParameter(const ParameterSyntax& s, DataType t, ParameterValue val);
         ScriptParameter(const ParameterSyntax& s, DataType t, const wstring& val);
         ScriptParameter(const ParameterSyntax& s, DataType t, UINT val);
         virtual ~ScriptParameter();

         // ------------------------ STATIC -------------------------
      public:

      protected:
         static DataType  IdentifyDataType(ScriptToken tok);
         static const ParameterSyntax&  ModifySyntax(const ParameterSyntax& s);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			
      
		   // ----------------------- MUTATORS ------------------------

      public:
         void  Generate(ScriptFile& script);
         void  Translate(ScriptFile& script);

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
         /// <summary>Create from single parameter</summary>
         ParameterArray(const ScriptParameter& p)
         {
            push_back(p);
         }

         /// <summary>Append parameter</summary>
         ParameterArray& operator +=(const ScriptParameter& p)
         {
            push_back(p);
            return *this;
         }

         /// <summary>Append parameter array</summary>
         ParameterArray& operator +=(const ParameterArray& arr)
         {
            for (const ScriptParameter& p : arr)
               push_back(p);
            return *this;
         }
      };
   }
}

using namespace Logic::Scripts;
