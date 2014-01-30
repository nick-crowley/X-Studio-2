#pragma once
#include "afxcmn.h"
#include "Logic/ScriptToken.h"

/// <summary>Forward declaration</summary>
NAMESPACE_BEGIN2(Logic,Scripts)
   class ScriptFile;
NAMESPACE_END2(Logic,Scripts)


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
         SuggestionItem(const wstring& txt, const wstring& type, const wstring& sort) : Text(txt), Type(type), Key(sort)
         {}
         SuggestionItem(const wstring& txt, const wstring& type) : Text(txt), Type(type), Key(txt)
         {}
         wstring Text, // Item text
                 Type, // Type/Group text
                 Key;  // Sorting key
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
      BOOL Create(ScriptEdit* parent, CPoint& pt, Suggestion type, const ScriptFile* script);
      void MatchSuggestion(const ScriptToken& tok);

   protected:
      void AdjustLayout();
      void PopulateContent();
	  
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      afx_msg void OnRetrieveItem(NMHDR *pNMHDR, LRESULT *pResult);
	  
      // -------------------- REPRESENTATION ---------------------
      
   private:
      const CSize DefaultSize = CSize(300,200),
                  CommandSize = CSize(600,200);
      
      Suggestion   SuggestionType;
      ContentArray Content;
      const ScriptFile* Script;
   };
   

NAMESPACE_END2(GUI,Controls)
