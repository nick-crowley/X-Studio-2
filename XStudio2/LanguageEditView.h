#pragma once

#include "afxcmn.h"
#include "ToolBarEx.h"
#include "LanguageEdit.h"
#include "LanguageDocument.h"
#include "LanguageStringView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Language document view</summary>
   class LanguageEditView : public CFormView
   {
      // ------------------------ TYPES --------------------------
   public:
	   enum { IDD = IDR_LANGUAGEVIEW };
	  
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
      LanguageDocument*   GetDocument() const;

      // ----------------------- MUTATORS ------------------------
   public:
      

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
      afx_msg void OnQueryMode(CCmdUI* pCmd);
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      handler void onStringSelectionChanged();

      // -------------------- REPRESENTATION ---------------------
   protected:
      SelectionChangedHandler  fnStringSelectionChanged;
      LanguageEdit             RichEdit;
      ToolBarEx                ToolBar;
   };


NAMESPACE_END2(GUI,Views)

