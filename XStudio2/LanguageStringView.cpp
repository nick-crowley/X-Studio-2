#include "stdafx.h"
#include "LanguageStringView.h"
#include "Logic/StringResolver.h"
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

   #ifdef _DEBUG
   void LanguageStringView::AssertValid() const
   {
	   CListView::AssertValid();
   }
   
   void LanguageStringView::Dump(CDumpContext& dc) const
   {
	   CListView::Dump(dc);
   }

   LanguageDocument* LanguageStringView::GetDocument() const // non-debug version is inline
   {
	   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(LanguageDocument)));
	   return (LanguageDocument*)m_pDocument;
   }
   #endif //_DEBUG
   
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
   LanguageString*   LanguageStringView::GetSelectedString() const
   {
      int item = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
      return item != -1 ? &GetPageView()->GetSelectedPage()->Strings.FindByIndex(item) : nullptr;
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
      
      // Raise 'SELECTION CHANGED'
      SelectionChanged.Raise();

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
         if (LanguagePage* page = GetPageView()->GetSelectedPage())
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
   
   bool  LanguageStringView::StringCustomDraw::onDrawSubItem(CDC* dc, const ItemData& item)
   {
      return false;
   }





NAMESPACE_END2(GUI,Views)

