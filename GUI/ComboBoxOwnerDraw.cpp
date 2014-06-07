#include "stdafx.h"
#include "ComboBoxOwnerDraw.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an owner draw implementation</summary>
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

      void ComboBoxOwnerDraw::DrawItem() 
      {
         // Ensure ComboBox holds strings
         if ((ComboBox->GetStyle() & CBS_HASSTRINGS) == 0)
            return;

         // EDIT MODE
         /*if (EditMode)
            return;*/

         //CString  str;
         //GetLBText(pData->itemID, str);
         //dc.DrawTextW(str, &pData->rcItem, DT_LEFT);

         // Prepare
         /*ComboItemData* item = reinterpret_cast<ComboItemData*>(pd->itemData);
         Console << "Drawing item ID: " << pd->itemID << " rcItem=" << Bounds << " data=" << item << ENDL;*/
      
         // BACKGROUND
         DrawBackground(Bounds, COLOR_WINDOW);

         // Shrink drawing area to create border
         Bounds.DeflateRect(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));
      
         // TEXT
         /*DrawText(item->Left, Bounds, DT_LEFT, COLOR_WINDOWTEXT);
         DrawText(item->Right, Bounds, DT_RIGHT, COLOR_GRAYTEXT);*/
         //CComboBox ctrl;
         //CString   txt;

         // Prepare
         //CComboBox* combo = dynamic_cast<CComboBox*>(Ctrl);
         CString txt;

         // Get item/edit text
         if (EditMode)
            ComboBox->GetWindowTextW(txt);
         else
            ComboBox->GetLBText(Index, txt);
         
         // Draw text
         DrawText((LPCWSTR)txt, Bounds, DT_LEFT, COLOR_WINDOWTEXT);

         // TODO:  Add your code to draw the specified item
         //Console << "Drawing item #" << (UINT)pData->hwndItem << " rcItem=" << CRect(pData->rcItem) << ENDL;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
