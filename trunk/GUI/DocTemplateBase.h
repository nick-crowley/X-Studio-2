#pragma once
#include "../Logic/FileIdentifier.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary></summary>
   class DocTemplateBase : public CMultiDocTemplate
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      DocTemplateBase(UINT nIDResource, FileType type, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
      
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(DocTemplateBase)

      // --------------------- PROPERTIES ------------------------
   public:

      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      Confidence     MatchDocType(LPCTSTR szPathName, CDocument*& rpDocMatch) override;
      DocumentBase*  OpenDocumentTemplate(Path path, BOOL bMakeVisible);

      // -------------------- REPRESENTATION ---------------------
   protected:
      FileType  DocType;
   };


/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

