#pragma once
#include "afxcmn.h"
#include "Logic/ScriptToken.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   class ScriptEdit;

   /// <summary>Defines suggestion categories</summary>
   enum class Suggestion : UINT { None, Variable, Command, GameObject, ScriptObject, Label };

   /// <summary></summary>
   class SuggestionList : public CListCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Item in the list</summary>
      struct SuggestionItem
      {
      public:
         SuggestionItem(const wstring& txt, const wstring& type) : Text(txt), Type(type)
         {}
         const wstring Text, Type;
      };

      /// <summary>Vector of items</summary>
      typedef vector<SuggestionItem> ContentArray;

      // --------------------- CONSTRUCTION ----------------------
   public:
      SuggestionList();
      virtual ~SuggestionList();
       
      // ------------------------ STATIC -------------------------
   
      DECLARE_DYNAMIC(SuggestionList)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif  
      ScriptEdit* GetParent() const;
      wstring GetSelected() const;
   
      // ----------------------- MUTATORS ------------------------
   public:
      BOOL Create(ScriptEdit* parent, CPoint& pt, Suggestion type);
      void MatchSuggestion(const ScriptToken& tok);

   protected:
      void AdjustLayout();
      void PopulateContent();
	  
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      afx_msg void OnRetrieveItem(NMHDR *pNMHDR, LRESULT *pResult);
	  
      // -------------------- REPRESENTATION ---------------------
      
   private:
      const CSize DefaultSize = CSize(300,200);
      
      Suggestion   SuggestionType;
      ContentArray Content;
   };
   

NAMESPACE_END2(GUI,Controls)
