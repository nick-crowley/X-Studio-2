#include "stdafx.h"
#include "DocumentBase.h"
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DocumentBase, CDocument)

   BEGIN_MESSAGE_MAP(DocumentBase, CDocument)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &DocumentBase::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &DocumentBase::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, &DocumentBase::OnQueryCommand)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   DocumentBase::DocumentBase() : IsVirtual(false)
   {
   }

   DocumentBase::DocumentBase(DocumentType type, bool virt) : Type(type), IsVirtual(virt)
   {
   }

   DocumentBase::~DocumentBase()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Gets the active script or language document</summary>
   /// <returns></returns>
   DocumentBase*  DocumentBase::GetActive()
   {
      CMDIChildWnd* pChild = theApp.GetMainWindow()->MDIGetActive();

      if (!pChild)
         return nullptr;

      return (DocumentBase*)pChild->GetActiveDocument();
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Activates this document</summary>
   void  DocumentBase::Activate()
   {
      auto pos = GetFirstViewPosition();

      // Activate parent frame of view (if any)
      if (auto view = GetNextView(pos))
         if (auto parent = view->GetParentFrame())
            parent->ActivateFrame();
   }

   /// <summary>Finds and highlights the next match, if any</summary>
   /// <param name="start">Starting offset</param>
   /// <param name="m">Match data</param>
   /// <returns>True if found, false otherwise</returns>
   bool  DocumentBase::FindNext(UINT start, MatchData& m) const
   {
      return false;
   }

   /// <summary>Get the full document path.</summary>
   /// <returns></returns>
   Path  DocumentBase::GetFullPath() const
   {
      return Path((const wchar*)GetPathName());
   }
   
   /// <summary>Gets the text selection.</summary>
   /// <returns></returns>
   CHARRANGE  DocumentBase::GetSelection() const
   {
      return {0,0};
   }

   /// <summary>Get document type.</summary>
   /// <returns></returns>
   DocumentType  DocumentBase::GetType() const
   {
      return Type;
   }

   /// <summary>Get whether virtual.</summary>
   bool  DocumentBase::GetVirtual() const
   {
      return IsVirtual;
   }
   
   /// <summary>Changes the document filename and updates the title.</summary>
   /// <param name="name">New filename.</param>
   void  DocumentBase::Rename(const wstring& name)
   {
      FullPath = FullPath.RenameFileName(name);
   }
   
   /// <summary>Replaces the current match</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if replaced, false if match was no longer selected</returns>
   bool  DocumentBase::Replace(MatchData& m)
   {
      return false;
   }

   /// <summary>Changes the path of the document.</summary>
   /// <returns></returns>
   void  DocumentBase::SetFullPath(Path path) 
   {
      // Change path. (Don't add to MRU - will cause argument exception if file doesn't exist)
      SetPathName(path.c_str(), FALSE);
      SetModifiedFlag(TRUE);

      // Update title
      SetTitle(path.FileName.c_str());
   }

   /// <summary>Sets the modified flag and updates the document title</summary>
   /// <param name="bModified">modified flag.</param>
   void  DocumentBase::SetModifiedFlag(BOOL bModified)
   {
      // Update flag
      __super::SetModifiedFlag(bModified);

      // Get title
      wstring title = (LPCWSTR)GetTitle();
      if (title.empty())
         return;

      // Add/Remove trailing '*'
      if (bModified && title.back() != '*')
         title += L'*';
      else if (!bModified && title.back() == '*')
         title.pop_back();

      // Update title
      __super::SetTitle(title.c_str());
   }

   /// <summary>Sets the selection.</summary>
   /// <param name="rng">char range.</param>
   void  DocumentBase::SetSelection(CHARRANGE rng)
   {
   }

   /// <summary>Sets the title, and adds a trailing '*' if document is modified</summary>
   /// <param name="title">title.</param>
   void  DocumentBase::SetTitle(LPCTSTR title) 
   {
      wstring txt(title);

      // Modified: Append *
      if (IsModified())
         txt += L"*";

      // Set title
      __super::SetTitle(txt.c_str());
   }
   
   /// <summary>Sets whether virtual.</summary>
   /// <param name="v">state.</param>
   void  DocumentBase::SetVirtual(bool v)
   {
      IsVirtual = v;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void  DocumentBase::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;

      switch (pCmdUI->m_nID)
      {
      // Save/SaveAs: Require file document
      case ID_FILE_SAVE:
      case ID_FILE_SAVE_AS:  state = !Virtual;   break;
      // SaveAll: Always enabled
      case ID_FILE_SAVE_ALL: state = true;       break;
      }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)
