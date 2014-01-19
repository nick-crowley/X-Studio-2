#pragma once


/// <summary>User interface</summary>
namespace GUI
{

   // LanguageDocument document

   class LanguageDocument : public CDocument
   {
	   DECLARE_DYNCREATE(LanguageDocument)

   public:
	   LanguageDocument();
	   virtual ~LanguageDocument();
   #ifndef _WIN32_WCE
	   virtual void Serialize(CArchive& ar);   // overridden for document i/o
   #endif
   #ifdef _DEBUG
	   virtual void AssertValid() const;
   #ifndef _WIN32_WCE
	   virtual void Dump(CDumpContext& dc) const;
   #endif
   #endif

   protected:
	   virtual BOOL OnNewDocument();

	   DECLARE_MESSAGE_MAP()
   };

}

using namespace GUI;
