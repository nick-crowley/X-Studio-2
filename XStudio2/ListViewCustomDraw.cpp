#include "stdafx.h"
#include "ListViewCustomDraw.h"
#include "Helpers.h"

namespace GUI
{
   namespace Utils
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      ListViewCustomDraw::ListViewCustomDraw(CListCtrl& ctrl) 
         : CustomDrawImpl<NMLVCUSTOMDRAW>((UINT)DrawCycle::Paint|(UINT)DrawCycle::Items), ListView(ctrl)
      {
         // Set default highlight colours
         ActiveHighlight = GetSysColor(COLOR_HIGHLIGHT);
         InactiveHighlight = GetSysColor(COLOR_GRAYTEXT);
      }


      ListViewCustomDraw::~ListViewCustomDraw()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------


      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Custom draws an entire listview item</summary>
      /// <param name="pDraw">Draw/Item data</param>
      /// <param name="stage">Draw stage.</param>
      /// <returns></returns>
      bool  ListViewCustomDraw::onDrawItem(NMLVCUSTOMDRAW* pDraw, Stage stage)
      {
         try
         {
            // DEBUG
            //Console << "onDrawItem item=" << (int)pDraw->nmcd.dwItemSpec << " Stage=" << GetString(stage) << " state=" << pDraw->nmcd.uItemState << ENDL;
      
            // PostPaint: Do nothing
            if (stage == Stage::PostPaint)
               return true;

            // Init
            CDC dc;
            dc.Attach(pDraw->nmcd.hdc);

            // Get item data
            ItemData item((int)pDraw->nmcd.dwItemSpec, 0);
            ListView.GetItemRect(item.Index, item.Rect, LVIR_BOUNDS);
            item.Selected = ListView.GetItemState(item.Index, LVIS_SELECTED) != 0;
            item.Focused  = ListView.GetItemState(item.Index, LVIS_FOCUSED) != 0;

            // Draw background
            bool ListHasFocus = (::GetFocus() == ListView.GetSafeHwnd());
            auto backColour   = (item.Selected ? (ListHasFocus ? ActiveHighlight : InactiveHighlight) : ListView.GetBkColor());
            dc.FillSolidRect(item.Rect, backColour);

            // BugFix: check for header ctrl
            if (!ListView.GetHeaderCtrl())
               throw GenericException(HERE, L"Listview has no header ctrl");

            // Draw sub items:
            for (int count = ListView.GetHeaderCtrl()->GetItemCount(); item.SubItem < count; ++item.SubItem)
            {
               // Get sub-item rectangle
               ListView.GetSubItemRect(item.Index, item.SubItem, LVIR_LABEL, item.Rect);

               // Set/Invert text colour
               dc.SetTextColor(GetSysColor(item.Selected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

               // Allow derived-class to do drawing
               onDrawSubItem(&dc, item);
            }

            // Draw Focus rect:
            if (ListHasFocus && item.Focused)
            {
               ListView.GetItemRect(item.Index, item.Rect, LVIR_BOUNDS);
               dc.DrawFocusRect(item.Rect);
            }

            // Cleanup
            dc.Detach();
            return true;
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e);
            return false;
         }
      }

      /// <summary>Custom draws a single sub-item</summary>
      /// <param name="dc">Device context</param>
      /// <param name="item">Item data.</param>
      void  ListViewCustomDraw::onDrawSubItem(CDC* dc, ItemData& item)
      {
         // Get item data
         LVItem data(item.Index, item.SubItem, 512, LVIF_IMAGE | LVIF_TEXT);
         ListView.GetItem(&data);

         // Draw Icon:
         CImageList* imgList = ListView.GetImageList(LVSIL_SMALL);
         if (item.SubItem == 0 && imgList)
         {
            CRect icon;

            // Get icon rectangle
            ListView.GetSubItemRect(item.Index, 0, LVIR_ICON, icon);
            
            // Draw transparent
            if (!imgList->Draw(dc, data.iImage, icon.TopLeft(), ILD_TRANSPARENT))
               throw Win32Exception(HERE, GuiString(L"Unable to draw icon for item %d", item.Index));

            // Create gap betwen icon/label
            item.Rect.left += 3;
         }

         // Draw Text:
         dc->DrawText(data.pszText, item.Rect, DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS|DT_SINGLELINE);
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
