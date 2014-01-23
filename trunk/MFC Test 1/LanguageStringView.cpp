#include "stdafx.h"
#include "LanguageStringView.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageStringView, CListView)

   BEGIN_MESSAGE_MAP(LanguageStringView, CListView)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageStringView::LanguageStringView()
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
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
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

   
   void LanguageStringView::OnInitialUpdate()
   {
      CListView::OnInitialUpdate();

      // Setup listView
      GetListCtrl().SetView(LV_VIEW_DETAILS);
      GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      GetListCtrl().InsertColumn(1, L"Text", LVCFMT_LEFT, 240, 1);
      GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);

      // Listen for PageClicked
      fnPageSelectionChanged = GetPageView()->SelectionChanged.Register(this, &LanguageStringView::onPageSelectionChanged);
   }

   void LanguageStringView::onPageSelectionChanged()
   {
      // Clear prev
      GetListCtrl().DeleteAllItems();

      // Get selection, if any
      if (LanguagePage* page = GetPageView()->GetSelectedPage())
      {
         UINT item = 0;
         //Console << L"User has clicked on page: " << (page?page->ID:-1) << L" : " << (page?page->Title:L"") << ENDL;

         // Re-Populate strings   
         for (const auto& pair : page->Strings)
         {
            const LanguageString& str = pair.second;

            // Add item {ID,Text}
            item = GetListCtrl().InsertItem(item++, GuiString(L"%d", str.ID).c_str(), 0);
            GetListCtrl().SetItem(item, 1, LVIF_TEXT, str.Text.c_str(), 0, NULL, NULL, NULL);
         }
      }
   }

   
   void LanguageStringView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)

