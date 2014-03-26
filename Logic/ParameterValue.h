#pragma once

namespace Logic
{
   namespace Scripts
   {
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

   }
}

using namespace Logic::Scripts;
