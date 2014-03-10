#pragma once
#include "ScriptEdit.h"
#include "DiffDocument.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>Difference highlighting control</summary>
   class DiffEdit : public ScriptEdit
   {

      // ------------------------ TYPES --------------------------
   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      DiffEdit();
      virtual ~DiffEdit();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(DiffEdit)
      DECLARE_MESSAGE_MAP()


      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   
      // ----------------------- MUTATORS ------------------------
   public:
      void   Initialize(DiffDocument* diff, ScriptDocument* doc);
      //void   SetPlainText(const wstring& txt);
      //void   SetRtf(const string& rtf) override;

   protected:
      void   UpdateHighlighting(int first, int last) override;
      
      /*afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
      afx_msg void OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult) override;
      afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);*/
	  
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      DiffDocument*  DiffDocument;
};
   

NAMESPACE_END2(GUI,Controls)
