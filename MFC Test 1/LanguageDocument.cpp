// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"
#include "Logic/FileStream.h"
#include "Logic/LanguageFileReader.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageDocument, CDocument)
   IMPLEMENT_DYNAMIC(LanguageDocTemplate, CMultiDocTemplate)

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
   
   CDocTemplate::Confidence LanguageDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      rpDocMatch = NULL;
      return yesAttemptNative;

	   /*ASSERT(lpszPathName != NULL);
	   rpDocMatch = NULL;

	   // go through all documents
	   POSITION pos = GetFirstDocPosition();
	   while (pos != NULL)
	   {
		   CDocument* pDoc = GetNextDoc(pos);
		   if (AfxComparePath(pDoc->GetPathName(), lpszPathName))
		   {
			   // already open
			   rpDocMatch = pDoc;
			   return yesAlreadyOpen;
		   }
	   }

	   // see if it matches our default suffix
	   CString strFilterExt;
	   if (GetDocString(strFilterExt, CDocTemplate::filterExt) &&
	     !strFilterExt.IsEmpty())
	   {
		   // see if extension matches
		   ASSERT(strFilterExt[0] == '.');
		   LPCTSTR lpszDot = ::PathFindExtension(lpszPathName);
		   if (lpszDot != NULL)
           {
               if(::AfxComparePath(lpszDot, static_cast<const TCHAR *>(strFilterExt)))
               {
			       return yesAttemptNative; // extension matches, looks like ours
               }
           }
	   }

	   // otherwise we will guess it may work
	   return yesAttemptForeign;*/
   }

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

   BOOL LanguageDocument::OnOpenDocument(LPCTSTR szPathName)
   {
      try
      {
         Console << ENDL << Colour::Cyan << L"Parsing language file: " << szPathName << ENDL;

         // Parse file
         StreamPtr fs2( new FileStream(szPathName, FileMode::OpenExisting, FileAccess::Read) );
         Content = LanguageFileReader(fs2).ReadFile(Path(szPathName).FileName);

         Console << Colour::Green << L"Language file loaded successfully" << ENDL;
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", szPathName, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
         return FALSE;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END(GUI)

