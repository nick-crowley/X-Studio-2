#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "ListViewCustomDraw.h"
#include "ImageListEx.h"
#include "GuiCommand.h"
#include "Clipboard.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)
   
   /// <summary></summary>
   class LanguageStringView : public CListView
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Custom draw implementation</summary>
      class StringCustomDraw : public ListViewCustomDraw
      {
      public:
         StringCustomDraw(LanguageStringView* view) : ListViewCustomDraw(view, view->GetListCtrl())
         {}

      protected:
         void  onDrawSubItem(CDC* dc, ItemData& item) override;
      };

   public:
      /// <summary>Base class for all string properties</summary>
      class StringPropertyBase : public LanguageDocument::PropertyBase
      {
      public:
         /// <summary>Create string property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="str">string.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         StringPropertyBase(LanguageDocument& doc, LanguageString& str, wstring name, _variant_t val, wstring desc)
            : String(str), PropertyBase(doc, name, val, desc)
         {}

      protected:
         LanguageString& String;
      };


      /// <summary>colour tags property grid item</summary>
      class ColourTagProperty : public StringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create string colour-tags property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="str">string.</param>
         /// <exception cref="Logic::ArgumentException">Colour tags are still undetermined</exception>
         ColourTagProperty(LanguageDocument& doc, LanguageString& str) 
            : StringPropertyBase(doc, str, L"Colour Tags", GetString(str.TagType).c_str(),  L"Determines whether to use named colour tags or escape codes")
         {
            // Require tags to be already determined
            if (String.TagType == ColourTag::Undetermined)
               throw ArgumentException(HERE, L"string", L"Colour tags are undetermined");

            // Populate
            AddOption(GetString(ColourTag::Unix).c_str());
            AddOption(GetString(ColourTag::Message).c_str());
            // Strict dropdown
            AllowEdit(FALSE);

            // Require 'Editor' mode
            Enable(Document.CurrentMode == EditMode::Edit ? TRUE : FALSE);
         }

         // ------------------------ STATIC -------------------------
      protected:
         /// <summary>Gets colour tag string.</summary>
         /// <param name="tag">The tag.</param>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Unknown tag</exception>
         static wstring  GetString(ColourTag tag)
         {
            switch (tag)
            {
            case ColourTag::Unix:          return L"Colour Codes";
            case ColourTag::Message:       return L"Message Tags";
            case ColourTag::Undetermined:  return L"Undetermined";
            }
            throw ArgumentException(HERE, L"tag", L"Unrecognised Colour tag enumeration");
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update colour tag type</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update tag-types. (Raises 'STRING UPDATED')
            String.TagType = (value == L"Colour Codes" ? ColourTag::Unix : ColourTag::Message);
            Document.StringUpdated.Raise();

            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>ID property grid item</summary>
      class IDProperty : public StringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create string ID property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page containing string.</param>
         /// <param name="string">string.</param>
         IDProperty(LanguageDocument& doc, LanguagePage& page, LanguageString& string)
            : Page(page), StringPropertyBase(doc, string, L"ID", string.ID,  L"string ID")
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures ID is unique</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return value.length() && value.IsNumeric() 
                && (String.ID == value.ToInt() || Page.IsAvailable(value.ToInt()));   // ID is Unchanged or available
         }

         /// <summary>Update ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Change ID
            Document.RenameString(String, value.ToInt());

            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         LanguagePage& Page;
      };

      /// <summary>version property grid item</summary>
      class VersionProperty : public StringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create game version property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="string">string.</param>
         VersionProperty(LanguageDocument& doc, LanguageString& str)
            : StringPropertyBase(doc, str, L"Version", VersionString(str.Version).c_str(),  L"Version of game that string originates")
         {
            // Populate game versions
            for (int i = 0; i < 4; i++)
               AddOption(VersionString(GameVersionIndex(i).Version).c_str());

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update game version</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Convert zero-based index into GameVersion.  (Raise 'STRING UPDATED')
            String.Version = GameVersionIndex(Find(value.c_str())).Version;      
            Document.StringUpdated.Raise();

            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };


      /// <summary>Base class for all string commands</summary>
      class CommandBase : public LanguageDocument::CommandBase
      {
         // ------------------------ TYPES --------------------------
      protected:
         typedef unique_ptr<LanguageString>  LanguageStringPtr;

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command</summary>
         /// <param name="view">The view.</param>
         /// <param name="doc">The document.</param>
         CommandBase(LanguageDocument& doc)
            : LanguageDocument::CommandBase(doc)
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      protected:
         LanguageStringPtr    String;   // Copy of string being operated on
      };

      /// <summary>Deletes the currently selected string</summary>
      class DeleteSelectedString : public CommandBase
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No string/page selected</exception>
         DeleteSelectedString(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure selection exists
            if (!doc.SelectedString)
               throw InvalidOperationException(HERE, L"No string is selected");
            else if (!doc.SelectedPage)
               throw InvalidOperationException(HERE, L"No page is selected");

            // Store copy of string
            String.reset(new LanguageString(*doc.SelectedString));
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Remove String"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Removes the selected string from the View and the Document</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Deleting string: " << *String << ENDL;

            // Remove string
            Document.RemoveString(String->Page, String->ID);
         }

         /// <summary>Inserts the removed string.</summary>
         /// <exception cref="Logic::ApplicationException">String ID already in use</exception>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Undo() override
         {
            // Feedback
            Console << Cons::UserAction << "Undoing Delete string: " << *String << ENDL;

            // Re-insert string
            Document.InsertString(*String);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Cuts the currently selected string to the clipboard</summary>
      class CutSelectedString : public DeleteSelectedString
      {
         // ------------------------ TYPES --------------------------
      
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No string/page selected</exception>
         CutSelectedString(LanguageDocument& doc) : DeleteSelectedString(doc)
         {}

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Cut String"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Removes the selected string and copies it to the clipboard</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Cutting string: " << *String << ENDL;

            // Remove string
            __super::Execute();

            // Copy to clipboard
            theClipboard.SetLanguageString(*String);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Copies the currently selected string to the clipboard</summary>
      class CopySelectedString : public CommandBase
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No string/page selected</exception>
         CopySelectedString(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure selection exists
            if (!doc.SelectedString)
               throw InvalidOperationException(HERE, L"No string is selected");
            else if (!doc.SelectedPage)
               throw InvalidOperationException(HERE, L"No page is selected");

            // Store copy of string
            String.reset(new LanguageString(*doc.SelectedString));
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Command cannot be undone.</summary>
         bool CanUndo() const override { return false; }

         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Copy String"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Copies the selected string</summary>
         /// <exception cref="Logic::Win32Exception">Clipboard error</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Copying string: " << *String << ENDL;

            // Duplicate to clipboard
            theClipboard.SetLanguageString(*String);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Inserts a new string preceeding the currently selected string</summary>
      class InsertNewString : public CommandBase
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No page selected</exception>
         InsertNewString(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure page is selected
            if (!doc.SelectedPage)
               throw InvalidOperationException(HERE, L"No page is selected");

            // Store PageID
            PageID = doc.SelectedPage->ID;

            // Define insertion point (if any)
            if (doc.SelectedString)
               InsertAt.reset(new LanguageString(*doc.SelectedString));
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"New String"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Insert a new string using the next available ID</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Insert new string:" << Cons::White << " insertAt=" << (InsertAt ? InsertAt->ID : -1) << ENDL;

            // Create/Insert string
            auto str = Document.CreateString(PageID, InsertAt.get());
            Document.InsertString(str);

            // Store for later removal
            String.reset( new LanguageString(str) );
         }

         /// <summary>Inserts the removed string.</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         void Undo() override
         {
            // Feedback
            Console << Cons::UserAction << "Undoing insert new string: " << *String << ENDL;

            // Remove created string
            Document.RemoveString(PageID, String->ID);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         UINT              PageID;     // Target Page 
         LanguageStringPtr InsertAt;   // Optional insertion position
      };

      /// <summary>Paste string on clipboard into the currently selected page</summary>
      class PasteString : public CommandBase
      {
         // ------------------------ TYPES --------------------------
      
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'paste string' command</summary>
         /// <param name="doc">The document.</param>
         /// <exception cref="Logic::InvalidOperationException">No string on clipboard -or- no page selected</exception>
         /// <exception cref="Logic::Win32Exception">Clipboard error</exception>
         PasteString(LanguageDocument& doc) : CommandBase(doc)
         {
            // Ensure clipboard contains string
            if (!theClipboard.HasLanguageString())
               throw InvalidOperationException(HERE, L"Clipboard does not contain a string");

            // Ensure page selected
            else if (!doc.SelectedPage)
               throw InvalidOperationException(HERE, L"No page is selected");

            // Duplicate clipboard string, Change PageID to currently selected page
            String.reset( new LanguageString(theClipboard.GetLanguageString()) );
            String->Page = doc.SelectedPage->ID;
         }

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Get name.</summary>
         wstring GetName() const override { return L"Paste String"; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Paste string into the selected page</summary>
         /// <exception cref="Logic::ApplicationException">String ID already in use</exception>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         void Execute() override
         {
            // Feedback
            Console << Cons::UserAction << "Pasting string: " << *String << ENDL;

            // insert string
            Document.InsertString(*String);
         }

         /// <summary>Removed pasted string.</summary>
         /// <exception cref="Logic::InvalidOperationException">Document is virtual</exception>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         void Undo() override
         {
            // Feedback
            Console << Cons::UserAction << "Undoing paste string: " << *String << ENDL;

            // Remove string
            Document.RemoveString(String->Page, String->ID);
         }  

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      // --------------------- CONSTRUCTION ----------------------
   protected:
      LanguageStringView();    // Protected constructor used by dynamic creation
   public:
      virtual ~LanguageStringView();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(LanguageStringView)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      LanguageDocument* GetDocument() const;

   protected:
      LanguageString*   GetSelected() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void InsertString(UINT index, LanguageString& str, bool display);
      void RemoveString(UINT index);
      BOOL PreTranslateMessage(MSG* pMsg) override;
      
      handler void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;

   protected:
      void AdjustLayout();
      void UpdateString(UINT index, LanguageStringRef str);
      
      afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      afx_msg void OnCommandEditCut()        { OnPerformCommand(ID_EDIT_CUT);        }
      afx_msg void OnCommandEditCopy()       { OnPerformCommand(ID_EDIT_COPY);       }
      afx_msg void OnCommandEditClear()      { OnPerformCommand(ID_EDIT_CLEAR);      }
      afx_msg void OnCommandEditPaste()      { OnPerformCommand(ID_EDIT_PASTE);      }
      afx_msg void OnCommandEditInsert()     { OnPerformCommand(ID_EDIT_INSERT);     }
      afx_msg void OnCommandEditSelectAll()  { OnPerformCommand(ID_EDIT_SELECT_ALL); }
      handler void OnInitialUpdate() override;
      afx_msg void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
      handler void onPageSelectionChanged();
      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);
      afx_msg void OnQueryMode(CCmdUI* pCmdUI);
      afx_msg void OnPerformCommand(UINT nID);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
      handler void OnStringUpdated();
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      
   protected:
      SelectionChangedHandler  fnPageSelectionChanged;
      EventHandler             fnStringUpdated;
      ImageListEx              Images;
      StringCustomDraw         CustomDraw;
      HACCEL                   Accelerators;
   };
   

NAMESPACE_END2(GUI,Views)
