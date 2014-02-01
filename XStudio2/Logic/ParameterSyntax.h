#pragma once
#include "Common.h"
#include "ParameterTypes.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Get parameter type string</summary>
      GuiString GetString(ParameterType t);

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
      private:
         ParameterSyntax(ParameterType t, UINT physical, UINT display);
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
            case ParameterType::RETURN_VALUE_IF:   
               return true;

            default: 
               return false;
            }
         }

		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         static const ParameterSyntax  ExpressionParameter,
                                       LabelDeclaration,
                                       ScriptCallArgument,
                                       UnrecognisedCommand;

      public:
         const ParameterType  Type;
         const UINT           DisplayIndex,
                              PhysicalIndex,
                              Ordinal;
         const ParameterUsage Usage;
      };

      /// <summary>Vector of parameter syntax</summary>
      typedef vector<ParameterSyntax>  ParamSyntaxArray;
      
   }
}

using namespace Logic::Scripts;
