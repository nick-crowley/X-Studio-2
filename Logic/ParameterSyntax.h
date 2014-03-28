#pragma once

#include "ParameterTypes.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Get parameter type string</summary>
      LogicExport GuiString GetString(ParameterType t);

      /// <summary>Defines the purpose of the parameter</summary>
      enum class ParameterUsage { None, PageID, StringID, ScriptName };

      /// <summary></summary>
      class LogicExport ParameterSyntax
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
         };

         // --------------------- CONSTRUCTION ----------------------
#ifdef LOGIC_COMPILER_FIX
      public:
         ParameterSyntax() : Type((ParameterType)0), DisplayIndex(0), PhysicalIndex(0), Ordinal(0), Usage((ParameterUsage)0)
            { throw AlgorithmException(HERE, L"LogicLibrary compiler-fix code executed"); }
#endif
      private:
         ParameterSyntax(ParameterType t, UINT physical, UINT display);
      public:
         ParameterSyntax(Declaration& d);
         virtual ~ParameterSyntax();

         // ----------------------- STATIC --------------------------

         static bool  Verify(ParameterType pt, DataType dt);

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

         /// <summary>Verifies a data-type against the syntax.</summary>
         bool Verify(DataType t) const;

         /// <summary>Compare two syntax</summary>
         bool operator==(const ParameterSyntax& r) const
         {
            return Type==r.Type && Usage==r.Usage && DisplayIndex==r.DisplayIndex && PhysicalIndex==r.PhysicalIndex && Ordinal==r.Ordinal;
         }

		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         static const ParameterSyntax  InfixParameter;
         static const ParameterSyntax  ExpressionParameter;
         static const ParameterSyntax  LabelNameParameter; 
         static const ParameterSyntax  LabelNumberParameter;
         static const ParameterSyntax  MacroParameter;
         static const ParameterSyntax  StructuralCount; 
         static const ParameterSyntax  VArgParameter;
         static const ParameterSyntax  UnrecognisedCommand;

      public:
         const ParameterType  Type;
         const UINT           DisplayIndex,
                              PhysicalIndex,
                              Ordinal;
         const ParameterUsage Usage;
      };

      /// <summary>Write parameter syntax to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ParameterSyntax& ps);

      /// <summary>Vector of parameter syntax</summary>
      typedef vector<ParameterSyntax>  ParamSyntaxArray;
      
   }
}

using namespace Logic::Scripts;
