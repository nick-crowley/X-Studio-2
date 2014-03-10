#pragma once

#include "CustomDrawImpl.hpp"

namespace GUI
{
   namespace Utils
   {

      /// <summary>Custom draw implementation for a ListView</summary>
      class ListViewCustomDraw : public CustomDrawImpl<NMLVCUSTOMDRAW>
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Holds convenient item properties</summary>
         class ItemData
         {
         public:
            ItemData(int index, int sub) : Index(index), SubItem(sub)
            {}

            int   Index,
                  SubItem;
            bool  Focused,
                  Selected;
            CRect Rect;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         ListViewCustomDraw(CListCtrl& ctrl);
         virtual ~ListViewCustomDraw();

         NO_COPY(ListViewCustomDraw);	// No copy semantics
         NO_MOVE(ListViewCustomDraw);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      protected:
         bool          onDrawItem(NMLVCUSTOMDRAW* pDraw, Stage stage) override final;
         virtual void  onDrawSubItem(CDC* dc, ItemData& item);

         // -------------------- REPRESENTATION ---------------------

      protected:
         CListCtrl&  ListView;
      };

   }
}

using namespace GUI::Utils;
