// ScriptDocument.cpp : implementation 
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include "MainWnd.h"
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
      ON_COMMAND(ID_INSERT_ARGUMENT, OnInsertArgument)
      //ON_UPDATE_COMMAND_UI_RANGE(ID_INSERT_ARGUMENT, ID_REMOVE_ARGUMENT, OnQueryCustomCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptDocument::ScriptDocument() : DocumentBase(DocumentType::Script, false)
   {
   }

   ScriptDocument::~ScriptDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Gets the active script document, if any</summary>
   /// <returns></returns>
   ScriptDocument*  ScriptDocument::GetActive()
   {
      return dynamic_cast<ScriptDocument*>(DocumentBase::GetActive());
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Attaches the script edit.</summary>
   /// <param name="edit">The edit.</param>
   void  ScriptDocument::AttachEdit(ScriptEdit& edit)
   {
      Edit = &edit;
   }

   /// <summary>Finds and highlights the next match, if any</summary>
   /// <param name="start">Starting offset</param>
   /// <param name="m">Match data</param>
   /// <returns>True if found, false otherwise</returns>
   bool  ScriptDocument::FindNext(UINT start, MatchData& m) const
   {
      return GetView()->FindNext(start, m);
   }
   
   /// <summary>Gets all text.</summary>
   /// <returns></returns>
   wstring  ScriptDocument::GetAllText() const
   {
      return Edit->GetAllText();
   }
   
   /// <summary>Gets the text selection.</summary>
   /// <returns></returns>
   CHARRANGE ScriptDocument::GetSelection() const
   {
      return GetView()->GetSelection();
   }
   
   /// <summary>Replaces the current match</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if replaced, false if match was no longer selected</returns>
   bool  ScriptDocument::Replace(MatchData& m)
   {
      SetModifiedFlag(TRUE);
      return GetView()->Replace(m);
   }
   
   /// <summary>Inserts current selection with text.</summary>
   /// <param name="txt">The text.</param>
   void  ScriptDocument::Replace(const wstring& txt)
   {
      SetModifiedFlag(TRUE);
      GetView()->Replace(txt);
   }

   /// <summary>Sets the text selection in the view</summary>
   /// <param name="rng">text range.</param>
   void  ScriptDocument::SetSelection(CHARRANGE rng)
   {
      GetView()->SetSelection(rng);
   }

   /// <summary>Gets the view.</summary>
   /// <returns></returns>
   ScriptView* ScriptDocument::GetView() const
   {
      auto pos = GetFirstViewPosition();
      return dynamic_cast<ScriptView*>(GetNextView(pos));
   }
   
   /// <summary>Commits the document</summary>
   /// <param name="title">Revision title</param>
   /// <returns></returns>
   BOOL ScriptDocument::OnCommitDocument(const wstring& title)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback
      Console << Cons::UserAction << "Committing script: " << FullPath << ENDL;
      data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Committing script '%s'", FullPath.c_str()));

      try
      {
         auto proj = ProjectDocument::GetActive();

         // Verify document is part of project
         if (!proj)
            throw InvalidOperationException(HERE, L"No current project");
         else if (!proj->Contains(FullPath))
            throw InvalidOperationException(HERE, L"Document is not a member of current project");
         
         // Commit
         proj->Commit(*this, title);

         // Feedback
         data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Script committed successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e) 
      {
         Console.Log(HERE, e, GuiString(L"Failed to commit script '%s'", FullPath.c_str()));
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to commit script");
         return FALSE;
      }
   }

   /// <summary>Populates the properties window</summary>
   /// <param name="grid">The grid.</param>
   void  ScriptDocument::OnDisplayProperties(CMFCPropertyGridCtrl& grid)
   {
      // Group: General
      CMFCPropertyGridProperty* general = new CMFCPropertyGridProperty(_T("General"));

      // Name/Description/Version/CommandID/Signature:
      general->AddSubItem(new NameProperty(*this));
      general->AddSubItem(new DescriptionProperty(*this));
      general->AddSubItem(new VersionProperty(*this));
      general->AddSubItem(new CommandIDProperty(*this));
      general->AddSubItem(new GameVersionProperty(*this));
      general->AddSubItem(new SignedProperty(false));
      grid.AddProperty(general);
      
      // Group: Arguments
      CMFCPropertyGridProperty* arguments = new CMFCPropertyGridProperty(_T("Arguments"));

      // Arguments
      ArgumentProperties.clear();
      for (ScriptVariable& v : Script.Variables)
         if (v.Type == VariableType::Argument)
         {
            ArgumentProperties.push_back(new ArgumentProperty(*this, v));
            arguments->AddSubItem(ArgumentProperties.back());
         }
      
      grid.AddProperty(arguments);
   }

   void ScriptDocument::OnInsertArgument()
   {
      AfxMessageBox(L"ScriptDocument::OnInsertArgument");
   }

   /// <summary>Called when new document.</summary>
   /// <returns></returns>
   BOOL ScriptDocument::OnNewDocument()
   {
	   if (!DocumentBase::OnNewDocument())
		   return FALSE;

	   // TODO: add initialization code here

	   return TRUE;
   }


   /// <summary>Opens a document.</summary>
   /// <param name="szPathName">path.</param>
   /// <returns></returns>
   BOOL ScriptDocument::OnOpenDocument(LPCTSTR szPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback
      Console << Cons::UserAction << "Loading script: " << Path(szPathName) << ENDL;
      data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading script '%s'", szPathName));

      try
      {
         // Read/Parse script
         Script = ScriptFileReader(XFileInfo(szPathName).OpenRead()).ReadFile(szPathName, false);

         // Feedback
         data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Script loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(ProgressType::Error, 0, L"Failed to load script");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load script '%s'", szPathName));
         return FALSE;
      }
   }

   /// <summary>Queries the state of a properties toolbar command.</summary>
   /// <param name="pCmd">Command</param>
   void  ScriptDocument::OnQueryCustomCommand(CCmdUI* pCmd) 
   {
      static function<bool (ArgumentProperty*)> IsSelected = [](ArgumentProperty* p) {return p->IsSelected() != FALSE;};

      // Set state
      switch (pCmd->m_nID)
      {
      case ID_INSERT_ARGUMENT:
         pCmd->Enable(TRUE);
         break;

      case ID_REORDER_ARGUMENT_UP:
      case ID_REORDER_ARGUMENT_DOWN:
      case ID_EDIT_ARGUMENT:
      case ID_REMOVE_ARGUMENT:
         pCmd->Enable(any_of(ArgumentProperties.begin(), ArgumentProperties.end(), IsSelected) ? TRUE : FALSE);
         break;
      }
   }   

   /// <summary>Saves the document</summary>
   /// <param name="lpszPathName">path.</param>
   /// <returns></returns>
   BOOL ScriptDocument::OnSaveDocument(LPCTSTR lpszPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      // Feedback
      Console << Cons::UserAction << "Saving script: " << lpszPathName << ENDL;
      data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Saving script '%s'", lpszPathName));

      try
      {
         // Parse script 
         ScriptParser parser(Script, Edit->GetAllLines(), Script.Game);

         // Compile 
         if (parser.Successful)
            parser.Compile();

         // Verified/Compiled OK:
         if (parser.Successful)
         {
            // IncrementVersion:
            if (PrefsLib.IncrementVersionOnSave)
               Script.Version++;

            // Write to X-Studio program folder
            StreamPtr fs(new FileStream(GuiString(L"D:\\My Projects\\XStudio2\\Files\\%s.xml", Script.Name.c_str()), FileMode::CreateAlways, FileAccess::Write));
            ScriptFileWriter w(fs);
            w.Write(Script);
            w.Close();

            // Remove 'Modified'
            SetModifiedFlag(FALSE);

            // Feedback
            data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Script saved successfully");

            // Auto-Commit: 
            if (PrefsLib.CommitOnSave)
               OnCommitDocument(L"Automatic commit");
            return TRUE;
         }
         else
         {
            // Display compiler output window
            theApp.GetMainWindow()->ActivateOutputPane(Operation::LoadSaveDocument, true);

            // DEBUG: Print tree 
            //parser.Print();

            // Feedback messages in output window
            for (const auto& err : parser.Errors)
            {
               GuiString msg(L"%d: %s '%s'", err.Line, err.Message.c_str(), err.Text.c_str());
               data.SendFeedback(ProgressType::Error, 1, msg);
            }

            // Feedback
            data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to save script");
            return FALSE;
         }
      }
      catch (ExceptionBase&  e) 
      {
         Console.Log(HERE, e, GuiString(L"Failed to save script '%s'", lpszPathName));
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to save script");
         return FALSE;
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Documents)

