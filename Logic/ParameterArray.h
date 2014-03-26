#pragma once


namespace Logic
{
   namespace Scripts
   {
      class ScriptParameter;

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
