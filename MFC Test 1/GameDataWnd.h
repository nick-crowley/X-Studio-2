#pragma once
#include "afxdockablepane.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)
   
   /// <summary></summary>
   class CGameDataWnd : public CDockablePane
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      CGameDataWnd();
      virtual ~CGameDataWnd();
       
      // ------------------------ STATIC -------------------------
   protected:
      DECLARE_MESSAGE_MAP()
      
      // --------------------- PROPERTIES ------------------------
	   
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   protected:
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	   afx_msg void OnPaint();
	   afx_msg void OnSetFocus(CWnd* pOldWnd);

   private:
      void  AdjustLayout();

      // -------------------- REPRESENTATION ---------------------

   protected:
      CListCtrl  ListView;
	   CImageList Images;
   };
   
NAMESPACE_END(GUI)

