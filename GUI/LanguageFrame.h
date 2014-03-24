#pragma once
#include "afxcmn.h"
#include "FrameWndBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Splits the language frame into two rows</summary>
   class DualRowSplitter : public CSplitterWnd
   {
      // --------------------- CONSTRUCTION ----------------------

      // ------------------------ STATIC -------------------------
      DECLARE_MESSAGE_MAP()

      // ----------------------- MUTATORS ------------------------
   public:
	   BOOL Create(CWnd* pParentWnd, UINT nID = AFX_IDW_PANE_FIRST);

      void RecalcLayout() override;
   };

   /// <summary>Splits the language fame into two columns</summary>
   class DualColumnSplitter : public CSplitterWnd
   {
      // --------------------- CONSTRUCTION ----------------------

      // ------------------------ STATIC -------------------------
      DECLARE_MESSAGE_MAP()

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL Create(CWnd* pParentWnd, UINT nID = AFX_IDW_PANE_FIRST);

      void RecalcLayout() override;
   };

   /// <summary>Child frame</summary>
   class LanguageFrame : public FrameWndBase
   {
	   // --------------------- CONSTRUCTION ----------------------
   protected:
	   LanguageFrame();           // protected constructor used by dynamic creation
   public:
	   virtual ~LanguageFrame();

      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(LanguageFrame)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      
      // ----------------------- MUTATORS ------------------------
   protected:
      BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;

      // -------------------- REPRESENTATION ---------------------
   protected:
	   DualColumnSplitter ColumnSplitter;
      DualRowSplitter    RowSplitter;
   };



NAMESPACE_END2(GUI,Views)

