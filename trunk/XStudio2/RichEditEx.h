#pragma once
#include <richole.h>
#include <Richedit.h>
#include <tom.h> 
#include "Helpers.h"
#include "ScriptEditTooltip.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Get undo operation name</summary>
   const wchar* GetString(UNDONAMEID id);

   /// <summary>Script editing control</summary>
   class RichEditEx : public CRichEditCtrl
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>RichEdit COM interface</summary>
      _COM_SMARTPTR_TYPEDEF(IRichEditOle, IID_IRichEditOle);

      /// <summary>TOM COM interface</summary>
      typedef TOM::ITextDocumentPtr  TextDocument;
      typedef TOM::ITextRangePtr     TextRange;

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

      // --------------------- CONSTRUCTION ----------------------
   public:
      RichEditEx();
      virtual ~RichEditEx();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(RichEditEx)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      bool      FindNext(UINT start, MatchData& m) const;
      wstring   GetAllText() const;
      int       GetCaretIndex() const;
      POINT     GetCaretLocation() const;
      int       GetLineLength(int line = -1) const;
      wstring   GetLineText(int line) const;
      GuiString GetLineTextEx(int line) const;
      LineArray GetLines() const;
      CHARRANGE GetSelection() const;
      bool      HasSelection() const;
      int       LineLength(int nChar = -1) const;
      void      PasteFormat(UINT nClipFormat);

   protected:
      CPoint     GetCursorLocation() const;
      int        GetLineEnd(int line = -1) const;
      int        GetLineStart(int line = -1) const;
      CPoint     GetScrollCoordinates() const;
      void       GroupUndo(bool start);
      
      // ----------------------- MUTATORS ------------------------
   public:
      bool   Replace(MatchData& m);
      bool   EnsureVisible(int line);
      void   Initialize(COLORREF bk);
      BOOL   PreTranslateMessage(MSG* pMsg) override;
      void   SetRtf(const string& rtf);
      void   SuspendUndo(bool suspend);

   protected:
      void   FormatToken(UINT offset, const TokenBase& t, CharFormat& cf);
      void   FreezeWindow(bool freeze, bool invalidate = true);
      void   SelectLine(int line = -1);
      void   SetScrollCoordinates(const CPoint& pt);
      void   UpdateHighlighting(int first, int last);
      
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      afx_msg void OnProtectedMessage(NMHDR *pNMHDR, LRESULT *pResult);
      handler void OnRequestTooltip(ScriptEditTooltip::TooltipData* data);
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      afx_msg void OnTextChange();
      
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      TextDocument       TomDocument;
      ScriptEditTooltip  Tooltip;
      
   private:
      TooltipEvent::Handler fnShowTooltip;
      DisplayState          PrevState;
};
   

NAMESPACE_END2(GUI,Controls)
