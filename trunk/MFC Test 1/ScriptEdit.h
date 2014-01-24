#pragma once
#include <Richedit.h>
#include "Logic/ScriptParser.h"
#include "SuggestionList.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class ScriptEdit : public CRichEditCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
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

      enum class InputState { Normal, Suggestions };
	  
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
      wstring GetLineText(int line) const;
      CHARRANGE GetSelection() const;

   protected:
      bool IsSuggestionInitiator(wchar ch) const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  HighlightErrors(ScriptParser::ScriptTree& t);
      void  SetRtf(const string& rtf);

   protected:
      void   FreezeWindow(bool freeze);
      CPoint GetScrollCoordinates();
      void   SetScrollCoordinates(const CPoint& pt);
      void   SetCompilerTimer(bool set);

      // NEW:
      void CloseSuggestions();
      void ShowSuggestions();
      void UpdateSuggestions();

      afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnTextChange();
      afx_msg void OnTimer(UINT_PTR nIDEvent);
	  
      // -------------------- REPRESENTATION ---------------------
   private:
      DisplayState   PrevState;
      InputState     State;
      SuggestionList Suggestions;
      
   public:
      afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   };
   

NAMESPACE_END2(GUI,Controls)
