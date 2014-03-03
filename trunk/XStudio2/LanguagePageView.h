#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "Logic/Event.h"
#include "ImageListEx.h"

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
      class PagePropertyBase : public LanguageDocument::LanguagePropertyBase
      {
      public:
         /// <summary>Create page property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         PagePropertyBase(LanguageDocument& doc, LanguagePage& page, wstring name, _variant_t val, wstring desc)
            : Page(page), LanguagePropertyBase(doc, name, val, desc)
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
            Page.Description = value;
            __super::OnValueChanged(value);    // Modify document
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
                && (Page.ID == value.ToInt() || !File.Pages.Contains(value.ToInt()));   // ID is Unchanged or available 
         }

         /// <summary>Update ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // TODO: Change ID
            //File.ID = value.ToInt();
            __super::OnValueChanged(value);    // Modify document
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
            Page.Title = value;
            __super::OnValueChanged(value);    // Modify document
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
            Page.Voiced = (value == L"True");
            __super::OnValueChanged(value);    // Modify document
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
   protected:
      void AdjustLayout();
      void InsertItem(UINT index, LanguagePageRef page);
      void Populate();
	  
      handler void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
      afx_msg void OnCommandEditCut()        { OnPerformCommand(ID_EDIT_CUT);        }
      afx_msg void OnCommandEditCopy()       { OnPerformCommand(ID_EDIT_COPY);       }
      afx_msg void OnCommandEditClear()      { OnPerformCommand(ID_EDIT_CLEAR);      }
      afx_msg void OnCommandEditPaste()      { OnPerformCommand(ID_EDIT_PASTE);      }
      afx_msg void OnCommandEditSelectAll()  { OnPerformCommand(ID_EDIT_SELECT_ALL); }
      handler void OnLibraryRebuilt();
      handler void OnInitialUpdate() override;
      afx_msg void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);
      afx_msg void OnQueryMode(CCmdUI* pCmdUI);
      afx_msg void OnPerformCommand(UINT nID);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:      
      
   protected:
      EventHandler  fnLibraryRebuilt;
      ImageListEx   Images;
   };
   

NAMESPACE_END2(GUI,Views)
