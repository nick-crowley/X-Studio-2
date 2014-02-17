#include "stdafx.h"
#include "DocumentBase.h"


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

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Finds the next.</summary>
   /// <param name="src">The source.</param>
   /// <returns></returns>
   bool  DocumentBase::FindNext(MatchData& src)
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
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)
