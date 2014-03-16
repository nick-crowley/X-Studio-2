#pragma once

#include "ParameterSyntax.h"
#include "ReturnValue.h"
#include "ScriptToken.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;
         
      /// <summary>Sentinel value for unspecified jump destination</summary>
      const unsigned short EMPTY_JUMP = 0xffff;
      
      /// <summary>Defines current type of parameter value union</summary>
      enum class ValueType { String, Int };

      /// <summary>Get value type string</summary>
      LogicExport GuiString  GetString(ValueType v);

      /// <summary>Holds a union value of String/integer</summary>
      class LogicExport ParameterValue
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create zero integer</summary>
         ParameterValue() : Type(ValueType::Int), Int(0) 
         {}
         /// <summary>Create from integer value</summary>
         explicit ParameterValue(const wstring& sz) : Type(ValueType::String), Int(0), String(sz)
         {}
         /// <summary>Create from string value</summary>
         explicit ParameterValue(int val) : Type(ValueType::Int), Int(val)
         {}

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(int,HighWord,GetHighWord);
         PROPERTY_GET(int,LowWord,GetLowWord);

         // ---------------------- ACCESSORS ------------------------			
      public:
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

         /// <summary>Get string/int value as string</summary>
         GuiString ToString() const
         {
            return Type == ValueType::String ? String : VString(L"%d", Int);
         }

         /// <summary>Compare two values</summary>
         bool operator==(const ParameterValue& v) const
         {
            return Type==v.Type && (Type==ValueType::String ? String==v.String : Int==v.Int);
         }

         /// <summary>Compare two values</summary>
         bool operator!=(const ParameterValue& v) const
         {
            return !operator==(v);
         }

		   // ----------------------- MUTATORS ------------------------
      public:
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
      public:
         ValueType Type;
         wstring   String;
         int       Int;
      };

      /// <summary>Write parameter value to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ParameterValue& val);

      /// <summary>Represents a script command parameter</summary>
      class LogicExport ScriptParameter
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptParameter() { throw NotImplementedException(HERE, L"StaticLib compiler fix"); } 
      public:
         ScriptParameter(const ScriptToken& name, const ScriptToken& val);
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

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Compare two parameters</summary>
         bool operator==(const ScriptParameter& r) const
         {
            return Type==r.Type && Value==r.Value; // && Syntax==r.Syntax && Text==r.Text && Token==r.Token;
         }
		   // ----------------------- MUTATORS ------------------------
      public:
         void  Generate(ScriptFile& script, UINT jumpDestination, bool commented);
         void  Translate(ScriptFile& script);

      public:
         ScriptParameter& operator=(const ScriptParameter& r) const
         {
            throw NotImplementedException(HERE, L"StaticLib compiler fix");
         }

		   // -------------------- REPRESENTATION ---------------------
      public:
         DataType        Type;
         ParameterValue  Value;
         ParameterSyntax Syntax;
         wstring         Text;
         ScriptToken     ArgName,
                         Token;
      };

      /// <summary>Write script parameter to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptParameter& p);

      /// <summary>Vector of script parameters</summary>
      class LogicExport ParameterArray : public vector<ScriptParameter> 
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
