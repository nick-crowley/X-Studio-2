#pragma once
#include "ScriptDocument.h"
#include "DialogBase.h"
#include "../Logic/CommandNode.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class ArgumentDialog : public DialogBase
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_ARGUMENT };
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      ArgumentDialog(ScriptFile& script, CWnd* parent);
      ArgumentDialog(ScriptFile& script, const ScriptVariable& arg, CWnd* parent);
      virtual ~ArgumentDialog();
       
      // ------------------------ STATIC -------------------------
      //DECLARE_DYNCREATE(ArgumentDialog)
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
	  
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptVariable Argument;

   protected:
      ScriptFile& Script;

      wstring    Name, 
                 Description;
      CComboBox  Types;
   };
   

NAMESPACE_END2(GUI,Windows)
