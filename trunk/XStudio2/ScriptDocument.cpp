// ScriptDocument.cpp : implementation 
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include "MainWnd.h"
#include <propkey.h>
#include "CommitDialog.h"
#include "ArgumentDialog.h"
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
      ON_COMMAND(ID_BACKUP_COMMIT, OnCommand_Commit)
      ON_COMMAND(ID_BACKUP_QUICK_COMMIT, OnCommand_QuickCommit)
      ON_COMMAND(ID_EDIT_ARGUMENT, OnCommand_EditArgument)
      ON_COMMAND(ID_INSERT_ARGUMENT, OnCommand_InsertArgument)
      ON_COMMAND(ID_REMOVE_ARGUMENT, OnCommand_RemoveArgument)
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

   /// <summary>Replaces all text in the document.</summary>
   /// <param name="txt">The text.</param>
   void  ScriptDocument::SetAllText(const wstring& txt)
   {
      Edit->SetPlainText(txt);
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
   
   /// <summary>Perform full commit.</summary>
   void  ScriptDocument::OnCommand_Commit()
   {
      // Require project
      if (auto proj = ProjectDocument::GetActive())
      {
         // Query for description
         CommitDialog dlg(theApp.GetMainWindow());
         if (dlg.DoModal() != IDOK)
            return;

         // Commit
         OnCommitDocument(dlg.Description);
      }
   }

   /// <summary>Edit selected argument.</summary>
   void  ScriptDocument::OnCommand_EditArgument()
   {
      try
      {
         // Require selection
         if (GetSelectedArgument() == -1)
            return;
      
         // Query User. 
         auto& arg = Script.Variables[GetSelectedArgument()];
         ArgumentDialog dlg(Script, arg, theApp.GetMainWindow());
         if (dlg.DoModal() != IDOK)
            return;
      
         // Update selected variable
         arg = dlg.Argument;   
         CPropertiesWnd::Connect(this, true);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, L"Unable to edit selected argument");
      }
   }

   /// <summary>Append new argument.</summary>
   void  ScriptDocument::OnCommand_InsertArgument()
   {
      try
      {
         // Query User. Append argument to existing arguments
         ArgumentDialog dlg(Script, theApp.GetMainWindow());
         if (dlg.DoModal() != IDOK)
            return;
      
         // Query User. Append argument to existing arguments
         Script.Variables.Insert(Script.Variables.Arguments.Count, dlg.Argument);   
         CPropertiesWnd::Connect(this, true);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, L"Unable to create new argument");
      }
   }
   
   /// <summary>Perform quick commit.</summary>
   void  ScriptDocument::OnCommand_QuickCommit()
   {
      // Quick Commit
      if (auto proj = ProjectDocument::GetActive())
         OnCommitDocument(L"Quick Commit");
   }

   /// <summary>Remove selected argument.</summary>
   void  ScriptDocument::OnCommand_RemoveArgument()
   {
      try
      {
         // Require selection
         if (GetSelectedArgument() == -1)
            return;
      
         // Remove selected
         Script.Variables.Remove(Script.Variables[GetSelectedArgument()]);   
         CPropertiesWnd::Connect(this, true);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, L"Unable to remove selected argument");
      }
   }

   
   /// <summary>Commits the document</summary>
   /// <param name="title">Revision title</param>
   /// <returns></returns>
   BOOL ScriptDocument::OnCommitDocument(const wstring& title)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      try
      {
         // Feedback
         Console << Cons::UserAction << "Committing script: " << FullPath << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Committing script '%s'", FullPath.c_str()));

         // Get project
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
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to load script");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load script '%s'", szPathName));
         return FALSE;
      }
   }
   
   /// <summary>Called when perform command.</summary>
   /// <param name="nID">The cmd identifier.</param>
   //void  ScriptDocument::OnPerformCommand(UINT nID)
   //{
   //   switch (nID)
   //   {
   //   // Insert Argument: 
   //   case ID_INSERT_ARGUMENT:
   //      break;

   //   // Edit Argument: 
   //   case ID_EDIT_ARGUMENT:
   //   
   //      break;

   //   // Remove Argument: Remove selected
   //   case ID_REMOVE_ARGUMENT:
   //   
   //      break;

   //   // Quick Commit: Commit
   //   case ID_BACKUP_QUICK_COMMIT:
   //      break;

   //   // Commit: Query for title, then commit
   //   case ID_BACKUP_COMMIT:
   //      break;
   //   }
   //}

   /// <summary>Queries the state of a properties toolbar command.</summary>
   /// <param name="pCmd">Command</param>
   void  ScriptDocument::OnQueryCustomCommand(CCmdUI* pCmd) 
   {
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
         pCmd->Enable(GetSelectedArgument() != -1 ? TRUE : FALSE);
         break;
      }
   }   
   
   /// <summary>Called to revert document to a previous version.</summary>
   /// <param name="r">Revision.</param>
   /// <returns>TRUE if successfully reverted, otherwise FALSE</returns>
   BOOL ScriptDocument::OnRevertDocument(const ScriptRevision& r)
   {
      try
      {
         // Feedback
         Console << Cons::UserAction << "Reverting document " << FullPath << ENDL;

         // Modify
         SetModifiedFlag(TRUE);

         // Set Properties
         Script.Version     = r.Version;
         Script.Name        = r.ScriptName;
         Script.Description = r.Description;
         Script.Game        = r.Game;
         Script.CommandID   = r.CommandID;

         // Set text
         SetAllText(r.Content);

         // Rename if appropriate
         if (FullPath != r.FullPath)
            FullPath = r.FullPath;

         // Refresh properties
         CPropertiesWnd::Connect(this, true);

         // Success: Feedback
         Console << Cons::UserAction << "Document reverted successfully" << ENDL;
         return TRUE;
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
         return FALSE;
      }
   }

   /// <summary>Saves the document</summary>
   /// <param name="szPath">The new/existing path.</param>
   /// <returns></returns>
   BOOL ScriptDocument::OnSaveDocument(LPCTSTR szPath)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      try
      {
         REQUIRED(szPath);

         // Feedback
         Console << Cons::UserAction << "Saving script: " << szPath << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Saving script '%s'", szPath));

         // Parse script 
         ScriptParser parser(Script, Edit->GetAllLines(), Script.Game);

         // Compile 
         if (parser.Successful)
            parser.Compile();

         // Verified/Compiled OK:
         if (parser.Successful)
         {
            // IncrementVersion:
            if (PrefsLib.IncrementOnSave)
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
         Console.Log(HERE, e, GuiString(L"Failed to save script '%s'", szPath));
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to save script");
         return FALSE;
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Gets the index of the selected argument.</summary>
   /// <returns></returns>
   int  ScriptDocument::GetSelectedArgument() const
   {
      // lambda: Query selected 
      static function<bool (ArgumentProperty*)> IsSelected = [](ArgumentProperty* p) {return p->IsSelected() != FALSE;};

      // Lookup item, return distance
      auto pos = find_if(ArgumentProperties.begin(), ArgumentProperties.end(), IsSelected);
      return pos != ArgumentProperties.end() ? distance(ArgumentProperties.begin(), pos) : -1;
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Documents)

