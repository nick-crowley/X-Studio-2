// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageDocument, CDocument)

   BEGIN_MESSAGE_MAP(LanguageDocument, CDocument)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageDocument::LanguageDocument()
   {
   }

   LanguageDocument::~LanguageDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   #ifdef _DEBUG
   void LanguageDocument::AssertValid() const
   {
	   CDocument::AssertValid();
   }

   
   void LanguageDocument::Dump(CDumpContext& dc) const
   {
	   CDocument::Dump(dc);
   }
   #endif //_DEBUG

   void LanguageDocument::Serialize(CArchive& ar)
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
   
   BOOL LanguageDocument::OnNewDocument()
   {
	   if (!CDocument::OnNewDocument())
		   return FALSE;
	   return TRUE;
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END(GUI)

