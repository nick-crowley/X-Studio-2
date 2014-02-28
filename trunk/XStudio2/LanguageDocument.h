#pragma once
#include "Logic/LanguageFile.h"
#include "DocumentBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   /// <summary>Language document template</summary>
   class LanguageDocTemplate : public CMultiDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      LanguageDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(LanguageDocTemplate)

      // ---------------------- ACCESSORS ------------------------	

      virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
   };

   /// <summary>Language document</summary>
   class LanguageDocument : public DocumentBase
   {
	   // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      LanguageDocument();
	   virtual ~LanguageDocument();

      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(LanguageDocument)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
   public:
      PROPERTY_GET_SET(LanguagePage*,SelectedPage,GetSelectedPage,SetSelectedPage);
      PROPERTY_GET_SET(LanguageString*,SelectedString,GetSelectedString,SetSelectedString);

      // ---------------------- ACCESSORS ------------------------			
   public:
      LanguagePage*   GetSelectedPage() const;
      LanguageString* GetSelectedString() const;

      /// <summary>Gets any language view</summary>
      /// <typeparam name="VIEW">View type</typeparam>
      /// <returns></returns>
      /// <exception cref="Logic::AlgorithmException">View not found</exception>
      template<typename VIEW>
      VIEW* GetView() const
      {
         // Iterate thru views
         for (POSITION pos = GetFirstViewPosition(); pos != NULL; )
            if (VIEW* v = dynamic_cast<VIEW*>(GetNextView(pos)))
               return v;

         // Error: Not found
         throw AlgorithmException(HERE, L"Cannot find desired View");
      }

      // ----------------------- MUTATORS ------------------------
   public:
      void  SetSelectedPage(LanguagePage* p);
      void  SetSelectedString(LanguageString* s);

   protected:
	   BOOL OnNewDocument() override;
      BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
      void OnQueryClipboardCommand(CCmdUI* pCmd);
      void OnQueryFormatCommand(CCmdUI* pCmd);
      void OnQueryModeCommand(CCmdUI* pCmd);

      // -------------------- REPRESENTATION ---------------------
   public:
      LanguageFile     Content;
      bool             Virtual;

   protected:
      LanguageString*  CurrentString;
      LanguagePage*    CurrentPage;
   };


NAMESPACE_END2(GUI,Documents)

