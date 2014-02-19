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
      GetNextView(pos)->GetParentFrame()->ActivateFrame();
   }

   /// <summary>Finds the next match, if any</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if match found, false otherwise</returns>
   bool  DocumentBase::FindNext(MatchData& m) const
   {
      return false;
   }

   /// <summary>Gets the text selection.</summary>
   /// <returns></returns>
   CHARRANGE DocumentBase::GetSelection() const
   {
      return {0,0};
   }

   /// <summary>Replaces the current match, if any</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if match found, false otherwise</returns>
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

   /// <summary>Sets the selection.</summary>
   /// <param name="rng">char range.</param>
   void  DocumentBase::SetSelection(CHARRANGE rng)
   {
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)
