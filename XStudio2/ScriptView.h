
// ScriptView.h : interface of the ScriptView class
//

#pragma once

#include "afxcmn.h"
#include "Logic/Common.h"
#include "Logic/ScriptObjectLibrary.h"
#include "ScriptDocument.h"
#include "ScriptEdit.h"
#include "CommandTooltip.h"
#include "PropertiesWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // ------------------ EVENTS AND DELEGATES -----------------

   typedef Event<POINT>                  CaretMovedEvent;
   typedef CaretMovedEvent::DelegatePtr  CaretMovedHandler;

   // ------------------------ CLASSES ------------------------

   /// <summary>Script view</summary>
   class ScriptView : public CFormView, public PropertySource
   {
      // ------------------------ TYPES --------------------------
   public:
	   enum{ IDD = IDR_SCRIPTVIEW };

   private:
      /// <summary>Script argument property grid item</summary>
      class ArgumentProperty : public CMFCPropertyGridProperty
      {
      public:
         /// <summary>Create argument property.</summary>
         /// <param name="arg">Argument.</param>
         ArgumentProperty(const ScriptVariable& arg)
            : CMFCPropertyGridProperty(arg.Name.c_str(), _variant_t(GetString(arg.ValueType).c_str()), arg.Description.c_str())
         {
            // Add all parameter types as options
            for (const ScriptObject& obj : ScriptObjectLib)
               if (obj.Group == ScriptObjectGroup::ParameterType && !obj.IsHidden())
                  AddOption(obj.Text.c_str(), FALSE);
         }
      };

      /// <summary>Command ID property grid item</summary>
      class CommandIDProperty : public CMFCPropertyGridProperty
      {
      public:
         /// <summary>Create command ID property.</summary>
         /// <param name="name">Command name.</param>
         CommandIDProperty(const wstring& name)
            : CMFCPropertyGridProperty(L"Command", name.c_str(), L"ID of ship/station command implemented by the script")
         {
            // Add all command IDs as options
            for (const ScriptObject& obj : ScriptObjectLib)
               if (obj.Group == ScriptObjectGroup::ObjectCommand)
                  AddOption(obj.Text.c_str(), FALSE);
         }
      };

      /// <summary>Game version property grid item</summary>
      class GameVersionProperty : public CMFCPropertyGridProperty
      {
      public:
         /// <summary>Create game version property.</summary>
         /// <param name="ver">Version.</param>
         GameVersionProperty(GameVersion ver)
            : CMFCPropertyGridProperty(L"Game Required", VersionString(ver).c_str(),  L"Minimum version of game required")
         {
            // Add all versions as options
            AddOption(VersionString(GameVersion::AlbionPrelude).c_str(), FALSE);
            AddOption(VersionString(GameVersion::TerranConflict).c_str(), FALSE);
            AddOption(VersionString(GameVersion::Reunion).c_str(), FALSE);
            AddOption(VersionString(GameVersion::Threat).c_str(), FALSE);
         }
      };

      /// <summary>Egosoft signature grid item</summary>
      class SignedProperty : public CMFCPropertyGridProperty
      {
      public:
         /// <summary>Create read-only Egosoft signature property.</summary>
         /// <param name="val">Signed property.</param>
         SignedProperty(bool val)
            : CMFCPropertyGridProperty(L"Signed", (_variant_t)val, L"Whether script has been signed by Egosoft")
         {
            Enable(FALSE);
         }
      };
      
      // --------------------- CONSTRUCTION ----------------------
   protected: 
	   ScriptView();
	public:
      virtual ~ScriptView();   
	  
      // ------------------------ STATIC -------------------------
   public:
      static CaretMovedEvent   CaretMoved;
   
      DECLARE_DYNCREATE(ScriptView)
      DECLARE_MESSAGE_MAP()
   
      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      bool            FindNext(UINT start, MatchData& m) const;
	   ScriptDocument* GetDocument() const;
      ScriptFile&     GetScript() const;
      CHARRANGE       GetSelection() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void          OnDisplayProperties(CMFCPropertyGridCtrl& grid) override;
      bool          Replace(MatchData& m);
      void          SetSelection(CHARRANGE rng);
      virtual BOOL  PreTranslateMessage(MSG* pMsg);

   protected:
      void AdjustLayout();
      void PopulateVariables();
      void PopulateScope();
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
      void UpdateScope();
      

      virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
      afx_msg void OnDestroy();
      afx_msg void OnEditComment();
      afx_msg void OnEditIndent();
      afx_msg void OnEditOutdent();
      afx_msg void OnEditUndo();
      afx_msg void OnEditRedo();
      afx_msg void OnClipboardCopy();
      afx_msg void OnClipboardCut();
      afx_msg void OnClipboardPaste();
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      handler void OnCompileComplete();
	   virtual void OnInitialUpdate(); 
      afx_msg void OnQueryClipboardCut(CCmdUI *pCmdUI);
      afx_msg void OnQueryClipboardPaste(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditComment(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditIndent(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditOutdent(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditUndo(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditRedo(CCmdUI *pCmdUI);
      afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
      afx_msg void OnScopeSelectionChange();
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      afx_msg void OnSize(UINT nType, int cx, int cy);	  
      afx_msg void OnTextSelectionChange(NMHDR* pNMHDR, LRESULT* result);

      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptEdit RichEdit;
      CComboBox  ScopeCombo;
      CComboBox  VariablesCombo;
      CommandTooltip  ToolTip;

   protected:
      EventHandler fnCompileComplete;

};


   
NAMESPACE_END2(GUI,Views)

