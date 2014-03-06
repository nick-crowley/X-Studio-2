#pragma once
#include "afxdockablepane.h"
#include "ImageListEx.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary>Dockable game data window</summary>
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
   public:
      //BOOL CanBeClosed() const override { return FALSE; }
      void Create(CWnd* parent, wstring title, UINT nID, UINT nIconID);

   protected:
      virtual void Clear();
      virtual void PopulateGroupCombo() PURE;
      virtual void PopulateItems(const wstring& searchTerm, UINT selectedGroup) PURE;

      handler void OnAppStateChanged(AppState s);
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	   afx_msg void OnPaint();
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
      afx_msg void OnSearchTermChanged();
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);

   private:
      void  AdjustLayout();
      void  UpdateContent();

      // -------------------- REPRESENTATION ---------------------

   protected:
      CListCtrl   ListView;
	   ImageListEx Images;
      CComboBox   Groups;
      CEdit       Search;

      AppStateChangedHandler  fnAppStateChanged;
   };
   
NAMESPACE_END2(GUI,Windows)

