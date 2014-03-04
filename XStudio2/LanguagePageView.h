#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "Logic/Event.h"
#include "ImageListEx.h"
#include "Clipboard.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // ----------------- EVENTS AND DELEGATES ------------------
   
   // ----------------------- CLASSES -------------------------

   /// <summary></summary>
   class LanguagePageView : public CListView
   {
      // ------------------------ TYPES --------------------------
   public:
      /// <summary>Base class for all Language document properties</summary>
      class PagePropertyBase : public LanguageDocument::PropertyBase
      {
      public:
         /// <summary>Create page property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         PagePropertyBase(LanguageDocument& doc, LanguagePage& page, wstring name, _variant_t val, wstring desc)
            : Page(page), PropertyBase(doc, name, val, desc)
         {}

      protected:
         LanguagePage&  Page;
      };

      /// <summary>Page Description property grid item</summary>
      class DescriptionProperty : public PagePropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Description property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page.</param>
         DescriptionProperty(LanguageDocument& doc, LanguagePage& page) 
            : PagePropertyBase(doc, page, L"Description", page.Description.c_str(),  L"Page description")
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update Description</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update value. Raise 'PAGE UPDATED'
            Page.Description = value;
            Document.PageUpdated.Raise();
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>page ID property grid item</summary>
      class IDProperty : public PagePropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create page ID property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page.</param>
         IDProperty(LanguageDocument& doc, LanguagePage& page)
            : PagePropertyBase(doc, page, L"ID", page.ID,  L"Page ID")
              
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures ID is between 1 and 9999</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return value.length() && value.IsNumeric()                     // Not empty
                && value.ToInt() >= 1 && value.ToInt() <= 9999             // 1 <= val <= 9999
                && (Page.ID == value.ToInt() || File.IsAvailable(value.ToInt()));   // ID is Unchanged or available 
         }

         /// <summary>Update ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Change ID
            Document.RenamePage(Page, value.ToInt());
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Page title property grid item</summary>
      class TitleProperty : public PagePropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create page title property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page.</param>
         TitleProperty(LanguageDocument& doc, LanguagePage& page) : PagePropertyBase(doc, page, L"Title", page.Title.c_str(),  L"Page title")
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update title</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update value. Raise 'PAGE UPDATED'
            Page.Title = value;
            Document.PageUpdated.Raise();
            // Modify document
            __super::OnValueChanged(value);  
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Voiced property grid item</summary>
      class VoicedProperty : public PagePropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Voiced property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page.</param>
         VoicedProperty(LanguageDocument& doc, LanguagePage& page) 
            : PagePropertyBase(doc, page, L"Voiced", page.Voiced ? L"True" : L"False",  L"Whether strings are voiced within the game")
         {
            AddOption(L"True", FALSE);
            AddOption(L"False", FALSE);
            // Strict option
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update Voiced</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update value. Raise 'PAGE UPDATED'
            Page.Voiced = (value == L"True");
            Document.PageUpdated.Raise();
            // Modify document
            __super::OnValueChanged(value);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };


      /// <summary>Base class for all Page commands</summary>
      class CommandBase : public LanguageDocument::CommandBase
      {
         // ------------------------ TYPES --------------------------
      protected:
         typedef unique_ptr<LanguagePage>  LanguagePagePtr;

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command</summary>
         /// <param name="doc">The document.</param>
         CommandBase(LanguageDocument& doc)
            : LanguageDocument::CommandBase(doc)
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      protected:
         LanguagePagePtr    Page;   // Copy of Page being operated on
      };

      /// <summary>Deletes the currently selected Page</summary>
      class DeleteSelectedPage : public CommandBase
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create delete-page command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No Page selected</exception>
         DeleteSelectedPage(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure selection exists
            if (!doc.SelectedPage)
               throw InvalidOperationException(HERE, L"No Page is selected");

            // Store copy of Page
            Page.reset(new LanguagePage(*doc.SelectedPage));
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Remove Page"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Removes the selected Page from the View and the Document</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Deleting Page: " << *Page << ENDL;

            // Remove Page
            Document.RemovePage(Page->ID);
         }

         /// <summary>Inserts the removed Page.</summary>
         /// <exception cref="Logic::ApplicationException">Page ID already in use</exception>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Undo() override
         {
            // Feedback
            Console << Cons::UserAction << "Undoing Delete Page: " << *Page << ENDL;

            // Re-insert Page
            Document.InsertPage(*Page);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };
	  
      /// <summary>Cuts the currently selected Page to the clipboard</summary>
      class CutSelectedPage : public DeleteSelectedPage
      {
         // ------------------------ TYPES --------------------------
      
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create cut-page command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No Page selected</exception>
         CutSelectedPage(LanguageDocument& doc) : DeleteSelectedPage(doc)
         {}

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Cut Page"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Removes the selected Page and copies it to the clipboard</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Cutting Page: " << *Page << ENDL;

            // Remove Page
            __super::Execute();

            // Copy to clipboard
            theClipboard.SetLanguagePage(*Page);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Copies the currently selected Page to the clipboard</summary>
      class CopySelectedPage : public CommandBase
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create copy-page command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No Page selected</exception>
         CopySelectedPage(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure selection exists
            if (!doc.SelectedPage)
               throw InvalidOperationException(HERE, L"No Page is selected");

            // Store copy of Page
            Page.reset(new LanguagePage(*doc.SelectedPage));
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Command cannot be undone.</summary>
         bool CanUndo() const override { return false; }

         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Copy Page"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Copies the selected Page</summary>
         /// <exception cref="Logic::Win32Exception">Clipboard error</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Copying Page: " << *Page << ENDL;

            // Duplicate to clipboard
            theClipboard.SetLanguagePage(*Page);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Inserts a new Page preceeding the currently selected Page</summary>
      class InsertNewPage : public CommandBase
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create insert-page command</summary>
         /// <param name="doc">The document.</param>
         InsertNewPage(LanguageDocument& doc) : CommandBase(doc)
         {
            // Define insertion point (if any)
            if (doc.SelectedPage)
               InsertAt.reset(new LanguagePage(*doc.SelectedPage));
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"New Page"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Insert a new Page using the next available ID</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Insert new Page:" << Cons::White << " insertAt=" << (InsertAt ? InsertAt->ID : -1) << ENDL;

            // Create/Insert Page
            auto page = Document.CreatePage(InsertAt.get());
            Document.InsertPage(page);

            // Store for later removal
            Page.reset( new LanguagePage(page) );
         }

         /// <summary>Inserts the removed Page.</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Undo() override
         {
            // Feedback
            Console << Cons::UserAction << "Undoing insert new Page: " << *Page << ENDL;

            // Remove created Page
            Document.RemovePage(Page->ID);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         LanguagePagePtr InsertAt;   // Optional insertion position
      };

      /// <summary>Paste Page on clipboard</summary>
      class PastePage : public CommandBase
      {
         // ------------------------ TYPES --------------------------
      
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'paste Page' command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No Page on clipboard</exception>
         /// <exception cref="Logic::Win32Exception">Clipboard error</exception>
         PastePage(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure clipboard contains Page
            if (!theClipboard.HasLanguagePage())
               throw InvalidOperationException(HERE, L"Clipboard does not contain a Page");

            // Duplicate clipboard Page
            Page.reset( new LanguagePage(theClipboard.GetLanguagePage()) );
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Paste Page"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Paste Page into the selected page</summary>
         /// <exception cref="Logic::ApplicationException">Page ID already in use</exception>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Pasting Page: " << *Page << ENDL;

            // insert Page
            Document.InsertPage(*Page);
         }

         /// <summary>Removed pasted Page.</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Undo() override
         {
            // Feedback
            Console << Cons::UserAction << "Undoing paste Page: " << *Page << ENDL;

            // Remove Page
            Document.RemovePage(Page->ID);
         }  

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      // --------------------- CONSTRUCTION ----------------------
   protected:
      LanguagePageView();    // Protected constructor used by dynamic creation
   public:
      virtual ~LanguagePageView();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguagePageView)
   protected:
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      LanguageDocument* GetDocument() const;
      LanguagePage*     GetSelected() const;
      
      // ----------------------- MUTATORS ------------------------
   public:
      void InsertPage(UINT index, LanguagePageRef page, bool display);
      void RemovePage(UINT index);
      BOOL PreTranslateMessage(MSG* pMsg) override;

   protected:
      void AdjustLayout();
      void Populate();
      void UpdatePage(UINT index, LanguagePageRef page);
	  
      handler void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      afx_msg void OnCommandEditCut()        { OnPerformCommand(ID_EDIT_CUT);        }
      afx_msg void OnCommandEditCopy()       { OnPerformCommand(ID_EDIT_COPY);       }
      afx_msg void OnCommandEditClear()      { OnPerformCommand(ID_EDIT_CLEAR);      }
      afx_msg void OnCommandEditPaste()      { OnPerformCommand(ID_EDIT_PASTE);      }
      afx_msg void OnCommandEditInsert()     { OnPerformCommand(ID_EDIT_INSERT);     }
      afx_msg void OnCommandEditSelectAll()  { OnPerformCommand(ID_EDIT_SELECT_ALL); }
      handler void OnLibraryRebuilt();
      handler void OnInitialUpdate() override;
      afx_msg void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);
      afx_msg void OnQueryMode(CCmdUI* pCmdUI);
      handler void OnPageUpdated();
      afx_msg void OnPerformCommand(UINT nID);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:      
      
   protected:
      EventHandler  fnLibraryRebuilt,
                    fnPageUpdated;
      ImageListEx   Images;
      HACCEL        Accelerators;
   };
   

NAMESPACE_END2(GUI,Views)
