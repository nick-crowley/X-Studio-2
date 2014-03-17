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
      class RichTextPropertyBase : public LanguageDocument::PropertyBase
      {
      public:
         /// <summary>Create Rich-text item property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         RichTextPropertyBase(LanguageDocument& doc, LanguageEdit& edit, wstring name, _variant_t val, wstring desc)
            : Edit(edit), PropertyBase(doc, name, val, desc)
         {
            // Require 'Editor' mode
            Enable(IsEnabled() && Document.CurrentMode == EditMode::Edit ? TRUE : FALSE);
         }

      protected:
         LanguageEdit& Edit;
      };

      /// <summary>Author property grid item</summary>
      class AuthorProperty : public RichTextPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create author property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         AuthorProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichTextPropertyBase(doc, edit, L"Author", edit.Author.c_str(),  L"Author displayed when used as a message")
         {}

         // ------------------------ STATIC -------------------------
         
         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update author</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update author. (Raises 'STRING UPDATED')
            Edit.Author = value;
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Column Spacing property grid item</summary>
      class ColumnSpacingProperty : public RichTextPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Columns spacing property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         ColumnSpacingProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichTextPropertyBase(doc, edit, L"Column Spacing", edit.ColumnSpacing, L"Spacing between columns when displayed as a message")
         {}

         // ------------------------ STATIC -------------------------

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures value is numeric</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            // Reset to zero on clear
            if (value.empty())
               value = L"0";

            // Ensure: 0 <= value <= 250
            return value.length() && value.IsNumeric() 
                && value.ToInt() >= 0 && value.ToInt() <= 250;
         }

         /// <summary>Update Columns</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update Columns. (Raises 'STRING UPDATED')
            Edit.ColumnSpacing = value.ToInt();
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Column type property grid item</summary>
      class ColumnTypeProperty : public RichTextPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Column type property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         ColumnTypeProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichTextPropertyBase(doc, edit, L"Columns", GetString(edit.Content.Columns), L"Layout of columns when displayed as a message")
         {
            AddOption(GetString(ColumnType::Default), FALSE);
            AddOption(GetString(ColumnType::Single), FALSE);
            AddOption(GetString(ColumnType::Double), FALSE);
            AddOption(GetString(ColumnType::Triple), FALSE);
            AllowEdit(FALSE);
         }

         // ------------------------ STATIC -------------------------
      protected:
         /// <summary>Convert name to enum.</summary>
         /// <param name="val">The value.</param>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Unrecognised</exception>
         static ColumnType ToEnum(GuiString& val)
         {
            if (val == L"Default")
               return ColumnType::Default;
            else if (val == L"Single")
               return ColumnType::Single;
            else if (val == L"Double")
               return ColumnType::Double;
            else if (val == L"Triple")
               return ColumnType::Triple;

            throw ArgumentException(HERE, val, L"Unknown column spacing" + val);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update Columns</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update Columns. (Raises 'STRING UPDATED')
            Edit.Columns = ToEnum(value);
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Column width property grid item</summary>
      class ColumnWidthProperty : public RichTextPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Columns width property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         ColumnWidthProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichTextPropertyBase(doc, edit, L"Column Width", edit.ColumnWidth, L"Width of columns when displayed as a message")
         {}

         // ------------------------ STATIC -------------------------

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures value is numeric</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            // Reset to zero on clear
            if (value.empty())
               value = L"0";

            // Ensure: 0 <= value <= 500
            return value.length() && value.IsNumeric() 
                && value.ToInt() >= 0 && value.ToInt() <= 500;
         }

         /// <summary>Update Columns</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update Columns. (Raises 'STRING UPDATED')
            Edit.ColumnWidth = value.ToInt();
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Title property grid item</summary>
      class TitleProperty : public RichTextPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create Title property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="edit">edit.</param>
         TitleProperty(LanguageDocument& doc, LanguageEdit& edit) 
            : RichTextPropertyBase(doc, edit, L"Title", edit.Title.c_str(),  L"Title displayed when used as a message")
         {}

         // ------------------------ STATIC -------------------------
         
         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update Title</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Update Title. (Raise 'STRING UPDATED')
            Edit.Title = value;
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
      PROPERTY_GET_SET(wstring,Author,GetAuthor,SetAuthor);
      PROPERTY_GET_SET(wstring,Title,GetTitle,SetTitle);
      PROPERTY_GET_SET(ColumnType,Columns,GetColumns,SetColumns);
      PROPERTY_GET_SET(UINT,ColumnWidth,GetColumnWidth,SetColumnWidth);
      PROPERTY_GET_SET(UINT,ColumnSpacing,GetColumnSpacing,SetColumnSpacing);

      // ---------------------- ACCESSORS ------------------------			
   public:
      wstring         GetAllText() const;
      wstring         GetAuthor() const;
      LanguageButton* GetButton(CHARRANGE pos) const;
      ColumnType      GetColumns() const;
      UINT            GetColumnSpacing() const;
      UINT            GetColumnWidth() const;
      wstring         GetTitle() const;

   protected:

      // ----------------------- MUTATORS ------------------------
   public:
      void  Clear();
      void  Initialize(LanguageDocument* doc);
      void  InsertButton(const wstring& txt, const wstring& id);
      void  OnButtonChanged(LanguageButton& btn);
      void  Refresh();
      void  SetAuthor(const wstring& author);
      void  SetColumns(ColumnType t);
      void  SetColumnWidth(UINT w);
      void  SetColumnSpacing(UINT s);
      void  SetTitle(const wstring& title);
      void  ToggleFormatting(DWORD fx);

   protected:
      wstring GetSourceText();
      void    InsertPhrase(const RichPhrase& p);
      void    HighlightMatch(UINT pos, UINT length, CharFormat& cf);
      void    SaveString();
      void    SetRichText(const RichString& str);
      void    UpdateHighlighting();

      handler void  OnButtonRemoved(IOleObjectPtr obj);
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
      EventHandler      fnEditModeChanged;   // Edit mode changed
   };
  
   

NAMESPACE_END2(GUI,Controls)
