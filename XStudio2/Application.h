#pragma once
#include "Logic/Event.h"
#include "Logic/Path.h"
#include "DocumentBase.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

/// <summary>Forward declarations</summary>
FORWARD_DECLARATION2(GUI,Documents,class DocumentBase)
FORWARD_DECLARATION2(GUI,Windows,class MainWnd)

// ------------------------ ENUMS --------------------------

/// <summary>Application state</summary>
enum class AppState  { NoGameData, GameDataPresent };

// -------------------- EVENTS & DELEGATES -----------------

/// <summary>Raise when application state changes</summary>
typedef Event<AppState>                    AppStateChangedEvent;
typedef AppStateChangedEvent::DelegatePtr  AppStateChangedHandler;

// ------------------------ CLASSES ------------------------

/// <summary>Main thread</summary>
class Application : public CWinAppEx
{
   // ------------------------ TYPES --------------------------
private:
   /// <summary>Document iterator</summary>
   class DocumentIterator : std::iterator<std::forward_iterator_tag, DocumentBase*>
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create document iterator.</summary>
      /// <param name="app">The application.</param>
      /// <param name="tmpl">First document template position.</param>
      DocumentIterator(const Application& app, POSITION tmpl) : App(app), Document(nullptr), DocumentPos(nullptr), TemplatePos(tmpl), Template(nullptr)
      {
         // Find first document
         if (TemplatePos = tmpl)
            while (TemplatePos && (Template=app.GetNextDocTemplate(TemplatePos)) )
               if (DocumentPos = Template->GetFirstDocPosition())
               {
                  Document = (DocumentBase*)Template->GetNextDoc(DocumentPos);
                  break;
               }

         // EOF: Ensure doc/template are null
         if (!Document)
            Template = nullptr;
      }
      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      const DocumentBase& operator*() const    { return *Document; }
      const DocumentBase* operator->() const   { return Document;  }
      DocumentBase& operator*()                { return *Document; }
      DocumentBase* operator->()               { return Document;  }

      // ----------------------- MUTATORS ------------------------
   public:
      DocumentIterator& operator++() 
      {
         // Find next document
         if (DocumentPos && (Document=(DocumentBase*)Template->GetNextDoc(DocumentPos)) )
            return *this;

         // End-Of-Current-Template: Find next non-empty template
         while (TemplatePos && (Template=App.GetNextDocTemplate(TemplatePos)) )
         {
            if (DocumentPos = Template->GetFirstDocPosition())
               if (Document = (DocumentBase*)Template->GetNextDoc(DocumentPos))
                  return *this;
         }

         // End-Of-All-Templates: EOF
         DocumentPos = nullptr;
         Document = nullptr;
         Template = nullptr;
         return *this; 
      }

      DocumentIterator operator++(int) 
      {
         DocumentIterator tmp(*this); 
         operator++(); 
         return tmp;
      }

      bool operator==(const DocumentIterator& r) const  { return Template==r.Template && Document==r.Document; }
      bool operator!=(const DocumentIterator& r) const  { return Template!=r.Template || Document!=r.Document; }

      // -------------------- REPRESENTATION ---------------------
   private:
      const Application&  App;
      CDocTemplate* Template;
      DocumentBase* Document;

      mutable POSITION  TemplatePos,
                        DocumentPos;
   };
	  
   // --------------------- CONSTRUCTION ----------------------
public:
	Application();

   // ------------------------ STATIC -------------------------
protected:
   DECLARE_MESSAGE_MAP()

public:
   AppStateChangedEvent     StateChanged;
   HINSTANCE                ResourceLibrary;

   // --------------------- PROPERTIES ------------------------
	
   PROPERTY_GET_SET(AppState,State,GetState,SetState);

   // ---------------------- ACCESSORS ------------------------			
public:
   DocumentIterator  begin() const;
   DocumentIterator  begin();
   DocumentIterator  end() const;
   DocumentIterator  end();

   GUI::Windows::MainWnd*  GetMainWindow() const;

   DocumentBase* GetOpenDocument(IO::Path p) const;
   DocumentList  GetOpenDocuments() const;
   GuiString     GetProfileSectionPath(const wstring& section) const;
   GuiString     GetProfileSectionPath(const wstring& section, const wstring& subsection) const;
   AppState      GetState() const;
   bool          IsDocumentOpen(IO::Path p) const;
   bool          IsDocumentOpen(DocumentBase* d) const;

   HICON     LoadIconW(UINT nResID, UINT iSize) const;
   CBitmap*  LoadBitmapW(UINT nResID, int x, int y, UINT flags) const;
   BOOL      ShowError(const GuiString& src, const exception& e, const GuiString& msg) const;
   BOOL      ShowError(const GuiString& src, const ExceptionBase& e, const GuiString& msg) const;
   BOOL      ShowError(const GuiString& src, const ExceptionBase& e) const;
   
   // ----------------------- MUTATORS ------------------------
public:
   virtual int   ExitInstance();
	virtual BOOL  InitInstance();
   void          OnPreferencesChanged();
   DocumentBase* OpenStringLibrary();
   afx_msg void  OnQueryCommand(CCmdUI* pCmdUI);
	virtual void  PreLoadState();
   void          SetState(AppState s);
   void          UpdateFonts();
   
protected:
	afx_msg void OnAppAbout();

   // -------------------- REPRESENTATION ---------------------
public:
   CFont  ToolWindowFont,
          TooltipFont;

protected:
   AppState    GameDataState;
};

extern Application theApp;
