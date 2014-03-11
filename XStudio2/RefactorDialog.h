#pragma once
#include "ScriptDocument.h"
#include "RefactorEdit.h"
#include "Logic/CommandNode.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class RefactorDialog : public CDialog
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_REFACTOR };
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      RefactorDialog(ScriptDocument& doc, const wstring& text, SymbolList matches, CWnd* parent);
      virtual ~RefactorDialog();
       
      // ------------------------ STATIC -------------------------
      //DECLARE_DYNCREATE(RefactorDialog)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void AdjustLayout();
      void DoDataExchange(CDataExchange* pDX) override;
	  
      afx_msg void OnCheckChanged();
      afx_msg void OnSelectionChanged();
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      SymbolList    Accepted;

   protected:
      vector<Symbol>  AllSymbols;
      ScriptDocument& Document;
      wstring         DocumentText;

      CCheckListBox  List;
      RefactorEdit   RichEdit;
   };
   

NAMESPACE_END2(GUI,Windows)
