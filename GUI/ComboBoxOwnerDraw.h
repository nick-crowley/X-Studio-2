#pragma once
#include "OwnerDrawImpl.h"

namespace GUI
{
   namespace Utils
   {
   
      /// <summary></summary>
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
      public:
         // ----------------------- MUTATORS ------------------------
      public:
         virtual void DrawItem() override;

         // -------------------- REPRESENTATION ---------------------
      protected:
         bool        EditMode;
         CComboBox*  ComboBox;
      };
   
   }
}

using namespace GUI::Utils;
