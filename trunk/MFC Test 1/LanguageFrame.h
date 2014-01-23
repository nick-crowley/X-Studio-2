#pragma once
#include "afxcmn.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Child frame</summary>
   class LanguageFrame : public CMDIChildWndEx
   {
	   // --------------------- CONSTRUCTION ----------------------
   protected:
	   LanguageFrame();           // protected constructor used by dynamic creation
   public:
	   virtual ~LanguageFrame();

      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguageFrame)
   protected:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif   

      // ----------------------- MUTATORS ------------------------
   protected:
      virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

      // -------------------- REPRESENTATION ---------------------
   protected:
	   CSplitterWnd Splitter;
   };



NAMESPACE_END2(GUI,Views)

