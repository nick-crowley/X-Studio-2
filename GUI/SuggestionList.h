#pragma once
#include "afxcmn.h"
#include "../Logic/ScriptFile.h"
#include "../Logic/ScriptToken.h"
#include "ListViewCustomDraw.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   class ScriptEdit;

   /// <summary>Defines suggestion categories</summary>
   enum class Suggestion : UINT { None, Variable, Command, GameObject, ScriptObject, Label };

   /// <summary>Customized listview for displaying Auto-complete suggestions in the script edit</summary>
   class SuggestionList : public CListCtrl
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Custom draw implementation</summary>
      class SuggestionCustomDraw : public ListViewCustomDraw
      {
      public:
         SuggestionCustomDraw(SuggestionList& ctrl) : ListViewCustomDraw(ctrl)
         {}

      protected:
         void  onDrawSubItem(CDC* dc, ItemData& item) override;
      };

      /// <summary>Item in the list</summary>
      struct SuggestionItem
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         SuggestionItem(const wstring& txt, const wstring& type, const wstring& sort) : Text(txt), Type(type), Key(sort)
         {}
         SuggestionItem(const wstring& txt, const wstring& type) : Text(txt), Type(type), Key(txt)
         {}

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring Text,  // Item text
                 Type,  // Type/Group text
                 Key;   // Sorting key
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

   public:
      static CSize  GetDefaultSize(Suggestion type);
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      ScriptEdit* GetParent() const;
      wstring GetSelected() const;
   
      // ----------------------- MUTATORS ------------------------
   public:
      BOOL Create(ScriptEdit* parent, CRect rc, Suggestion type, const ScriptFile* script);
      void MatchSuggestion(const ScriptToken& tok);

   protected:
      void AdjustLayout();
      void PopulateContent();
      void ShrinkToFit();
	  
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnKeyDown(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      afx_msg LRESULT OnVisibleItemsChanged(WPARAM wParam = 0, LPARAM lParam = 0);
      afx_msg void OnRetrieveItem(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnScrollBegin(NMHDR *pNMHDR, LRESULT *pResult);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      const static CSize DefaultSize,
                         CommandSize;
      
      Suggestion            SuggestionType;
      SuggestionCustomDraw  CustomDraw;
      ContentArray          Content;
      const ScriptFile*     Script;
   };
   

NAMESPACE_END2(GUI,Controls)
