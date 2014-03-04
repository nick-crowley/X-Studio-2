#pragma once
#include "RichEditEx.h"
#include "LanguageDocument.h"
#include "RichTextRenderer.h"
#include <regex>

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   class LanguageEdit;

   /// <summary>Data associated with each OLE button object</summary>
   class LanguageButton
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Creates button data</summary>
      /// <param name="txt">text</param>
      /// <param name="id">identifier.</param>
      LanguageButton(const wstring& txt, const wstring& id) : Text(txt), ID(id)
      {}

      // ------------------------ STATIC -------------------------

      static HBITMAP CreateBitmap(LanguageEdit* wnd, const wstring& txt, const wstring& id);

      // ---------------------- ACCESSORS ------------------------

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   public:
      wstring  ID,
               Text;
   };

   /// <summary>Shared pointer to language button data</summary>
   typedef shared_ptr<LanguageButton>  LanguageButtonPtr;

   /// <summary>Write Button data to the console</summary>
   ConsoleWnd& operator<<(ConsoleWnd& c, const LanguageButton& b);


   /// <summary>RichEdit extension class</summary>
   class LanguageEdit : public RichEditEx
   {
      // ------------------------ TYPES --------------------------
   public:
      /// <summary>Base class for all Rich-text string properties</summary>
      class RichStringPropertyBase : public LanguageDocument::LanguagePropertyBase
      {
      public:
         /// <summary>Create Rich-text string property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="str">string.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         RichStringPropertyBase(LanguageDocument& doc, RichString& str, wstring name, _variant_t val, wstring desc)
            : String(str), LanguagePropertyBase(doc, name, val, desc)
         {
            // Require 'Editor' mode
            Enable(Document.CurrentMode == EditMode::Edit ? TRUE : FALSE);
         }

      protected:
         RichString& String;
      };

      /// <summary>Author property grid item</summary>
      class AuthorProperty : public RichStringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create author property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         AuthorProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichStringPropertyBase(doc, edit.Content, L"Author", edit.Content.Author.c_str(),  L"Author displayed when used as a message")
         {}

         // ------------------------ STATIC -------------------------
         
         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update author</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update author. Raise 'STRING CONTENT CHANGED'
            String.Author = value;
            Document.StringContentChanged.Raise();

            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Title property grid item</summary>
      class TitleProperty : public RichStringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Title property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         TitleProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichStringPropertyBase(doc, edit.Content, L"Title", edit.Content.Title.c_str(),  L"Title displayed when used as a message")
         {}

         // ------------------------ STATIC -------------------------
         
         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update Title</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update Title. Raise 'STRING CONTENT CHANGED'
            String.Title = value;
            Document.StringContentChanged.Raise();

            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
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

      /// <summary>List of language button data</summary>
      typedef list<LanguageButtonPtr>  ButtonDataList;

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
      

      // ---------------------- ACCESSORS ------------------------			
   public:
      wstring         GetAllText() const;
      LanguageButton* GetButton(CHARRANGE pos) const;
      
   protected:

      // ----------------------- MUTATORS ------------------------
   public:
      void  Clear();
      void  Initialize(LanguageDocument* doc);
      void  InsertButton(const wstring& txt, const wstring& id);
      void  OnButtonChanged(LanguageButton& btn);
      void  Refresh();
      void  ToggleFormatting(DWORD fx);

   protected:
      wstring GetSourceText();
      void    InsertPhrase(const RichPhrase& p);
      void    HighlightMatch(UINT pos, UINT length, CharFormat& cf);
      void    SetRichText(const RichString& str);
      void    UpdateHighlighting();

      handler void  OnButtonRemoved(IOleObjectPtr obj);
      handler void  OnContentChanged();
      handler void  OnEditModeChanged();
      handler void  OnRequestTooltip(CustomTooltip::TooltipData* data) override;
      handler void  OnTextChange() override;
      
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      LanguageDocument* Document;            // Document 
      EditCallbackPtr   Callback;            // Object removal notification callback
      RichString        Content;             // [EDITOR] Current content 
      ButtonDataList    ActiveData;          // Temporary storage for button data
      EventHandler      fnContentChanged,    // String Text/Properties changed
                        fnEditModeChanged;   // Edit mode changed
   };
  
   

NAMESPACE_END2(GUI,Controls)
