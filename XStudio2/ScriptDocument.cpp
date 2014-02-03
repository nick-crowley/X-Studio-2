// ScriptDocument.cpp : implementation 
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include <propkey.h>
#include "Logic/FileStream.h"
#include "Logic/ScriptFileReader.h"
#include "Logic/FileIdentifier.h"
#include "Logic/WorkerFeedback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(ScriptDocTemplate, CMultiDocTemplate)
   
   /// <summary>Creates script document template</summary>
   ScriptDocTemplate::ScriptDocTemplate()
         : CMultiDocTemplate(IDR_SCRIPTVIEW, RUNTIME_CLASS(ScriptDocument), RUNTIME_CLASS(CMDIChildWndEx), RUNTIME_CLASS(ScriptView))
   {}

   /// <summary>Queries whether an external file should be opened as a script</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if script, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence ScriptDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CMultiDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return FileIdentifier::Identify(lpszPathName) == FileType::Script ? yesAttemptNative : noAttempt;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptDocument, CDocument)

   BEGIN_MESSAGE_MAP(ScriptDocument, CDocument)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptDocument::ScriptDocument()
   {
	   // TODO: add one-time construction code here

   }

   ScriptDocument::~ScriptDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void ScriptDocument::AssertValid() const
   {
	   CDocument::AssertValid();
   }

   void ScriptDocument::Dump(CDumpContext& dc) const
   {
	   CDocument::Dump(dc);
   }
   #endif //_DEBUG

   void  ScriptDocument::AttachEdit(ScriptEdit& edit)
   {
      Edit = &edit;
   }

   BOOL ScriptDocument::OnNewDocument()
   {
	   if (!CDocument::OnNewDocument())
		   return FALSE;

	   // TODO: add reinitialization code here
	   // (SDI documents will reuse this document)

	   return TRUE;
   }


   BOOL ScriptDocument::OnOpenDocument(LPCTSTR lpszPathName)
   {
      /*if (!CDocument::OnOpenDocument(lpszPathName))
         return FALSE;*/
      
      try
      {
         // Parse script
         Console << ENDL << Colour::Cyan << L"Parsing MSCI script: " << lpszPathName << ENDL;

         StreamPtr fs2( new FileStream(lpszPathName, FileMode::OpenExisting, FileAccess::Read) );
         Script = ScriptFileReader(fs2).ReadFile(lpszPathName, false);

         Console << Colour::Green << L"Script loaded successfully" << ENDL;
      
         //UpdateAllViews(NULL);
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         theApp.ShowError(HERE, e, GuiString(L"Unable to load '%s'", lpszPathName));
         return FALSE;
      }
   }

   

   BOOL ScriptDocument::OnSaveDocument(LPCTSTR lpszPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);
      Console << "Saving document.." << ENDL;

      // Feedback
      data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Saving script '%s'", lpszPathName));

      // Parse script 
      ScriptParser parser(Script, Edit->GetLines(), Script.Game);

      // Print errors in debug window
      for (const auto& err : parser.Errors)
         data.SendFeedback(ProgressType::Error, 1, err.Message);

      // Feedback
      if (parser.Errors.size() == 0)
         data.SendFeedback(ProgressType::Succcess, 0, L"Script saved successfully");
      else
         data.SendFeedback(ProgressType::Failure, 0, L"Failed to save script");
      return TRUE;
   }


   void ScriptDocument::Serialize(CArchive& ar)
   {
	   if (ar.IsStoring())
	   {
		   // TODO: add storing code here
	   }
	   else
	   {
		   // TODO: add loading code here
	   }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Documents)

