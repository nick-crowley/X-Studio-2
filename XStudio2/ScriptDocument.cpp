// ScriptDocument.cpp : implementation 
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include <propkey.h>
#include "Logic/FileStream.h"
#include "Logic/XFileInfo.h"
#include "Logic/ScriptFileReader.h"
#include "Logic/ScriptFileWriter.h"
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
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback
      Console << Cons::Heading << "Loading script: " << lpszPathName << ENDL;
      data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading script '%s'", lpszPathName));

      try
      {
         // Read/Parse script
         Script = ScriptFileReader(XFileInfo(lpszPathName).OpenRead()).ReadFile(lpszPathName, false);

         // Feedback
         data.SendFeedback(Colour::Green, ProgressType::Succcess, 0, L"Script loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(ProgressType::Failure, 0, L"Failed to load script");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load script '%s'", lpszPathName));
         return FALSE;
      }
   }

   

   BOOL ScriptDocument::OnSaveDocument(LPCTSTR lpszPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback
      Console << Cons::Heading << "Saving script: " << lpszPathName << ENDL;
      data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Saving script '%s'", lpszPathName));

      try
      {
         // Parse script 
         ScriptParser parser(Script, Edit->GetLines(), Script.Game);

         // Compile if possible
         if (parser.Errors.empty())
            parser.Compile();

         // Print errors in debug window
         for (const auto& err : parser.Errors)
         {
            GuiString msg(L"%d: %s '%s'", err.Line, err.Message.c_str(), Edit->GetTokenText(err).c_str());
            data.SendFeedback(ProgressType::Error, 1, msg);
         }

         // Success: Write
         if (parser.Errors.empty())
         {
            // Write to OUTPUT debugging file
            StreamPtr fs(new FileStream(L"D:\\My Projects\\XStudio2\\XStudio2\\output.xml", FileMode::CreateAlways, FileAccess::Write));
            ScriptFileWriter w(fs);
            w.Write(Script);
            w.Close();

            // Feedback
            data.SendFeedback(Colour::Green, ProgressType::Succcess, 0, L"Script saved successfully");
            return TRUE;
         }
      }
      catch (ExceptionBase&  e) {
         Console.Log(HERE, e, GuiString(L"Failed to save script '%s'", lpszPathName));
      }

      // Feedback
      data.SendFeedback(Colour::Red, ProgressType::Failure, 0, L"Failed to save script");
      return FALSE;
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

