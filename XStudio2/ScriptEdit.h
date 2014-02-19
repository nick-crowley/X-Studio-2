#pragma once
#include <richole.h>
#include <Richedit.h>
#include <tom.h> 
#import "Msftedit.dll" rename("FindText","FindText2") rename_namespace(_T("Tom"))

#include "Logic/ScriptParser.h"
#include "SuggestionList.h"

/// <summary>Forward declaration</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   class ScriptDocument;
NAMESPACE_END2(GUI,Documents)

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

   // <summary>Character format helper</summary>
   class ParaFormat : public PARAFORMAT
   {
   public:
      ParaFormat() { Clear(); }
      ParaFormat(DWORD mask) 
      {
         Clear();
         dwMask = mask;
      }

      /// <summary>Clear formatting</summary>
      void  Clear()
      {
         ZeroMemory((PARAFORMAT*)this, sizeof(PARAFORMAT));
         cbSize = sizeof(PARAFORMAT);
      }
   };

   /// <summary>Get undo operation name</summary>
   const wchar* GetString(UNDONAMEID id);

   /// <summary>Script editing control</summary>
   class ScriptEdit : public CRichEditCtrl
   {
      friend class SuggestionList;

      // ------------------------ TYPES --------------------------
   private:
      /// <summary>COM interface</summary>
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

      /// <summary>Defines whether suggestions visible</summary>
      enum class InputState : UINT { Normal, Suggestions };

   private:
      class LineTextIterator;

      /// <summary>Device context containing same font as ScriptEdit</summary>
      class FontDC : public CClientDC
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         FontDC(ScriptEdit* wnd) : CClientDC(wnd)
         {
            // Set font
            Font.CreatePointFont(10*10, L"Arial");
            OldFont = SelectObject(&Font);
         }
         ~FontDC()
         {
            SelectObject(OldFont);
         }
         
         // -------------------- REPRESENTATION ---------------------
      protected:
         CFont Font;
         CFont* OldFont;
      };

      /// <summary>Defines the logical co-ordinates of the line number gutter</summary>
      class GutterRect : public ClientRect
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         GutterRect(ScriptEdit* wnd) : ClientRect(wnd)
         {
            FontDC dc(wnd);
            right = dc.GetTextExtent(CString(L"0000")).cx;
         }
      };

      /// <summary>Defines the logical co-ordinates of a single line number in the gutter</summary>
      class LineRect : public GutterRect
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create a line number indicator rectangle</summary>
         /// <param name="wnd">script edit.</param>
         /// <param name="line">zero-based line number</param>
         /// <param name="height">height in pixels</param>
         LineRect(ScriptEdit* wnd, int line, int height) : GutterRect(wnd), Line(line), Height(height)
         {
            bottom = Height;
            OffsetRect(0, Line*Height);
         }

         // --------------------- PROPERTIES ------------------------
	  
         PROPERTY_GET(int,LineNumber,GetLineNumber);

         // ---------------------- ACCESSORS ------------------------	

         /// <summary>Get the 0-based line number</summary>
         int  GetLineNumber() const
         {
            return Line;
         }

         // ----------------------- MUTATORS ------------------------

         /// <summary>Advance to the next line</summary>
         void  Advance()
         {
            OffsetRect(0, Height);
            ++Line;
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         int Line;
         int Height;
      };

      /// <summary>Device context for drawing onto script edit</summary>
      class ScriptEditDC : public FontDC
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptEditDC(ScriptEdit* wnd) : FontDC(wnd)
         {
            SCROLLINFO horz, vert;
      
            // Clip to client window
            ClientRect rc(wnd);
            IntersectClipRect(rc);

            // Get scroll-bar info
            wnd->GetScrollInfo(SB_HORZ, &horz);
            wnd->GetScrollInfo(SB_VERT, &vert);

            // Define drawing origin
            horz.nPos = (horz.nTrackPos ? horz.nTrackPos : horz.nPos);
            vert.nPos = (vert.nTrackPos ? vert.nTrackPos : vert.nPos);
            SetViewportOrg(static_cast<int>(-0.5f*horz.nPos), static_cast<int>(-0.5f*vert.nPos));

            // Set colour
            SetBkColor(0);
            SetTextColor(0x00ffffff);
         }

         // --------------------- PROPERTIES ------------------------
	  
         // ---------------------- ACCESSORS ------------------------	

         /// <summary>Gets the indicator rectangle for a line</summary>
         /// <param name="line">Zero-based line number</param>
         /// <returns>Indicator rectangle in DEVICE co-ordinates</returns>
         LineRect GetLineRect(int line)
         {
            ScriptEdit* wnd = dynamic_cast<ScriptEdit*>(GetWindow());

            // Get line height
            CharFormat cf(CFM_OFFSET|CFM_SIZE, NULL);
            wnd->GetDefaultCharFormat(cf);

            // Get line rect in device co-ordinates  
            LineRect rc(wnd, line, cf.yHeight/10); 
            LPtoDP(rc);
            return rc;
         }

         /// <summary>Convert pixels to twips.</summary>
         /// <param name="pixels">pixels.</param>
         /// <param name="direction">LOGPIXELSX or LOGPIXELSY</param>
         /// <returns>Twips</returns>
         /// <exception cref="ArgumentException">Invalid direction</exception>
         int PixelsToTwips(int pixels, int direction) const
         {
            if (direction != LOGPIXELSX && direction != LOGPIXELSY)
               throw ArgumentException(HERE, L"direction", L"Direction must be LOGPIXELSX or LOGPIXELSY");

            return pixels * 1440 / GetDeviceCaps(direction);
         }

         /// <summary>Convert twips to pixels.</summary>
         /// <param name="twips">twips</param>
         /// <param name="direction">LOGPIXELSX or LOGPIXELSY</param>
         /// <returns>Pixels</returns>
         /// <exception cref="ArgumentException">Invalid direction</exception>
         int TwipsToPixels(int twips, int direction) const
         {
            if (direction != LOGPIXELSX && direction != LOGPIXELSY)
               throw ArgumentException(HERE, L"direction", L"Direction must be LOGPIXELSX or LOGPIXELSY");

            return twips * GetDeviceCaps(direction) / 1440;
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>Proxy for manipulating line text</summary>
      class LineProxy
      {
         friend class LineTextIterator;
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Creates a line proxy</summary>
         /// <param name="edit">edit.</param>
         /// <param name="line">zero-based line number</param>
         LineProxy(ScriptEdit& edit, int line) : Edit(edit), LineNumber(line)
         {}
         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
		public:
         PROPERTY_GET_SET(wstring,Text,GetText,SetText);
         PROPERTY_GET(bool,Commented,GetCommented);
         PROPERTY_GET(int,End,GetEnd);
         PROPERTY_GET(int,Length,GetLength);
         PROPERTY_GET(int,Line,GetLine);
         PROPERTY_GET(bool,NOP,GetNOP);
         PROPERTY_GET(int,Start,GetStart);

         // ---------------------- ACCESSORS ------------------------			
      public:
         
         /// <summary>Gets whether line is commented</summary>
         /// <returns></returns>
         bool GetCommented() const
         {
            auto txt = GetText();
            // Check for '*' as first character
            int pos = txt.find_first_not_of(L' ');
            return pos != wstring::npos && txt[pos] == L'*';
         }

         /// <summary>Gets the character index of the end of the line</summary>
         /// <returns></returns>
         int GetEnd() const
         {
            return Edit.GetLineEnd(LineNumber);
         }
         
         /// <summary>Gets the line length</summary>
         /// <returns></returns>
         int GetLength() const
         {
            return Edit.GetLineLength(LineNumber);
         }

         /// <summary>Gets the zero-based line number</summary>
         /// <returns></returns>
         int GetLine() const
         {
            return LineNumber;
         }

         /// <summary>Gets whether command is NOP</summary>
         /// <returns></returns>
         bool GetNOP() const
         {
            return Text.find_first_not_of(' ') == wstring::npos;
         }

         /// <summary>Gets the character index of the start of the line</summary>
         /// <returns></returns>
         int GetStart() const
         {
            return Edit.GetLineStart(LineNumber);
         }

         /// <summary>Gets the line text (Does not alter the edit selection)</summary>
         /// <returns></returns>
         wstring GetText() const
         {
            return Edit.GetLineText(LineNumber);
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Sets the line text and redraws it (Alters the edit selection)</summary>
         /// <param name="str">Text</param>
         void SetText(const wstring& str)
         {
            SetText(str, true, true);
         }

         /// <summary>Sets the text (Alters the edit selection)</summary>
         /// <param name="str">text.</param>
         /// <param name="canUndo">Whether can be undone.</param>
         /// <param name="redraw">Whether to redraw.</param>
         void SetText(const wstring& str, bool canUndo, bool redraw)
         {
            Edit.FreezeWindow(true);
            Edit.SelectLine(LineNumber);
            Edit.ReplaceSel(str.c_str(), canUndo ? TRUE : FALSE);
            Edit.FreezeWindow(false, redraw);
         }
      private:
         /// <summary>Changes the line number.</summary>
         /// <param name="line">zero-based line number</param>
         void SetLine(int line)
         {
            LineNumber = line;
         }
         // -------------------- REPRESENTATION ---------------------
      private:
         ScriptEdit& Edit;
         int         LineNumber;
      };

      /// <summary>Bi-directional line text iterator</summary>
      class LineTextIterator : public std::iterator<std::bidirectional_iterator_tag, LineProxy>
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         LineTextIterator(ScriptEdit& edit, int line) : Line(edit,line), Edit(edit), LineNumber(line)
         {}
         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      public:
         const LineProxy& operator*() const    { return Line;  }
         const LineProxy* operator->() const   { return &Line; }
         LineProxy& operator*()                { return Line;  }
         LineProxy* operator->()               { return &Line; }

         bool operator==(const LineTextIterator& r) const  { return Edit.m_hWnd==r.Edit.m_hWnd && LineNumber==r.LineNumber; }
         bool operator!=(const LineTextIterator& r) const  { return Edit.m_hWnd!=r.Edit.m_hWnd || LineNumber!=r.LineNumber; }
         bool operator<(const LineTextIterator& r) const   { return LineNumber < r.LineNumber;  }
         bool operator>(const LineTextIterator& r) const   { return LineNumber > r.LineNumber;  }
         bool operator<=(const LineTextIterator& r) const  { return LineNumber <= r.LineNumber; }
         bool operator>=(const LineTextIterator& r) const  { return LineNumber >= r.LineNumber; }
         /*distance_type operator-(const LineTextIterator& r) const  { return LineNumber - r.LineNumber; }
         distance_type operator+(const LineTextIterator& r) const  { return LineNumber + r.LineNumber; }*/

         // ----------------------- MUTATORS ------------------------
      public:
         LineTextIterator& operator++() 
         { 
            Line.SetLine(++LineNumber); 
            return *this; 
         }

         LineTextIterator& operator--()
         { 
            Line.SetLine(--LineNumber); 
            return *this; 
         }

         LineTextIterator operator++(int) 
         {
            LineTextIterator tmp(*this); 
            operator++(); 
            return tmp;
         }

         LineTextIterator operator--(int) 
         {
            LineTextIterator tmp(*this); 
            operator--(); 
            return tmp;
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         LineProxy   Line;
         ScriptEdit& Edit;
         int         LineNumber;
      };

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
      bool      FindNext(MatchData& m) const;
      wstring   GetAllText() const;
      int       GetCaretIndex() const;
      POINT     GetCaretLocation() const;
      int       GetLineLength(int line = -1) const;
      wstring   GetLineText(int line) const;
      LineArray GetLines() const;
      CHARRANGE GetSelection() const;
      bool      HasSelection() const;
      int       LineLength(int nChar = -1) const;

   protected:
      int        GetLineEnd(int line = -1) const;
      int        GetLineStart(int line = -1) const;
      CPoint     GetScrollCoordinates() const;
      bool       HasDocument() const;
      Suggestion IdentifySuggestion(wchar ch) const;
      bool       MatchSuggestionType(Compiler::TokenType t) const;
      
      // ----------------------- MUTATORS ------------------------
   public:
      bool   Replace(MatchData& m);
      void   CommentSelection();
      bool   EnsureVisible(int line);
      void   IndentSelection(bool indent);
      void   SetDocument(ScriptDocument* doc);
      void   SetRtf(const string& rtf);
      void   SuspendUndo(bool suspend);

   protected:
      LineTextIterator begin(int line = 0);
      LineTextIterator sbegin();
      LineTextIterator end();
      LineTextIterator send();

      void   CloseSuggestions();
      void   FormatToken(UINT offset, const TokenBase& t, CharFormat& cf);
      void   FreezeWindow(bool freeze, bool invalidate = true);
      void   InsertSuggestion();
      void   RefreshGutter();
      void   SelectLine(int line = -1);
      void   SetScrollCoordinates(const CPoint& pt);
      void   SetCompilerTimer(bool set);
      void   SetGutterWidth(UINT twips);
      void   ShowSuggestions();
      void   UpdateSuggestions();
      
      handler void OnBackgroundCompile();
      afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
      afx_msg void OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
      afx_msg void OnKillFocus(CWnd* pNewWnd);
      afx_msg void OnPaint();
      handler void OnTabKeyDown(bool shift);
      afx_msg void OnTextChange();
      afx_msg void OnTimer(UINT_PTR nIDEvent);
      afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      SimpleEvent     CompileComplete;

   private:
      DisplayState    PrevState;
      InputState      State;
      Suggestion      SuggestionType;
      SuggestionList  SuggestionsList;
      ScriptDocument* Document;
      Tom::ITextDocumentPtr TextDocument;
   };
   

NAMESPACE_END2(GUI,Controls)
