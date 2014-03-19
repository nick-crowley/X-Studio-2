#pragma once
#include "HeadingStatic.h"
#include "TitleStatic.h"
#include "BitmapStatic.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)
   
   /// <summary>Base class for all dialogs</summary>
   class DialogBase : public CDialogEx
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      DialogBase();
      DialogBase(UINT dlg, CWnd* parent = nullptr, UINT bitmap = 0, bool alpha = false); 
      virtual ~DialogBase();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(DialogBase)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;

   protected:
      void AdjustLayout();
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	  
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      afx_msg BOOL   OnEraseBkgnd(CDC* pDC);
	   afx_msg void   OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      BitmapStatic   Image;
      TitleStatic    Title;
      HeadingStatic  Heading1,
                     Heading2,
                     Heading3;
   };
   

NAMESPACE_END(GUI)
