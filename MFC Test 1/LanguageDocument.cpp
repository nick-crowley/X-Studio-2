// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"


/// <summary>User interface</summary>
namespace GUI
{

   // LanguageDocument

   IMPLEMENT_DYNCREATE(LanguageDocument, CDocument)

   LanguageDocument::LanguageDocument()
   {
   }

   BOOL LanguageDocument::OnNewDocument()
   {
	   if (!CDocument::OnNewDocument())
		   return FALSE;
	   return TRUE;
   }

   LanguageDocument::~LanguageDocument()
   {
   }


   BEGIN_MESSAGE_MAP(LanguageDocument, CDocument)
   END_MESSAGE_MAP()


   // LanguageDocument diagnostics

   #ifdef _DEBUG
   void LanguageDocument::AssertValid() const
   {
	   CDocument::AssertValid();
   }

   #ifndef _WIN32_WCE
   void LanguageDocument::Dump(CDumpContext& dc) const
   {
	   CDocument::Dump(dc);
   }
   #endif
   #endif //_DEBUG

   #ifndef _WIN32_WCE
   // LanguageDocument serialization

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
   #endif


   // LanguageDocument commands

}
