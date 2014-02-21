#pragma once
#include "DocumentBase.h"


/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary>MSCI Script document template</summary>
   class ProjectDocTemplate : public CSingleDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ProjectDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(ProjectDocTemplate)

      // ---------------------- ACCESSORS ------------------------	

      // ----------------------- MUTATORS ------------------------
   public:
      Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch) override;
      CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) override;
      CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible) override;
   };

   /// <summary></summary>
   class ProjectDocument : public DocumentBase
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      ProjectDocument();    // Protected constructor used by dynamic creation
   public:
      virtual ~ProjectDocument();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(ProjectDocument)
      DECLARE_MESSAGE_MAP()

   public:
      static ProjectDocument*  GetActive();
	  
      // --------------------- PROPERTIES ------------------------
   public:

      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:

      // -------------------- REPRESENTATION ---------------------
   protected:
   };

   

/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

