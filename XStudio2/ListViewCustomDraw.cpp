#include "stdafx.h"
#include "ListViewCustomDraw.h"
#include "Helpers.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      ListViewCustomDraw::ListViewCustomDraw(CWnd* wnd, CListCtrl& ctrl) 
         : CustomDrawImpl<NMLVCUSTOMDRAW>(wnd, (UINT)DrawCycle::Paint|(UINT)DrawCycle::Items), ListView(ctrl)
      {
      }


      ListViewCustomDraw::~ListViewCustomDraw()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------


      // ------------------------------ PROTECTED METHODS -----------------------------

      bool  ListViewCustomDraw::onDrawItem(NMLVCUSTOMDRAW* pDraw, Stage stage) 
      {
         // DEBUG
         Console << "onDrawItem item=" << (int)pDraw->nmcd.dwItemSpec << " Stage=" << GetString(stage) << " state=" << pDraw->nmcd.uItemState << ENDL;
      
         // Init
         CDC dc;
         dc.Attach(pDraw->nmcd.hdc);

         // Get item data
         ItemData item((int)pDraw->nmcd.dwItemSpec, 0);
         ListView.GetItemRect(item.Index, item.Rect, LVIR_BOUNDS);
         item.State = ListView.GetItemState(pDraw->nmcd.dwItemSpec, LVIS_SELECTED|LVIS_FOCUSED);

         // Draw background
         COLORREF cl = (item.State & LVIS_SELECTED ? GetSysColor(COLOR_HIGHLIGHT) : ListView.GetBkColor());
         dc.FillSolidRect(item.Rect, cl);

         // Draw sub items
         int count = ListView.GetHeaderCtrl() ? ListView.GetHeaderCtrl()->GetItemCount() : 0;
         for (; item.SubItem < count; ++item.SubItem)
         {
            // Get sub-item rectangle
            ListView.GetSubItemRect(item.Index, item.SubItem, LVIR_BOUNDS, item.Rect);

            // Allow derived-class to do drawing
            if (onDrawSubItem(&dc, item))
               continue;

            // Get item data
            LVItem data(512, LVIF_IMAGE | LVIF_TEXT);
            ListView.GetItem(&data);

            // Draw Icon:
            if (item.SubItem == 0)
            {
               CSize size(item.Rect.Height(), item.Rect.Height());
               ListView.GetImageList(LVSIL_SMALL)->DrawEx(&dc, data.iImage, item.Rect.TopLeft(), size, CLR_NONE, CLR_NONE, ILD_TRANSPARENT);
            }

            // Draw Text:
            ListView.GetSubItemRect(item.Index, item.SubItem, LVIR_LABEL, item.Rect);
            dc.DrawText(data.pszText, item.Rect, DT_LEFT|DT_END_ELLIPSIS|DT_SINGLELINE);
         }

         // Cleanup
         dc.Detach();
         return true;
      }


      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
