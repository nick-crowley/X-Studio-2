#pragma once
#include "Logic/LanguageFile.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   /// <summary>Language document template</summary>
   class LanguageDocTemplate : public CMultiDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      LanguageDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(LanguageDocTemplate)

      // ---------------------- ACCESSORS ------------------------	

      virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
   };

   /// <summary>Language document</summary>
   class LanguageDocument : public CDocument
   {
	   // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      LanguageDocument();
	   virtual ~LanguageDocument();

      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguageDocument)
   protected:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
     public:
	#ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif
      // ----------------------- MUTATORS ------------------------
   public:
      virtual void Serialize(CArchive& ar);      

   protected:
	   virtual BOOL OnNewDocument();
      virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

      // -------------------- REPRESENTATION ---------------------
   public:
      LanguageFile  Content;
      bool          Virtual;
   };


NAMESPACE_END2(GUI,Documents)

