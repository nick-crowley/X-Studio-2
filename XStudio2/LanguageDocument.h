#pragma once
#include "Logic/LanguageFile.h"
#include "DocumentBase.h"
#include "PropertySource.h"

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
   class LanguageDocument : public DocumentBase, public PropertySource
   {
	   // ------------------------ TYPES --------------------------
   public:
	   typedef LanguageFile::PageCollection PageCollection;

   protected:
      /// <summary>Base class for all Language document properties</summary>
      class LanguageProperty : public ValidatingProperty
      {
      public:
         LanguageProperty(LanguageDocument& d, wstring name, _variant_t val, wstring desc)
            : Document(d), File(d.Content), ValidatingProperty(name.c_str(), val, desc.c_str(), NULL, nullptr, nullptr, nullptr)
         {
            // Disable if document is virtual
            AllowEdit(Document.Virtual ? FALSE : TRUE);
         }

      protected:
         /// <summary>Modify document</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Document.SetModifiedFlag(TRUE);
         }

      protected:
         LanguageDocument&  Document;
         LanguageFile&      File;
      };



      /// <summary>Language file ID property grid item</summary>
      class FileIDProperty : public LanguageProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create language file ID property.</summary>
         /// <param name="doc">Language document.</param>
         FileIDProperty(LanguageDocument& doc)
            : LanguageProperty(doc, L"ID", GuiString(L"%d", doc.Content.ID).c_str(),  L"File ID")
              
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures ID is between 0 and 1000</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return !value.length() || (value.IsNumeric() && value.ToInt() >= 1 && value.ToInt() <= 1000);
         }

         /// <summary>Update file ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            File.ID = value.ToInt();
            
            __super::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };



      /// <summary>Game language property grid item</summary>
      class FileLanguageProperty : public LanguageProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create game language property.</summary>
         /// <param name="doc">Language document.</param>
         FileLanguageProperty(LanguageDocument& doc)
            : LanguageProperty(doc, L"Language", GetString(doc.Content.Language).c_str(),  L"Language of all strings in document")
              
         {
            // Populate game languages
            for (int i = 0; i <= 7; i++)
               AddOption(GetString(GameLanguageIndex(i).Language).c_str(), FALSE);

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update language</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            File.Language = GameLanguageIndex(Find(value.c_str())).Language;      // Convert zero-based index into GameLanguage
            
            __super::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

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
      PageCollection&  GetContent();
      LanguagePage*    GetSelectedPage() const;
      LanguageString*  GetSelectedString() const;

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
      void  OnDisplayProperties(CMFCPropertyGridCtrl& grid) override;
      void  OnCloseDocument() override;
      BOOL  OnNewDocument() override;
      BOOL  OnOpenDocument(LPCTSTR lpszPathName) override;
      void  SetSelectedPage(LanguagePage* p);
      void  SetSelectedString(LanguageString* s);

   protected:
      void OnQueryClipboardCommand(CCmdUI* pCmd);
      void OnQueryFileCommand(CCmdUI* pCmd);
      void OnQueryFormatCommand(CCmdUI* pCmd);
      void OnQueryModeCommand(CCmdUI* pCmd);

      // -------------------- REPRESENTATION ---------------------
   public:
      LanguageFile     Content;
      PageCollection   Library;
      bool             Virtual;

   protected:
      LanguageString*  CurrentString;
      LanguagePage*    CurrentPage;
};


NAMESPACE_END2(GUI,Documents)

