#include "stdafx.h"
#include "DocumentBase.h"
#include "MainWnd.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DocumentBase, CDocument)

   BEGIN_MESSAGE_MAP(DocumentBase, CDocument)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   DocumentBase::DocumentBase()
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

   /// <summary>Gets the text selection.</summary>
   /// <returns></returns>
   CHARRANGE DocumentBase::GetSelection() const
   {
      return {0,0};
   }

   /// <summary>Replaces the current match</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if replaced, false if match was no longer selected</returns>
   bool  DocumentBase::Replace(MatchData& m)
   {
      return false;
   }

   /// <summary>Get the full document path.</summary>
   /// <returns></returns>
   IO::Path  DocumentBase::GetFullPath() const
   {
      return IO::Path((const wchar*)GetPathName());
   }

   /// <summary>Get document type.</summary>
   /// <returns></returns>
   DocumentType  DocumentBase::GetType() const
   {
      return Type;
   }
   
   /// <summary>Changes the fullpath of the document.</summary>
   /// <returns></returns>
   void  DocumentBase::SetFullPath(IO::Path path) 
   {
      SetPathName(path.c_str(), FALSE);
   }

   /// <summary>Sets the selection.</summary>
   /// <param name="rng">char range.</param>
   void  DocumentBase::SetSelection(CHARRANGE rng)
   {
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)
