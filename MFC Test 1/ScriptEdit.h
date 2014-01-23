#pragma once
#include <Richedit.h>
#include "Logic/ScriptParser.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class ScriptEdit : public CRichEditCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      ScriptEdit();
      virtual ~ScriptEdit();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNAMIC(ScriptEdit)
   protected:
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif  
      wstring GetLine(int line) const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  HighlightErrors(ScriptParser::ScriptTree& t);
      void  SetRtf(const string& rtf);

   protected:
      void  FreezeWindow(bool freeze);

      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnChange();
      afx_msg void OnTimer(UINT_PTR nIDEvent);
	  
      // -------------------- REPRESENTATION ---------------------
   private:
      CHARRANGE Selection;
      DWORD     EventMask;
      
   public:
      
   };
   

NAMESPACE_END2(GUI,Controls)
