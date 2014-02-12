#pragma once
#include "afxcmn.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Splits the language view vertically</summary>
   class LanguagePageSplitter : public CSplitterWnd
   {
   public:
      DECLARE_MESSAGE_MAP()

      /// <summary>Saves column sizes on change</summary>
      void RecalcLayout() override
      {
         CSplitterWnd::RecalcLayout();

         // Store column widths
         int width, min;
         GetColumnInfo(0, width, min);
         theApp.WriteSectionInt(L"LanguageFrame", L"PageWidth", width);
         GetColumnInfo(1, width, min);
         theApp.WriteSectionInt(L"LanguageFrame", L"StringWidth", width);
      }
   };

   /// <summary>Splits the language view horizontally</summary>
   class LanguageStringSplitter : public CSplitterWnd
   {
   public:
      DECLARE_MESSAGE_MAP()

      /// <summary>Saves row sizes on change</summary>
      void RecalcLayout() override
      {
         CSplitterWnd::RecalcLayout();

         // Store row widths
         int height, min;
         GetRowInfo(0, height, min);
         theApp.WriteSectionInt(L"LanguageFrame", L"StringHeight", height);
         GetRowInfo(1, height, min);
         theApp.WriteSectionInt(L"LanguageFrame", L"EditHeight", height);
      }
   };

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
	   LanguageStringSplitter SubSplitter;
      LanguagePageSplitter   Splitter;
   };



NAMESPACE_END2(GUI,Views)

