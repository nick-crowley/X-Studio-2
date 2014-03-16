#include "stdafx.h"
#include "LanguageStringView.h"
#include "LanguageEditView.h"
#include "PropertiesWnd.h"
#include "Logic/StringResolver.h"
#include "Logic/RichStringParser.h"
#include "RichTextRenderer.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageStringView, CListView)

   BEGIN_MESSAGE_MAP(LanguageStringView, CListView)
      ON_WM_CONTEXTMENU()
      ON_WM_SIZE()
      ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &LanguageStringView::OnItemStateChanged)
      ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &LanguageStringView::OnCustomDraw)
      ON_COMMAND(ID_EDIT_CUT, &LanguageStringView::OnCommandEditCut)
      ON_COMMAND(ID_EDIT_COPY, &LanguageStringView::OnCommandEditCopy)
      ON_COMMAND(ID_EDIT_PASTE, &LanguageStringView::OnCommandEditPaste)
      ON_COMMAND(ID_EDIT_CLEAR, &LanguageStringView::OnCommandEditClear)
      ON_COMMAND(ID_EDIT_INSERT, &LanguageStringView::OnCommandEditInsert)
      ON_COMMAND(ID_EDIT_SELECT_ALL, &LanguageStringView::OnCommandEditSelectAll)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &LanguageStringView::OnQueryCommand)
      // Toolbar Commands
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageStringView::OnQueryMode)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_BOLD, ID_EDIT_UNDERLINE, &LanguageStringView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_LEFT, ID_EDIT_JUSTIFY, &LanguageStringView::OnQueryCommand)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageStringView::LanguageStringView() : CustomDraw(this), Accelerators(nullptr)
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
   
   /// <summary>Inserts a string.</summary>
   /// <param name="index">The index.</param>
   /// <param name="str">The string.</param>
   /// <param name="display">Whether to ensure visible after insert</param>
   void LanguageStringView::InsertString(UINT index, LanguageString& str, bool display)
   {
      // First display: Identify colour tags 
      if (str.TagType == ColourTag::Undetermined)
         str.IdentifyColourTags();

      // Add item, resolve and set Text
      GetListCtrl().InsertItem(index, VString(L"%d", str.ID).c_str(), 2+GameVersionIndex(str.Version).Index);
      GetListCtrl().SetItemText(index, 1, str.ResolvedText.c_str());

      // Ensure visible
      if (display)
      {
         GetListCtrl().EnsureVisible(index, FALSE);
         GetListCtrl().SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
      }
   }

   /// <summary>Removes a string.</summary>
   /// <param name="index">The index.</param>
   void LanguageStringView::RemoveString(UINT index)
   {
      GetListCtrl().EnsureVisible(index, FALSE);
      GetListCtrl().DeleteItem(index);
   }

   /// <summary>Called when activate view.</summary>
   /// <param name="bActivate">activated.</param>
   /// <param name="pActivateView">The activate view.</param>
   /// <param name="pDeactiveView">The deactive view.</param>
   void LanguageStringView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
   {
      // Show properties
      if (bActivate)
         CPropertiesWnd::Connect(GetDocument(), true); 

      __super::OnActivateView(bActivate, pActivateView, pDeactiveView);
   }
   
   /// <summary>Translates custom accelerators for this window.</summary>
   /// <param name="pMsg">The MSG.</param>
   /// <returns></returns>
   BOOL LanguageStringView::PreTranslateMessage(MSG* pMsg)
   {
      if (Accelerators != nullptr && pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	      return ::TranslateAccelerator(m_hWnd, Accelerators, pMsg);

      return __super::PreTranslateMessage(pMsg);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Arrange controls</summary>
   void  LanguageStringView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
      GetListCtrl().SetColumnWidth(1, wnd.Width()-GetListCtrl().GetColumnWidth(0));  //LVSCW_AUTOSIZE_USEHEADER); 
   }
   
   /// <summary>Retrieves the language string representing the current selection.</summary>
   /// <returns>Selected string if any, otherwise nullptr</returns>
   /// <exception cref="Logic::IndexOutOfRangeException">Selected item index is invalid</exception>
   LanguageString*   LanguageStringView::GetSelected() const
   {
      int item = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
      return item != -1 ? &GetDocument()->SelectedPage->Strings.FindByIndex(item) : nullptr;
   }
   
   /// <summary>Display context menu.</summary>
   /// <param name="pWnd">The WND.</param>
   /// <param name="point">The point.</param>
   void LanguageStringView::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      SetFocus();
      theApp.GetContextMenuManager()->ShowPopupMenu(IDM_STRINGVIEW_POPUP, point.x, point.y, this, TRUE);
   }

   /// <summary>Custom draw the strings</summary>
   /// <param name="pNMHDR">header.</param>
   /// <param name="pResult">result.</param>
   void LanguageStringView::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
   {
      NMCUSTOMDRAW* pDraw = reinterpret_cast<NMCUSTOMDRAW*>(pNMHDR);
      *pResult = CustomDraw.OnCustomDraw(pDraw);
   }

   
   /// <summary>Custom draws the text column</summary>
   /// <param name="dc">Device context</param>
   /// <param name="item">Item data.</param>
   void  LanguageStringView::StringCustomDraw::onDrawSubItem(CDC* dc, ItemData& item) 
   {
      try
      {
         // ID:
         if (item.SubItem == 0)
            __super::onDrawSubItem(dc, item);
         
         // Text:
         else if (item.SubItem == 1)
         {
            auto src = ListView.GetItemText(item.Index, 1);
            auto flags = item.Selected ? RenderFlags::Selected : RenderFlags::Inverted;

            try
            {  // Parse+Draw
               RichTextRenderer::DrawLine(dc, item.Rect, RichStringParser((LPCWSTR)src).Output, flags);
            }
            catch (ExceptionBase&) {
               dc->SetTextColor((COLORREF)RichTextColour::Red);
               dc->DrawText(src, item.Rect, DT_LEFT|DT_SINGLELINE);
            }
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }


   /// <summary>Initialise control</summary>
   void LanguageStringView::OnInitialUpdate()
   {
      __super::OnInitialUpdate();

      try
      {
         // Icons
         Images.Create(IDB_LANGUAGE_ICONS, 16, 6, RGB(255,0,255));
	   
         // Setup listView
         GetListCtrl().ModifyStyle(WS_BORDER, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
         GetListCtrl().SetView(LV_VIEW_DETAILS);
         GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
         GetListCtrl().InsertColumn(1, L"Text", LVCFMT_LEFT, 240, 1);
         GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);
         GetListCtrl().SetImageList(&Images, LVSIL_SMALL);

         // Custom accelerators
         Accelerators = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_STRINGVIEW));

         // Listen for PAGE CHANGED + TEXT CHANGED
         fnPageSelectionChanged = GetDocument()->PageSelectionChanged.Register(this, &LanguageStringView::OnPageSelectionChanged);
         fnStringUpdated = GetDocument()->StringUpdated.Register(this, &LanguageStringView::OnStringUpdated);
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }
   
   /// <summary>Raise the STRING SELECTION CHANGED event</summary>
   /// <param name="pNMHDR">Item data</param>
   /// <param name="pResult">Notify result.</param>
   void LanguageStringView::OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LPNMLISTVIEW pItem = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
      
      try
      {
         // Selection (not focus) has changed
         if ((pItem->uOldState | pItem->uNewState) & LVIS_SELECTED)
            // Update document
            GetDocument()->SelectedString = GetSelected();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      *pResult = 0;
   }

   /// <summary>Populates the strings for the currently selected page</summary>
   void LanguageStringView::OnPageSelectionChanged()
   {
      try
      {
         CWaitCursor c;

         // Clear items + selection
         GetListCtrl().DeleteAllItems();
         GetDocument()->SelectedString = nullptr;

         // Get selection, if any
         if (GetDocument()->SelectedPage)
         {
            int item = 0;

            // Freeze
            GetListCtrl().SetRedraw(FALSE);

            // Re-Populate strings
            for (auto& pair : GetDocument()->SelectedPage->Strings)
               InsertString(item++, pair.second, false);
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e); 
      }

      // Unfreeze
      GetListCtrl().SetRedraw(TRUE);
      GetListCtrl().UpdateWindow();
   }


   /// <summary>Performs a menu command</summary>
   /// <param name="nID">Command identifier.</param>
   void LanguageStringView::OnPerformCommand(UINT nID)
   {
      try 
      {
         switch (nID)
         {
         // Cut/Copy/Paste/Delete
         case ID_EDIT_CUT:    GetDocument()->Execute(new CutSelectedString(*GetDocument()));       break;
         case ID_EDIT_COPY:   GetDocument()->Execute(new CopySelectedString(*GetDocument()));      break;
         case ID_EDIT_PASTE:  GetDocument()->Execute(new PasteString(*GetDocument()));             break;
         case ID_EDIT_CLEAR:  GetDocument()->Execute(new DeleteSelectedString(*GetDocument()));    break;
         case ID_EDIT_INSERT: GetDocument()->Execute(new InsertNewString(*GetDocument()));         break;

         // Select All
         //case ID_EDIT_SELECT_ALL:  GetListCtrl().SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);   break;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }
   

   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void LanguageStringView::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;

      // Query relevant states
      bool selection = GetDocument()->SelectedString != nullptr,
           library   = GetDocument()->Virtual,
           hasFocus  = ::GetFocus() == GetListCtrl();

      // Disable-all if tool window is focused
      if (hasFocus)
         switch (pCmdUI->m_nID)
         {
         // Non-modifying
         case ID_EDIT_COPY:        state = selection;    break;
         case ID_EDIT_SELECT_ALL:  state = true;         break;
         case ID_EDIT_FIND:        state = false;        break;

         // Modifying
         case ID_EDIT_CLEAR:  
         case ID_EDIT_CUT:         state = !library && selection;                            break;
         case ID_EDIT_PASTE:       state = !library && theClipboard.HasLanguageString();     break;
         case ID_EDIT_INSERT:      state = !library;                                         break;    

         // Formatting: Unchecked/Disabled
         case ID_EDIT_BOLD:       
         case ID_EDIT_ITALIC:     
         case ID_EDIT_UNDERLINE:  
         // Alignment: Unchecked/Disabled
         case ID_EDIT_LEFT:       
         case ID_EDIT_RIGHT:      
         case ID_EDIT_CENTRE:     
         case ID_EDIT_JUSTIFY:    
            break;
         }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }

   /// <summary>Queries the state of the editor mode toolbar commands.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void LanguageStringView::OnQueryMode(CCmdUI* pCmdUI)
   {
      // Enable mode switch, Delegate to Edit View
      GetDocument()->GetView<LanguageEditView>()->OnQueryMode(pCmdUI);
   }


   /// <summary>Adjusts layout</summary>
   /// <param name="nType">Type of the resize</param>
   /// <param name="cx">The new width</param>
   /// <param name="cy">The new height</param>
   void LanguageStringView::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   /// <summary>Refreshes the currently selected item</summary>
   void LanguageStringView::OnStringUpdated()
   {
      try 
      {  
         REQUIRED(GetDocument()->SelectedString);  

         // Update string
         UpdateString(GetListCtrl().GetNextItem(-1, LVNI_SELECTED), *GetDocument()->SelectedString);
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e);
      }
   }
   
   /// <summary>Updates a string.</summary>
   /// <param name="index">The index.</param>
   /// <param name="str">String data.</param>
   void LanguageStringView::UpdateString(UINT index, LanguageStringRef str)
   {
      // Validate index
      if (index >= (UINT)GetListCtrl().GetItemCount())
         throw IndexOutOfRangeException(HERE, index, GetListCtrl().GetItemCount());

      // Define 1st sub-item
      LVItem item(index, VString(L"%d", str.ID), NULL, LVIF_TEXT|LVIF_IMAGE);
      item.iImage = GameVersionIndex(str.Version).Index + 2;

      // Set ID/Icon/Text
      GetListCtrl().SetItem(&item);
      GetListCtrl().SetItemText(index, 1, str.ResolvedText.c_str());
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   

NAMESPACE_END2(GUI,Views)


