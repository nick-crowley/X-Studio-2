#pragma once
#include "DocumentBase.h"
#include "PropertySource.h"
#include "ScriptDocument.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ------------------------- ENUMS -------------------------

   // ----------------- EVENTS AND DELEGATES ------------------

   // ------------------------ CLASSES ------------------------

   class DiffDocument;

   /// <summary>Language document template</summary>
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

   /// <summary>Language document</summary>
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
      /// <summary>Gets any language view</summary>
      /// <typeparam name="VIEW">View type</typeparam>
      /// <returns></returns>
      /// <exception cref="Logic::AlgorithmException">View not found</exception>
      //template<typename VIEW>
      //VIEW* GetView() const
      //{
      //   // Iterate thru views
      //   for (POSITION pos = GetFirstViewPosition(); pos != NULL; )
      //      if (VIEW* v = dynamic_cast<VIEW*>(GetNextView(pos)))
      //         return v;

      //   // Error: Not found
      //   throw AlgorithmException(HERE, L"Cannot find desired View");
      //}


      // ----------------------- MUTATORS ------------------------
   public:
      void  OnDisplayProperties(CMFCPropertyGridCtrl& grid) override;
      void  OnCloseDocument() override;
      void  OnDocumentEvent(DocumentEvent deEvent) override;
      BOOL  OnOpenDocument(ScriptDocument& doc, const wstring& alternate);
      BOOL  OnSaveDocument(LPCTSTR szPathName) override;

   protected:
      void  OnPerformCommand(UINT nID);
      void  OnQueryCommand(CCmdUI* pCmdUI);

      // -------------------- REPRESENTATION ---------------------
   public:
      wstring         Original, 
                      Alternate;
      ScriptDocument* Source;

};


NAMESPACE_END2(GUI,Documents)

