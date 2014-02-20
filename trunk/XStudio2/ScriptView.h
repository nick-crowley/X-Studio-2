
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
      class ArgumentProperty : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create argument property.</summary>
         /// <param name="arg">Argument.</param>
         ArgumentProperty(ScriptVariable& arg)
            : Argument(arg), ValidatingProperty(arg.Name.c_str(), _variant_t(GetString(arg.ValueType).c_str()), arg.Description.c_str())
         {
            // Populate parameter types 
            for (const ScriptObject& obj : ScriptObjectLib)
               if (obj.Group == ScriptObjectGroup::ParameterType && !obj.IsHidden())
                  AddOption(obj.Text.c_str(), FALSE);

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update parameter type</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Argument.ValueType = (ParameterType)ScriptObjectLib.Find(value).ID;
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptVariable& Argument;
      };

      /// <summary>Command ID property grid item</summary>
      class CommandIDProperty : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command ID property.</summary>
         /// <param name="s">Script.</param>
         CommandIDProperty(ScriptFile& s)
            : Script(s), ValidatingProperty(L"Command", s.CommandName.c_str(), L"ID of ship/station command implemented by the script")
         {
            // Populate command IDs 
            for (const ScriptObject& obj : ScriptObjectLib)
               if (obj.Group == ScriptObjectGroup::ObjectCommand)
                  AddOption(obj.Text.c_str(), FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Validates and optionally modifies the value being input</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            const ScriptObject* obj = nullptr;
            int index = 0;
            
            // Command ID: Allow manually input numeric IDs
            if (value.IsNumeric())
            {
               // Replace with name if possible
               if (ScriptObjectLib.TryFind(ScriptObjectGroup::ObjectCommand, value.ToInt(), obj))
                  value = obj->Text;
            }
            // Command Name: Replace with exact match if present
            else if (TryFind(value, index))
               value = GetOption(index);

            // Always accept values
            return true;
         }

         /// <summary>Update command ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            if (value.IsNumeric())
               Script.CommandID = value.ToInt();
            else
               Script.CommandID = value;
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptFile&  Script;
      };

      /// <summary>Script description property grid item</summary>
      class DescriptionProperty : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create description property.</summary>
         /// <param name="s">Script.</param>
         DescriptionProperty(ScriptFile& s)
            : Script(s), ValidatingProperty(L"Description", s.Description.c_str(), L"Short description of functionality")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update description</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Description = value;
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptFile&  Script;
      };

      /// <summary>Game version property grid item</summary>
      class GameVersionProperty : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create game version property.</summary>
         /// <param name="s">Script.</param>
         GameVersionProperty(ScriptFile& s)
            : ValidatingProperty(L"Game Required", VersionString(s.Game).c_str(),  L"Minimum version of game required"),
              Script(s)
         {
            // Populate game versions
            AddOption(VersionString(GameVersion::Threat).c_str(), FALSE);
            AddOption(VersionString(GameVersion::Reunion).c_str(), FALSE);
            AddOption(VersionString(GameVersion::TerranConflict).c_str(), FALSE);
            AddOption(VersionString(GameVersion::AlbionPrelude).c_str(), FALSE);

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
            Script.Game = GameVersionIndex(Find(value.c_str())).Version;      // Convert zero-based index into GameVersion
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptFile&  Script;
      };

      /// <summary>Script name property grid item</summary>
      class NameProperty : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create name property.</summary>
         /// <param name="s">Script.</param>
         NameProperty(ScriptFile& s)
            : Script(s), ValidatingProperty(L"Name", s.Name.c_str(), L"How script is referenced throughout the game")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update name</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Name = value;
         }
         
         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptFile&  Script;
      };

      /// <summary>Egosoft signature grid item</summary>
      class SignedProperty : public CMFCPropertyGridProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create read-only Egosoft signature property.</summary>
         /// <param name="val">Signed property.</param>
         SignedProperty(bool val)
            : CMFCPropertyGridProperty(L"Signed", (_variant_t)val, L"Whether script has been signed by Egosoft")
         {
            Enable(FALSE);
         }
      };

      /// <summary>Script name property grid item</summary>
      class VersionProperty : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create version property.</summary>
         /// <param name="s">Script.</param>
         VersionProperty(ScriptFile& s)
            : Script(s), ValidatingProperty(L"Version", (_variant_t)s.Version, L"Current version number")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures version is numeric</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return !value.length() || (value.IsNumeric() && value.ToInt() > 0);
         }

         /// <summary>Update version</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Version = value.length() ? value.ToInt() : 0;
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptFile&  Script;
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

