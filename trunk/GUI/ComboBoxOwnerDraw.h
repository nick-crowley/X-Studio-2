#pragma once
#include "OwnerDrawImpl.h"

namespace GUI
{
   namespace Utils
   {
   
      /// <summary>Implementation of Owner draw for combo-boxes</summary>
      class ComboBoxOwnerDraw : public OwnerDrawImpl
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      public:
         ComboBoxOwnerDraw(DRAWITEMSTRUCT& d, CComboBox* c);
         virtual ~ComboBoxOwnerDraw();

         NO_COPY(ComboBoxOwnerDraw);	// Uncopyable
         NO_MOVE(ComboBoxOwnerDraw);	// Unmoveable

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      protected:
         bool  HasStrings() const;

         // ----------------------- MUTATORS ------------------------
      public:
         virtual void DrawItem() override;

         // -------------------- REPRESENTATION ---------------------
      protected:
         bool        EditMode;         // Whether in edit or listbox mode
         CComboBox*  ComboBox;         // ComboBox handle
      };
   
   }
}

using namespace GUI::Utils;
