#pragma once
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
      RefactorDialog(SymbolList matches);
      virtual ~RefactorDialog();
       
      // ------------------------ STATIC -------------------------
      //DECLARE_DYNCREATE(RefactorDialog)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      BOOL OnInitDialog() override;

      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
      void DoDataExchange(CDataExchange* pDX) override;
	  
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      SymbolList    Accepted;

   protected:
      vector<Symbol> AllSymbols;
      CCheckListBox  List;
      virtual void OnOK();
   };
   

NAMESPACE_END2(GUI,Windows)
