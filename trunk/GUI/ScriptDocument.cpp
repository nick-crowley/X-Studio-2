// ScriptDocument.cpp : implementation 
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include "FrameWndBase.h"
#include "MainWnd.h"
#include <propkey.h>
#include "CommitDialog.h"
#include "ArgumentDialog.h"
#include "../Logic/FileStream.h"
#include "../Logic/XFileInfo.h"
#include "../Logic/ScriptFileReader.h"
#include "../Logic/ScriptFileWriter.h"
#include "../Logic/WorkerData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(ScriptDocTemplate, CMultiDocTemplate)
   
   /// <summary>Creates script document template</summary>
   ScriptDocTemplate::ScriptDocTemplate() : DocTemplateBase(IDR_SCRIPTVIEW, FileType::Script,
                                                            RUNTIME_CLASS(ScriptDocument), 
                                                            RUNTIME_CLASS(FrameWndBase), 
                                                            RUNTIME_CLASS(ScriptView))
   {}

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptDocument, DocumentBase)

   BEGIN_MESSAGE_MAP(ScriptDocument, DocumentBase)
      ON_COMMAND(ID_FILE_COMMIT, OnCommand_Commit)
      ON_COMMAND(ID_FILE_QUICK_COMMIT, OnCommand_QuickCommit)
      ON_COMMAND(ID_EDIT_ARGUMENT, OnCommand_EditArgument)
      ON_COMMAND(ID_INSERT_ARGUMENT, OnCommand_InsertArgument)
      ON_COMMAND(ID_REMOVE_ARGUMENT, OnCommand_RemoveArgument)
      //ON_UPDATE_COMMAND_UI_RANGE(ID_INSERT_ARGUMENT, ID_REMOVE_ARGUMENT, OnQueryCustomCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptDocument::ScriptDocument() : DocumentBase(DocumentType::Script, false), Script(L"")
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
      
         // Lookup argument
         auto index = GetSelectedArgument();
         auto& arg = Script.Variables[index];

         // Query User. 
         ArgumentDialog dlg(Script, arg, theApp.GetMainWindow());
         if (dlg.DoModal() != IDOK)
            return;
      
         // Remove/Insert argument     [Cannot edit because keyed by name]
         Script.Variables.Remove(arg);
         Script.Variables.Insert(index, dlg.Argument);   

         // Refresh properties. Raise 'ARGUMENT CHANGED'
         CPropertiesWnd::Connect(this, true);
         ArgumentChanged.Raise();
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
         // Query new argument
         ArgumentDialog dlg(Script, theApp.GetMainWindow());
         if (dlg.DoModal() != IDOK)
            return;
      
         // Append to existing arguments
         auto nextIndex = Script.Variables.Arguments.Count;
         Script.Variables.Insert(nextIndex, dlg.Argument);   

         // Refresh properties. Raise 'ARGUMENT CHANGED'
         CPropertiesWnd::Connect(this, true);
         ArgumentChanged.Raise();
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

         // Refresh properties. Raise 'ARGUMENT CHANGED'
         CPropertiesWnd::Connect(this, true);
         ArgumentChanged.Raise();
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
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Committing script '%s'", FullPath.c_str()));

         // Get project
         auto proj = ProjectDocument::GetActive();

         // Verify document is part of project
         if (!proj)
            throw InvalidOperationException(HERE, L"No current project");
         else if (!proj->Contains(FullPath))
            throw InvalidOperationException(HERE, L"Document is not a member of current project");
         
         // Commit
         if (proj->Commit(*this, title))
            data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Script committed successfully");
         else
            data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Script commit aborted");

         return TRUE;
      }
      catch (ExceptionBase&  e) 
      {
         Console.Log(HERE, e, VString(L"Failed to commit script '%s'", FullPath.c_str()));
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
   
   /// <summary>Manually set the path of the string library document</summary>
   /// <param name="deEvent">The de event.</param>
   void ScriptDocument::OnDocumentEvent(DocumentEvent deEvent)
   {
      switch (deEvent)
      {
      // Close: Clear properties
      case onAfterCloseDocument:
         CPropertiesWnd::Connect(this, false);
         break;
      }
      
      // Refresh backups
      __super::OnDocumentEvent(deEvent);
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
      
      try
      {
         // Feedback
         Console << Cons::UserAction << "Loading script: " << Path(szPathName) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Loading script '%s'", szPathName));

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
         theApp.ShowError(HERE, e, VString(L"Failed to load script '%s'", szPathName));
         return FALSE;
      }
   }
   
   /// <summary>Opens a document template.</summary>
   /// <param name="docPath">document path.</param>
   /// <param name="t">template.</param>
   /// <returns></returns>
   BOOL ScriptDocument::OnOpenTemplate(Path docPath, const TemplateFile& t)
   {
      WorkerData data(Operation::LoadSaveDocument);
      
      try
      {
         // Feedback
         Console << Cons::UserAction << "Creating script: " << docPath << " from template: " << Path(t.SubPath) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Creating %s '%s'", t.Name.c_str(), docPath.c_str()));

         // Read/Parse template script
         AppPath path(t.SubPath);
         Script = ScriptFileReader(XFileInfo(path).OpenRead()).ReadFile(path, false);

         // Set properties
         Script.Name = docPath.RemoveExtension().FileName;
         Script.Game = PrefsLib.GameDataVersion;
         Script.Version = 1;
         Script.Description = L"no description";

         // Populate template
         LineArray lines;
         wstring date = COleDateTime(time(nullptr)).Format(L"%d/%m/%Y");
         for (auto& cmd : Script.Commands.Input)
         {
            cmd.Text = cmd.Text.ReplaceAll(L"$NAME$", Script.Name);
            cmd.Text = cmd.Text.ReplaceAll(L"$DATE$", date);
            cmd.Text = cmd.Text.ReplaceAll(L"$AUTHOR$", L"");
            lines.push_back(cmd.Text);
         }

         // Create temp scriptFile
         ScriptFile tmp(Script);
         tmp.FullPath = docPath;

         // Compile changes into temporary script
         ScriptParser parser(tmp, lines, Script.Game);
         parser.Compile();
         
         // Write to disc
         ScriptFileWriter w(XFileInfo(docPath).OpenWrite());
         w.Write(tmp);
         w.Close();

         // Feedback
         data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Script created successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, VString(L"Failed to create %s '%s'", t.Name.c_str(), docPath.c_str()));
         theApp.ShowError(HERE, e, VString(L"Failed to create %s '%s'", t.Name.c_str(), docPath.c_str()));
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
   //   case ID_FILE_QUICK_COMMIT:
   //      break;

   //   // Commit: Query for title, then commit
   //   case ID_FILE_COMMIT:
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

         // Revert path
         if (FullPath != r.FullPath)
         {
            /*auto proj = ProjectDocument::GetActive();
            if (proj && proj->Contains(*this))*/

            FullPath = r.FullPath;
         }

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

         // Clear output pane
         theApp.GetMainWindow()->ClearOutputPane(Operation::LoadSaveDocument, true);

         // Feedback
         Console << Cons::UserAction << "Saving script: " << szPath << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, VString(L"Saving script '%s'", szPath));

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

            // Write script
            ScriptFileWriter w(XFileInfo(szPath).OpenWrite());
            w.Write(Script);
            w.Close();

            // Remove 'Modified'
            SetModifiedFlag(FALSE);

            // Feedback
            data.SendFeedback(Cons::Success, ProgressType::Succcess, 0, L"Script saved successfully");

            // Auto-Commit: Commit if document part of project
            if (auto proj = ProjectDocument::GetActive())
               if (PrefsLib.CommitOnSave && proj->Contains(FullPath))
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
               VString msg(L"%d: %s '%s'", err.Line, err.Message.c_str(), err.Text.c_str());
               data.SendFeedback(ProgressType::Error, 1, msg);
            }

            // Feedback
            data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to save script");
            return FALSE;
         }
      }
      catch (ExceptionBase&  e) 
      {
         Console.Log(HERE, e, VString(L"Failed to save script '%s'", szPath));
         data.SendFeedback(Cons::Error, ProgressType::Failure, 0, L"Failed to save script");
         return FALSE;
      }
   }
   
   /// <summary>Sets the name of the path.</summary>
   /// <param name="szPathName">Name of the sz path.</param>
   /// <param name="bAddToMRU">The b add to MRU.</param>
   void  ScriptDocument::SetPathName(LPCTSTR szPathName, BOOL bAddToMRU /*= TRUE*/)
   {
      __super::SetPathName(szPathName, bAddToMRU);

      // Update ScriptFile path (Used for script-call resolution)
      Script.FullPath = szPathName;
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

