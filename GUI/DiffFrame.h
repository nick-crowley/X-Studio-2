#pragma once
#include "afxcmn.h"
#include "FrameWndBase.h"
#include "LanguageFrame.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Splits the diff fame into two columns</summary>
   class DiffSplitter : public DualColumnSplitter
   {
      // --------------------- CONSTRUCTION ----------------------

      // ------------------------ STATIC -------------------------
      DECLARE_MESSAGE_MAP()

      // ----------------------- MUTATORS ------------------------
   public:
      void RecalcLayout() override;
   };

   /// <summary>Child frame</summary>
   class DiffFrame : public FrameWndBase
   {
	   // --------------------- CONSTRUCTION ----------------------
   protected:
	   DiffFrame();           // protected constructor used by dynamic creation
   public:
	   virtual ~DiffFrame();

      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(DiffFrame)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      
      // ----------------------- MUTATORS ------------------------
   protected:
      BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;

      // -------------------- REPRESENTATION ---------------------
   protected:
	   DiffSplitter Splitter;
   };



NAMESPACE_END2(GUI,Views)

