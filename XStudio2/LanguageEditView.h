#pragma once

#include "afxcmn.h"
#include "ToolBarEx.h"
#include "LanguageEdit.h"
#include "LanguageDocument.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Language document view</summary>
   class LanguageEditView : public CFormView
   {
      // ------------------------ TYPES --------------------------
   public:
      /// <summary>Dialog ID.</summary>
	   enum { IDD = IDR_LANGUAGEVIEW };

      typedef CArray<COLORREF, COLORREF>  MFCColourArray;

      /// <summary>Base class for all button properties</summary>
      class ButtonPropertyBase : public LanguageDocument::PropertyBase
      {
      public:
         /// <summary>Create button property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">language edit.</param>
         /// <param name="button">button.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         ButtonPropertyBase(LanguageDocument& doc, LanguageEdit& edit, LanguageButton& button, wstring name, _variant_t val, wstring desc)
            : Button(button), Edit(edit), PropertyBase(doc, name, val, desc)
         {}

      protected:
         LanguageButton&  Button;
         LanguageEdit&    Edit;
      };

      /// <summary>Button ID property grid item</summary>
      class ButtonIDProperty : public ButtonPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create button ID property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="view">Edit view.</param>
         /// <param name="button">button.</param>
         ButtonIDProperty(LanguageDocument& doc, LanguageEditView& view, LanguageButton& button)
            : ButtonPropertyBase(doc, view.RichEdit, button, L"ID", button.ID.c_str(),  L"Button ID used by scripts")
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Always allow, even duplicates</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return true;
         }

         /// <summary>Update ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Change ID
            Button.ID = value;
            Edit.OnButtonChanged(Button);
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Button text property grid item</summary>
      class ButtonTextProperty : public ButtonPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create button text property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="view">Edit view.</param>
         /// <param name="button">button.</param>
         ButtonTextProperty(LanguageDocument& doc, LanguageEditView& view, LanguageButton& button)
            : ButtonPropertyBase(doc, view.RichEdit, button, L"Text", button.Text.c_str(),  L"Button display text (can include formatting tags)")
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Always allow, even duplicates</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return true;
         }

         /// <summary>Update text</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Change text
            Button.Text = value;
            Edit.OnButtonChanged(Button);
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Text colours array</summary>
      class TextColourArray : public MFCColourArray
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         TextColourArray()
         {
            Add((COLORREF)RichTextColour::Black);
            Add((COLORREF)RichTextColour::Blue);
            Add((COLORREF)RichTextColour::Cyan);
            Add((COLORREF)RichTextColour::Default);
            Add((COLORREF)RichTextColour::Green);
            Add((COLORREF)RichTextColour::Grey);
            Add((COLORREF)RichTextColour::Orange);
            Add((COLORREF)RichTextColour::Purple);
            Add((COLORREF)RichTextColour::Red);
            Add((COLORREF)RichTextColour::Silver);
            Add((COLORREF)RichTextColour::White);
            Add((COLORREF)RichTextColour::Yellow);
         }
      };

      /// <summary>Text colour dialog</summary>
      class TextColourMenu : public CMFCColorPopupMenu
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create colour menu.</summary>
         /// <param name="txt">Text colour.</param>
         /// <param name="cols">Available colours.</param>
         TextColourMenu(LanguageEdit& ctrl, COLORREF txt, MFCColourArray& cols)
            : Edit(ctrl), CMFCColorPopupMenu(nullptr, cols, txt, L"Default", nullptr, nullptr, Dummy, 6, (COLORREF)RichTextColour::Default)
         {}

         // ------------------------ STATIC -------------------------
      private:
         static CList<COLORREF,COLORREF>  Dummy;

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Display menu at position.</summary>
         /// <param name="parent">parent.</param>
         /// <returns></returns>
         BOOL  Create(CWnd* parent)
         {
            CursorPoint pt;
            return __super::Create(parent, pt.x, pt.y, nullptr, FALSE, TRUE);
         }

      protected:
         void  OnChooseItem(UINT nID) override
         {
            Console << "User chose item "<< nID << ENDL;
            /*CharFormat cf(CFM_COLOR, 0, col);
            Edit.SetSelectionCharFormat(cf);*/
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         LanguageEdit& Edit;
      };
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
	   LanguageEditView();           
   public:
	   virtual ~LanguageEditView();

      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(LanguageEditView)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      LanguageDocument*  GetDocument() const;
      LanguageEdit&      GetEdit() const;

      // ----------------------- MUTATORS ------------------------
   public:
      afx_msg void OnQueryMode(CCmdUI* pCmd);

   protected:
      void AdjustLayout();
      void DisplayProperties();
      void DoDataExchange(CDataExchange* pDX) override;   

      handler void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
      afx_msg void OnCommandEditCut()        { OnPerformCommand(ID_EDIT_CUT);        }
      afx_msg void OnCommandEditCopy()       { OnPerformCommand(ID_EDIT_COPY);       }
      afx_msg void OnCommandEditClear()      { OnPerformCommand(ID_EDIT_CLEAR);      }
      afx_msg void OnCommandEditPaste()      { OnPerformCommand(ID_EDIT_PASTE);      }
      afx_msg void OnCommandEditColour()     { OnPerformCommand(ID_EDIT_COLOUR);     }
      afx_msg void OnCommandEditSelectAll()  { OnPerformCommand(ID_EDIT_SELECT_ALL); }
      afx_msg void OnCommandEditAddButton()  { OnPerformCommand(ID_EDIT_ADD_BUTTON); }
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      handler void OnInitialUpdate() override;
      afx_msg void OnPerformCommand(UINT nID);
      afx_msg void OnQueryAlignment(CCmdUI* pCmd);
      afx_msg void OnQueryClipboard(CCmdUI* pCmd);
      afx_msg void OnQueryFormat(CCmdUI* pCmd);
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      handler void OnStringSelectionChanged();
      handler void OnStringContentChanged();
      afx_msg void OnTextSelectionChange(NMHDR* pNMHDR, LRESULT* result);
      
      // -------------------- REPRESENTATION ---------------------
   protected:
      SelectionChangedHandler  fnStringSelectionChanged;
      TextColourArray          TextColours;
      LanguageEdit             RichEdit;
      ToolBarEx                ToolBar;
   };


NAMESPACE_END2(GUI,Views)

