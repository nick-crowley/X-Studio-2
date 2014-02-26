// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"
#include "LanguageFrame.h"
#include "LanguageEditView.h"
#include "Logic/FileStream.h"
#include "Logic/LanguageFileReader.h"
#include "Logic/FileIdentifier.h"
#include "Logic/WorkerFeedback.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(LanguageDocTemplate, CMultiDocTemplate)

   /// <summary>Creates language document template</summary>
   LanguageDocTemplate::LanguageDocTemplate() 
      : CMultiDocTemplate(IDR_LANGUAGEVIEW, RUNTIME_CLASS(LanguageDocument), RUNTIME_CLASS(LanguageFrame), nullptr)
   {}

   /// <summary>Queries whether an external file should be opened as a language file</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if language file, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence LanguageDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CMultiDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return GuiString(L"String Library") == lpszPathName || FileIdentifier::Identify(lpszPathName) == FileType::Language ? yesAttemptNative : noAttempt;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageDocument, DocumentBase)
   
   BEGIN_MESSAGE_MAP(LanguageDocument, DocumentBase)
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageDocument::OnQueryEditMode)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageDocument::LanguageDocument() : DocumentBase(DocumentType::Language), CurrentString(nullptr), CurrentPage(nullptr)
   {
   }

   LanguageDocument::~LanguageDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Gets the selected page.</summary>
   /// <returns></returns>
   LanguagePage*   LanguageDocument::GetSelectedPage() const
   {
      return CurrentPage;
   }

   /// <summary>Gets the selected string.</summary>
   /// <returns></returns>
   LanguageString* LanguageDocument::GetSelectedString() const
   {
      return CurrentString;
   }

   /// <summary>Sets the selected page.</summary>
   /// <param name="p">The page.</param>
   void  LanguageDocument::SetSelectedPage(LanguagePage* p)
   {
      CurrentPage = p;
   }

   /// <summary>Sets the selected string.</summary>
   /// <param name="s">The string.</param>
   void  LanguageDocument::SetSelectedString(LanguageString* s)
   {
      CurrentString = s;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   BOOL LanguageDocument::OnNewDocument()
   {
	   if (!DocumentBase::OnNewDocument())
		   return FALSE;
	   return TRUE;
   }

   BOOL LanguageDocument::OnOpenDocument(LPCTSTR szPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);

      try
      {
         // Feedback
         Console << Cons::UserAction << L"Loading language file: " << Path(szPathName) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading language file '%s'", szPathName));

         // Parse file
         if (GuiString(L"String Library") == szPathName)
            Virtual = true;
         else
         {
            StreamPtr fs2( new FileStream(szPathName, FileMode::OpenExisting, FileAccess::Read) );
            Content = LanguageFileReader(fs2).ReadFile(Path(szPathName).FileName);
            Virtual = false;
         }

         data.SendFeedback(Cons::Green, ProgressType::Succcess, 0, L"Language file loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(ProgressType::Failure, 0, L"Failed to load language file");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load language file '%s'", szPathName));
         return FALSE;
      }
   }

   
   /// <summary>Query state of editor mode commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageDocument::OnQueryEditMode(CCmdUI* pCmd)
   {
      // Delegate to edit view
      GetView<LanguageEditView>()->OnQueryEditMode(pCmd);
   }


   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END2(GUI,Documents)

