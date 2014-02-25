#include "stdafx.h"
#include "LanguageStringView.h"
#include "Logic/StringResolver.h"
#include "Logic/StringParser.h"
#include "RichTextRenderer.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageStringView, CListView)

   BEGIN_MESSAGE_MAP(LanguageStringView, CListView)
      ON_WM_SIZE()
      ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &LanguageStringView::OnItemStateChanged)
      ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &LanguageStringView::OnCustomDraw)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageStringView::LanguageStringView() : CustomDraw(this)
   {
   }

   LanguageStringView::~LanguageStringView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Gets the document.</summary>
   /// <returns></returns>
   LanguageDocument* LanguageStringView::GetDocument() const 
   {
#ifdef _DEBUG
	   return dynamic_cast<LanguageDocument*>(m_pDocument);
#else
      return reinterpret_cast<LanguageDocument*>(m_pDocument);
#endif
   }
   
   /// <summary>Gets the language page view</summary>
   /// <returns></returns>
   /// <exception cref="Logic::GenericException">View not found</exception>
   LanguagePageView*  LanguageStringView::GetPageView() const
   {
      // Iterate thru views
      for (POSITION pos = GetDocument()->GetFirstViewPosition(); pos != NULL; )
      {
         LanguagePageView* pView = dynamic_cast<LanguagePageView*>(GetDocument()->GetNextView(pos));
         if (pView != nullptr)
            return pView;
      }   

      throw GenericException(HERE, L"Cannot find page View");
   }

   /// <summary>Gets the currently selected string.</summary>
   /// <returns>Selected string if any, otherwise nullptr</returns>
   /// <exception cref="Logic::IndexOutOfRangeException">Selected item index is invalid</exception>
   LanguageString*   LanguageStringView::GetSelected() const
   {
      int item = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
      return item != -1 ? &GetPageView()->GetSelected()->Strings.FindByIndex(item) : nullptr;
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Arrange controls</summary>
   void  LanguageStringView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
      GetListCtrl().SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
   }
   

   /// <summary>Custom draw the strings</summary>
   /// <param name="pNMHDR">header.</param>
   /// <param name="pResult">result.</param>
   void LanguageStringView::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
   {
      NMCUSTOMDRAW* pDraw = reinterpret_cast<NMCUSTOMDRAW*>(pNMHDR);
      *pResult = CustomDraw.OnCustomDraw(pDraw);
   }

   
   /// <summary>Initialise control</summary>
   void LanguageStringView::OnInitialUpdate()
   {
      CListView::OnInitialUpdate();

      // Icons
      Images.Create(IDB_LANGUAGE_ICONS, 16, 6, RGB(255,0,255));
	   
      // Setup listView
      GetListCtrl().ModifyStyle(WS_BORDER, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
      GetListCtrl().SetView(LV_VIEW_DETAILS);
      GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      GetListCtrl().InsertColumn(1, L"Text", LVCFMT_LEFT, 240, 1);
      GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);
      GetListCtrl().SetImageList(&Images, LVSIL_SMALL);

      // Listen for PageClicked
      fnPageSelectionChanged = GetPageView()->SelectionChanged.Register(this, &LanguageStringView::onPageSelectionChanged);
   }
   
   /// <summary>Raise the STRING SELECTION CHANGED event</summary>
   /// <param name="pNMHDR">Item data</param>
   /// <param name="pResult">Notify result.</param>
   void LanguageStringView::OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LPNMLISTVIEW pItem = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
      
      // Selection (not focus) has changed
      if ((pItem->uOldState | pItem->uNewState) & LVIS_SELECTED)
      {
         // Update document
         GetDocument()->SelectedString = GetSelected();

         // Raise SELECTION CHANGED
         SelectionChanged.Raise();
      }

      *pResult = 0;
   }

   /// <summary>Populates the strings for the currently selected page</summary>
   void LanguageStringView::onPageSelectionChanged()
   {
      try
      {
         CWaitCursor c;

         // Clear prev
         GetListCtrl().DeleteAllItems();

         // Get selection, if any
         if (LanguagePage* page = GetDocument()->SelectedPage)
         {
            int item = -1;
            //Console << L"User has clicked on page: " << (page?page->ID:-1) << L" : " << (page?page->Title:L"") << ENDL;

            // Re-Populate strings
            GetListCtrl().SetRedraw(FALSE);
            for (const auto& pair : page->Strings)
            {
               const LanguageString& str = pair.second;

               // Add item. set Text
               GetListCtrl().InsertItem(++item, GuiString(L"%d", str.ID).c_str(), 2+GameVersionIndex(str.Version).Index);
               GetListCtrl().SetItemText(item, 1, str.ResolvedText.c_str());
            }
            GetListCtrl().SetRedraw(TRUE);
            GetListCtrl().UpdateWindow();
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e); 
      }
   }

   
   /// <summary>Adjusts layout</summary>
   /// <param name="nType">Type of the resize</param>
   /// <param name="cx">The new width</param>
   /// <param name="cy">The new height</param>
   void LanguageStringView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   /// <summary>Custom draws the text column</summary>
   /// <param name="dc">Device context</param>
   /// <param name="item">Item data.</param>
   void  LanguageStringView::StringCustomDraw::onDrawSubItem(CDC* dc, ItemData& item) 
   {
      try
      {
         switch (item.SubItem)
         {
         case 0:
            ListViewCustomDraw::onDrawSubItem(dc, item);
            break;

         case 1:
            auto src = ListView.GetItemText(item.Index, 1);
            RichTextRenderer::DrawLine(dc, item.Rect, StringParser((const wchar*)src).Output);
            break;
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }





NAMESPACE_END2(GUI,Views)

