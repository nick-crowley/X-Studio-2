#include "stdafx.h"
#include "ComboBoxOwnerDraw.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an owner draw implementation for combo-boxes</summary>
      /// <param name="d">Owner Draw data</param>
      /// <param name="c">ComboBox window</param>
      /// <exception cref="Logic::ArgumentNullException">ComboBox window is null</exception>
      ComboBoxOwnerDraw::ComboBoxOwnerDraw(DRAWITEMSTRUCT& d, CComboBox* c)
         : OwnerDrawImpl(d, c),
           EditMode((d.itemState & ODS_COMBOBOXEDIT) != 0),
           ComboBox(c)
      {
      }


      ComboBoxOwnerDraw::~ComboBoxOwnerDraw()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Draws an item</summary>
      /// <exception cref="Logic::InvalidOperationException">Missing window styles</exception>
      void ComboBoxOwnerDraw::DrawItem()
      {
         CString txt;

         // Ensure ComboBox holds strings
         if (!HasStrings())
            throw InvalidOperationException(HERE, L"Cannot owner-draw ComboBox without CBS_HASSTRINGS");

         // BACKGROUND
         DrawBackground(Bounds, COLOR_WINDOW);

         // Shrink drawing area to create border
         Bounds.DeflateRect(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));
      
         // Get item/edit text
         if (EditMode)
            ComboBox->GetWindowTextW(txt);
         else
            ComboBox->GetLBText(Index, txt);
         
         // Draw text
         DrawText((LPCWSTR)txt, Bounds, DT_LEFT, COLOR_WINDOWTEXT);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Determines whether comboBox holds strings.</summary>
      /// <returns></returns>
      bool  ComboBoxOwnerDraw::HasStrings() const
      {
         return (ComboBox->GetStyle() & CBS_HASSTRINGS) != 0;
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
