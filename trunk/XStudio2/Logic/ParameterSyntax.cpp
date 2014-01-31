#include "stdafx.h"
#include "ParameterSyntax.h"
#include "StringLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Expression parameter syntax</summary>
      const ParameterSyntax  ParameterSyntax::ExpressionParameter(ParameterType::EXPRESSION, 1, 1);

      /// <summary>Variable argument script call argument syntax</summary>
      const ParameterSyntax  ParameterSyntax::ScriptCallArgument(ParameterType::PARAMETER, 3, 3);

      /// <summary>Goto/gosub label name syntax</summary>
      const ParameterSyntax  ParameterSyntax::LabelDeclaration(ParameterType::LABEL_NAME, 0, 0);
      
      // -------------------------------- CONSTRUCTION --------------------------------
      
      /// <summary>Private ctor used by sentinel values</summary>
      /// <param name="t">Parameter type</param>
      /// <param name="physical">physical index</param>
      /// <param name="display">display index</param>
      ParameterSyntax::ParameterSyntax(ParameterType t, UINT physical, UINT display)
         : Type(t), PhysicalIndex(physical), DisplayIndex(display), Ordinal(0), Usage(ParameterUsage::None)
      {
      }

      /// <summary>Create syntax from a declaration</summary>
      /// <param name="d">The declaration</param>
      ParameterSyntax::ParameterSyntax(Declaration& d)
         : Type(d.Type), PhysicalIndex(d.PhysicalIndex), DisplayIndex(d.DisplayIndex), Ordinal(d.Ordinal), Usage(d.Usage)
      {
      }

      ParameterSyntax::~ParameterSyntax()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get parameter type string</summary>
      GuiString  GetString(ParameterType t)
      {
         return StringLib.Contains(KnownPage::PARAMETER_TYPES, (UINT)t) ? StringLib.Find(KnownPage::PARAMETER_TYPES, (UINT)t).Text 
                                                                        : GuiString(L"Missing (%d)", (UINT)t);
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
