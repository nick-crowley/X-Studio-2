
#pragma once
#include "Logic/WorkerFeedback.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   
   /// <summary>Output window list</summary>
   class COutputList : public CListCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      COutputList();
	   virtual ~COutputList();

      // ------------------------ STATIC -------------------------
   protected:
	   DECLARE_MESSAGE_MAP()

      // ----------------------- MUTATORS ------------------------
   protected:
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	   afx_msg void OnEditCopy();
	   afx_msg void OnEditClear();
	   afx_msg void OnViewOutput();
   public:
      afx_msg void OnSize(UINT nType, int cx, int cy);
   };



   /// <summary>Dockable output window</summary>
   class COutputWnd : public CDockablePane
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      COutputWnd();
      virtual ~COutputWnd();
      
      // ------------------------ STATIC -------------------------
   protected:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   public:
      void UpdateFonts();

   protected:
	   void AdjustHorzScroll(CListBox& wndListBox);
      void onGameDataFeedback(const WorkerProgress& wp);

	   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnSize(UINT nType, int cx, int cy);


      // -------------------- REPRESENTATION ---------------------
   protected:
	   CMFCTabCtrl	m_wndTabs;

	   COutputList m_wndOutputBuild;
	   COutputList m_wndOutputDebug;
	   COutputList m_wndOutputFind;
      CImageList  Images;

      FeedbackHandler fnGameDataFeedback;
   };


NAMESPACE_END(GUI)

