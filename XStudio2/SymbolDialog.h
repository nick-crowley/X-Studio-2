#pragma once
#include "Logic/CommandNode.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class SymbolDialog : public CDialog
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_SYMBOL };
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      SymbolDialog(const wstring& symbol, SymbolType type, CWnd* parent);    
      virtual ~SymbolDialog();
       
      // ------------------------ STATIC -------------------------
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void AdjustLayout();
      void DoDataExchange(CDataExchange* pDX) override;
	  
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      wstring  RenameText;

   protected:
      wstring     SymbolText,
                  SymbolName;
      SymbolType  Type;
   };
   

NAMESPACE_END2(GUI,Windows)
