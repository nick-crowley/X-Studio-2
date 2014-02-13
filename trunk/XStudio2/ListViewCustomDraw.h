#pragma once

#include "CustomDrawImpl.hpp"

namespace GUI
{
   namespace Utils
   {

      /// <summary></summary>
      class ListViewCustomDraw : public CustomDrawImpl<NMLVCUSTOMDRAW>
      {
         // ------------------------ TYPES --------------------------
      protected:
         class ItemData
         {
         public:
            ItemData(int index, int sub) : Index(index), SubItem(sub)
            {}

            int   Index,
                  SubItem;
            UINT  State;
            CRect Rect;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         ListViewCustomDraw(CWnd* wnd, CListCtrl& ctrl);
         virtual ~ListViewCustomDraw();

         NO_COPY(ListViewCustomDraw);	// No copy semantics
         NO_MOVE(ListViewCustomDraw);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      protected:
         bool  onDrawItem(NMLVCUSTOMDRAW* pDraw, Stage stage) override final;

         virtual bool  onDrawSubItem(CDC* dc, const ItemData& item) PURE;

         // -------------------- REPRESENTATION ---------------------

      protected:
         CListCtrl&  ListView;
      };

   }
}

using namespace GUI::Utils;
