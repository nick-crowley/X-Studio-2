#pragma once
#include <Richedit.h>
#include "Logic/ScriptParser.h"
#include "SuggestionList.h"

/// <summary>Forward declaration</summary>
NAMESPACE_BEGIN(GUI)
   class ScriptDocument;
NAMESPACE_END(GUI)

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary>Character format helper</summary>
   struct CHARFORMAT3 : public CHARFORMAT2
   {
   public:
      BYTE bUnderlineColor;   // Undocumented index into RTF colour table
   };

   /// <summary>Character format helper</summary>
   class CharFormat : public CHARFORMAT3
   {
   public:
      CharFormat() { Clear(); }
      CharFormat(DWORD mask, DWORD effects) 
      {
         Clear();
         dwMask = mask;
         dwEffects = effects;
      }

      /// <summary>Clear formatting</summary>
      void  Clear()
      {
         ZeroMemory((CHARFORMAT3*)this, sizeof(CHARFORMAT2));
         cbSize = sizeof(CHARFORMAT2);
      }
   };

   /// <summary>Script editing control</summary>
   class ScriptEdit : public CRichEditCtrl
   {
      friend class SuggestionList;

      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Records state of text selection, view position, event mask</summary>
      class DisplayState
      {
      public:
         DisplayState() : Selection({0,0}), EventMask(NULL)
         {}
         DisplayState(CHARRANGE sel, DWORD mask, CPoint pos) : Selection(sel), EventMask(mask), ScrollPos(pos)
         {}
         CHARRANGE Selection;
         DWORD     EventMask;
         CPoint    ScrollPos;
      };

      /// <summary>Defines whether suggestions visible</summary>
      enum class InputState : UINT { Normal, Suggestions };

      // --------------------- CONSTRUCTION ----------------------
   public:
      ScriptEdit();
      virtual ~ScriptEdit();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNAMIC(ScriptEdit)
   protected:
      DECLARE_MESSAGE_MAP()

      static const wchar*  GetString(Suggestion& s);
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif  
   public:
      int       GetCaretIndex() const;
      int       GetLineLength(int line = -1) const;
      wstring   GetLineText(int line) const;
      CHARRANGE GetSelection() const;
      int       LineLength(int nChar = -1) const;

   protected:
      CPoint     GetScrollCoordinates() const;
      bool       HasDocument() const;
      Suggestion IdentifySuggestion(wchar ch) const;
      bool       MatchSuggestionType(Compiler::TokenType t) const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  SetDocument(ScriptDocument* doc);
      void  SetRtf(const string& rtf);

   protected:
      void   CloseSuggestions();
      void   FormatToken(UINT offset, const TokenBase& t, CharFormat& cf);
      void   FreezeWindow(bool freeze);
      void   InsertSuggestion();
      void   SetScrollCoordinates(const CPoint& pt);
      void   SetCompilerTimer(bool set);
      void   ShowSuggestions();
      void   UpdateSuggestions();
      
      void         OnBackgroundCompile();
      afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      afx_msg void OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnTextChange();
      afx_msg void OnTimer(UINT_PTR nIDEvent);
	  
      // -------------------- REPRESENTATION ---------------------
   private:
      DisplayState   PrevState;
      InputState     State;
      Suggestion     SuggestionType;
      SuggestionList SuggestionsList;
      ScriptDocument* Document;
      
   public:
      
      
   };
   

NAMESPACE_END2(GUI,Controls)
