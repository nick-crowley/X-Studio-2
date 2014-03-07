
#pragma once
#include "OutputList.h"
#include "ImageListEx.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)


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
      void ActivatePane(Operation op);
      void Create(CWnd* parent);

   protected:
	   //void AdjustHorzScroll(CListBox& wndListBox);
      void UpdateFonts();

      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      handler void onFindReplaceFeedback(const WorkerProgress& wp);
      handler void OnGameDataFeedback(const WorkerProgress& wp);
      handler void onLoadSaveFeedback(const WorkerProgress& wp);
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	   afx_msg void OnSize(UINT nType, int cx, int cy);


      // -------------------- REPRESENTATION ---------------------
   protected:
	   CMFCTabCtrl	TabCtrl;
	   OutputList  GameDataList,
	               OutputList,
	               FindList1,
                  FindList2;
      ImageListEx Images;

      FeedbackHandler fnGameDataFeedback,
                      fnFindReplaceFeedback,
                      fnLoadSaveFeedback;
   };


NAMESPACE_END2(GUI,Windows)

