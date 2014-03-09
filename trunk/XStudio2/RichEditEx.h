#pragma once
#include <richole.h>
#include <Richedit.h>
#include <tom.h> 
#include "Helpers.h"
#include "CustomTooltip.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Get undo operation name</summary>
   const wchar* GetString(UNDONAMEID id);

   /// <summary>RichEdit extension class</summary>
   class RichEditEx : public CRichEditCtrl
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>RichEdit COM interface</summary>
      _COM_SMARTPTR_TYPEDEF(IRichEditOle, IID_IRichEditOle);

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
   public:
      PROPERTY_GET_SET(BOOL,ReadOnly,IsReadOnly,SetReadOnly);

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
      wstring   GetRedoMenuItem() const;
      CHARRANGE GetSelection() const;
      wstring   GetUndoMenuItem() const;
      bool      HasSelection() const;
      BOOL      IsReadOnly() const;
      int       LineLength(int nChar = -1) const;

   protected:
      CPoint     GetCursorLocation() const;
      int        GetLineEnd(int line = -1) const;
      int        GetLineStart(int line = -1) const;
      CPoint    GetScrollCoordinates() const;
      void       GroupUndo(bool start);
      
      // ----------------------- MUTATORS ------------------------
   public:
      bool   Replace(MatchData& m);
      bool   EnsureVisible(int line);
      void   Initialize(COLORREF bk);
      virtual void   PasteFormat(UINT nClipFormat);
      virtual BOOL   PreTranslateMessage(MSG* pMsg) override;
      virtual void   SetRtf(const string& rtf);
      void   SelectLine(int line = -1);
      void   SuspendUndo(bool suspend);

   protected:
      void   FreezeWindow(bool freeze, bool invalidate = true);
      void   SetScrollCoordinates(const CPoint& pt);
      
      afx_msg void HScroll(UINT nSBCode, UINT nPos);
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      virtual void OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnProtectedMessage(NMHDR *pNMHDR, LRESULT *pResult);
      virtual void OnRequestTooltip(CustomTooltip::TooltipData* data);
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      virtual void OnTextChange();
      afx_msg void VScroll(UINT nSBCode, UINT nPos);
      
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      TextDocumentPtr  TextDocument;
      IRichEditOlePtr  OleDocument;
      CustomTooltip    Tooltip;
      
   private:
      TooltipEvent::Handler fnShowTooltip;
      DisplayState          PrevState;
};
   

NAMESPACE_END2(GUI,Controls)
