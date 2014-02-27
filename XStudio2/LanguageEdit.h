#pragma once
#include "RichEditEx.h"
#include "LanguageDocument.h"
#include "Logic/RtfStringWriter.h"
#include <regex>

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>RichEdit extension class</summary>
   class LanguageEdit : public RichEditEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum class EditMode { Source, Edit, Display };

      /// <summary>Data associated with each OLE button object</summary>
      class ButtonData
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Creates button data</summary>
         /// <param name="txt">text</param>
         /// <param name="id">identifier.</param>
         /// <param name="col">text colour</param>
         ButtonData(const wstring& txt, const wstring& id, Colour col)
            : Text(txt), ID(id), Colour(col) 
         {
            
         }

         // ------------------------ STATIC -------------------------

         /// <summary>Creates a bitmap for a button</summary>
         /// <param name="wnd">Edit window.</param>
         /// <param name="txt">text</param>
         /// <param name="id">identifier.</param>
         /// <param name="col">text colour</param>
         /// <returns>New button bitmap</returns>
         /// <exception cref="Logic::Win32Exception">Drawing error</exception>
         static HBITMAP CreateBitmap(LanguageEdit* wnd, const wstring& txt, const wstring& id, Colour col)
         {
            CClientDC dc(wnd);
            CBitmap   bmp;
            CDC       memDC;
            
            // Create memory DC
            if (!memDC.CreateCompatibleDC(&dc))
               throw Win32Exception(HERE, L"Unable to create memory DC");

            // Load bitmap
            CRect rcButton(0,0, 160,19);
            if (!bmp.LoadBitmapW(IDB_RICH_BUTTON))
               throw Win32Exception(HERE, L"Unable to load empty button image");

            // Setup DC
            auto prevBmp = memDC.SelectObject(&bmp);
            auto prevFont = memDC.SelectStockObject(ANSI_VAR_FONT);
      
            // Draw button text onto bitmap
            memDC.SetBkMode(TRANSPARENT);
            memDC.SetTextColor(RtfStringWriter::ToRGB(col));
            memDC.DrawText(txt.c_str(), txt.length(), &rcButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

            // Cleanup without destroying bitmap
            memDC.SelectObject(prevBmp);
            memDC.SelectObject(prevFont);

            // Detach/return bitmap
            return (HBITMAP)bmp.Detach();
         }

         // ---------------------- ACCESSORS ------------------------

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         Colour   Colour;   
         wstring  Text,     
                  ID;       
      };

   protected:
      /// <summary>RichEdit Callback COM interface</summary>
      _COM_SMARTPTR_TYPEDEF(IRichEditOleCallback, IID_IRichEditOleCallback);

      /// <summary>Default character formatting</summary>
      class DefaultCharFormat : public CharFormat
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         DefaultCharFormat() : CharFormat(CFM_FACE|CFM_COLOR|CFM_PROTECTED|CFM_SIZE, CFE_PROTECTED)
         {
            crTextColor = RGB(255,255,255);
            yHeight = 10*20;
            StringCchCopy(szFaceName, LF_FACESIZE, L"Arial");
         }
      };

      /// <summary>Ole Callback interface</summary>
      class EditCallback : public IRichEditOleCallback
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         EditCallback(LanguageEdit* edit);
         ~EditCallback();

         // --------------------- PROPERTIES ------------------------
      public:
      // IUnknown
         STDMETHOD(QueryInterface)(REFIID  iid, void**  pInterface);
         STDMETHOD_(ULONG,AddRef)();
         STDMETHOD_(ULONG,Release)();

      // IRichEditOleCallback
         STDMETHOD(GetContextMenu)(WORD  iSelectionType, IOleObject*  piObject, CHARRANGE*  pSelection, HMENU*  pOutput);
         STDMETHOD(DeleteObject)(IOleObject* pOleObject);
         STDMETHOD(QueryAcceptData)(IDataObject* pDataObject, CLIPFORMAT* lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
	      STDMETHOD(QueryInsertObject)(CLSID* pCLSID, IStorage* pStorage, LONG cp);

      // Not Implemented
         STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode)                                                                            { return E_NOTIMPL; };
	      STDMETHOD(GetClipboardData)(CHARRANGE* lpchrg, DWORD reco, IDataObject** ppDataObject)                                      { return E_NOTIMPL; };
         STDMETHOD(GetDragDropEffect)(BOOL fDrag, DWORD grfKeyState, DWORD* pdwEffect)                                               { return E_NOTIMPL; };
         STDMETHOD(GetInPlaceContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)    { return E_NOTIMPL; };
         STDMETHOD(GetNewStorage)(IStorage** ppStorage)                                                                              { return E_NOTIMPL; };
	      STDMETHOD(ShowContainerUI)(BOOL fShow)                                                                                      { return E_NOTIMPL; };
   
         // -------------------- REPRESENTATION ---------------------
      protected:
         INT           RefCount;
         LanguageEdit* Edit;
      };

      /// <summary>Short name for OLE callback smart-pointer</summary>
      typedef IRichEditOleCallbackPtr  EditCallbackPtr;

      // --------------------- CONSTRUCTION ----------------------
   public:
      LanguageEdit();
      virtual ~LanguageEdit();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(LanguageEdit)
      DECLARE_MESSAGE_MAP()

   protected:
      const static wregex   MatchComplexTag,
                            MatchColourCode,
                            MatchSimpleTag,
                            MatchSubString,
                            MatchVariable;

      static const COLORREF  MessageBackground = RGB(22,31,46);

      // --------------------- PROPERTIES ------------------------
   public:
      //PROPERTY_GET_SET(EditMode,EditMode,GetEditMode,SetEditMode);

      // ---------------------- ACCESSORS ------------------------			
   public:
      wstring   GetAllText() const;
      EditMode  GetEditMode() const;

   protected:

      // ----------------------- MUTATORS ------------------------
   public:
      void  Clear();
      void  Initialize(LanguageDocument* doc);
      void  InsertButton(const wstring& txt, const wstring& id, Colour col);
      void  SetEditMode(EditMode m);
      void  Refresh();

   protected:
      wstring GetSourceText();
      void    HighlightMatch(UINT pos, UINT length, CharFormat& cf);
      void    UpdateHighlighting();

      handler void  OnButtonRemoved(IOleObjectPtr obj);
      virtual void  OnRequestTooltip(CustomTooltip::TooltipData* data);
      virtual void  OnTextChange();
      
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      EditMode          Mode;
      LanguageDocument* Document;
      EditCallbackPtr   Callback;
};
   

NAMESPACE_END2(GUI,Controls)
