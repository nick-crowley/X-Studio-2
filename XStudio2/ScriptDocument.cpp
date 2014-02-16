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
  
   IMPLEMENT_DYNCREATE(ScriptDocument, DocumentBase)

   BEGIN_MESSAGE_MAP(ScriptDocument, DocumentBase)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptDocument::ScriptDocument() 
   {
	   Type = DocumentType::Script;
   }

   ScriptDocument::~ScriptDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Attaches the script edit.</summary>
   /// <param name="edit">The edit.</param>
   void  ScriptDocument::AttachEdit(ScriptEdit& edit)
   {
      Edit = &edit;
   }

   /// <summary>Gets the view.</summary>
   /// <returns></returns>
   ScriptView* ScriptDocument::GetView()
   {
      auto pos = GetFirstViewPosition();
      return dynamic_cast<ScriptView*>(GetNextView(pos));
   }

   BOOL ScriptDocument::OnNewDocument()
   {
	   if (!DocumentBase::OnNewDocument())
		   return FALSE;

	   // TODO: add initialization code here

	   return TRUE;
   }


   /// <summary>Opens a document.</summary>
   /// <param name="lpszPathName">path.</param>
   /// <returns></returns>
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

   

   /// <summary>Saves the document</summary>
   /// <param name="lpszPathName">path.</param>
   /// <returns></returns>
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

         // Compile 
         if (parser.Errors.empty())
            parser.Compile();

         // ERRORS: Print tree
         //DEBUG: if (!parser.Errors.empty())
            parser.Print();
         
         // ERRORS: Feedback messages in output window
         for (const auto& err : parser.Errors)
         {
            GuiString msg(L"%d: %s '%s'", err.Line, err.Message.c_str(), err.Text.c_str());
            data.SendFeedback(ProgressType::Error, 1, msg);
         }

         // Success: Write
         if (parser.Errors.empty())
         {
            // DEBUG: Tweak name
            wstring orig_name(Script.Name);
            if (Script.Name.front() == L'!')
               Script.Name.erase(0, 1);
            Script.Name.insert(0, L"zz.");

            // Write to X-Studio program folder
            StreamPtr fs(new FileStream(GuiString(L"D:\\My Projects\\XStudio2\\XStudio2\\%s.xml", Script.Name.c_str()), FileMode::CreateAlways, FileAccess::Write));
            ScriptFileWriter w(fs);
            w.Write(Script);
            w.Close();

            // DEBUG: Revert name
            Script.Name = orig_name;

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

   /// <summary>Finds the next text match</summary>
   /// <param name="src">search data</param>
   /// <returns></returns>
   bool  ScriptDocument::FindNext(SearchOperation& src)
   {
      return GetView()->FindNext(src);
   }

   /// <summary>Sets the text selection in the view</summary>
   /// <param name="rng">text range.</param>
   void  ScriptDocument::SetSelection(CHARRANGE rng)
   {
      GetView()->SetSelection(rng);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Documents)

