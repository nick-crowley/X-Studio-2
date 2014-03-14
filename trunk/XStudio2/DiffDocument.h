#pragma once
#include "DocumentBase.h"
#include "PropertySource.h"
#include "ScriptDocument.h"
#include "Logic/dtl/dtl.hpp"

/// <summary>Forward declaration</summary>
FORWARD_DECLARATION2(GUI,Views,class DiffView)

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ------------------------- ENUMS -------------------------
   
   /// <summary>Identifies each difference view</summary>
   enum DiffViewType : UINT  { Original = 0, Alternate = 1 };

   /// <summary>Toggle a diff view type</summary>
   DiffViewType operator!(const DiffViewType& t);

   // ----------------- EVENTS AND DELEGATES ------------------

   // ------------------------ CLASSES ------------------------

   class DiffDocument;

   /// <summary>Diff document template</summary>
   class DiffDocTemplate : public CMultiDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      DiffDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(DiffDocTemplate)

      // ---------------------- ACCESSORS ------------------------	
   public:
      virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
      DiffDocument* OpenDocumentFile(ScriptDocument& doc, const wstring& alternate);
   };

   /// <summary>Phrase that was added or removed</summary>
   class DiffPhrase
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create empty phrase.</summary>
      /// <param name="type">type.</param>
      /// <param name="start">start index.</param>
      DiffPhrase(const dtl::edit_t type, UINT start) : Type(type), Start(start)
      {
         // DEBUG:
         /*auto sz = type==dtl::SES_ADD    ? L"ADD" 
                 : type==dtl::SES_DELETE ? L"DELETE" 
                 :                         L"COMMON";

         Console << sz << " start=" << Start << ENDL;*/
      }

      // ---------------------- ACCESSORS ------------------------	

      /// <summary>Gets the start char index.</summary>
      /// <returns></returns>
      UINT  GetStart() const
      {
         return Start;
      }

      /// <summary>Gets the end char index.</summary>
      /// <returns></returns>
      UINT  GetEnd() const
      {
         return Start + Text.length();
      }

      // -------------------- REPRESENTATION ---------------------
   public:
      wstring     Text;
      dtl::edit_t Type;
      UINT        Start;
   };

   /// <summary>Script diff document</summary>
   class DiffDocument : public DocumentBase, public PropertySource
   {
	   // ------------------------ TYPES --------------------------
   public:

      // --------------------- CONSTRUCTION ----------------------
   public:
      DiffDocument();
	   virtual ~DiffDocument();

      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(DiffDocument)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
   public:

      // ---------------------- ACCESSORS ------------------------			
   public:
      GUI::Views::DiffView* GetView(UINT index) const;
      GUI::Views::DiffView* GetView(DiffViewType t) const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  OnDisplayProperties(CMFCPropertyGridCtrl& grid) override;
      void  OnCloseDocument() override;
      void  OnDocumentEvent(DocumentEvent deEvent) override;
      BOOL  OnOpenDocument(ScriptDocument& doc, const wstring& alternate);
      BOOL  OnSaveDocument(LPCTSTR szPathName) override;

   protected:
      wstring ConvertLineBreaks(wstring str);
      void  GeneratePhrases(dtl::Ses<wchar>& ses);
      void  OnPerformCommand(UINT nID);
      void  OnQueryCommand(CCmdUI* pCmdUI);

      // -------------------- REPRESENTATION ---------------------
   public:
      wstring         Original, 
                      Alternate;
      ScriptDocument* Source;

      list<DiffPhrase> Phrases;

};


NAMESPACE_END2(GUI,Documents)

