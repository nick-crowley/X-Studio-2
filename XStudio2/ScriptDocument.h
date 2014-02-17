#pragma once
#include "Logic/ScriptFile.h"
#include "DocumentBase.h"

/// <summary>Forward declaration</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   class ScriptEdit;
NAMESPACE_END2(GUI,Controls)

/// <summary>Forward declaration</summary>
NAMESPACE_BEGIN2(GUI,Views)
   class ScriptView;
NAMESPACE_END2(GUI,Views)


/// <summary>Documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   /// <summary>MSCI Script document template</summary>
   class ScriptDocTemplate : public CMultiDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ScriptDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(ScriptDocTemplate)

      // ---------------------- ACCESSORS ------------------------	
      virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
   };

   /// <summary>MSCI Script document</summary>
   class ScriptDocument : public DocumentBase
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------

   protected: // create from serialization only
	   ScriptDocument();
   public:
	   virtual ~ScriptDocument();

      // ------------------------ STATIC -------------------------

   protected:
      DECLARE_DYNCREATE(ScriptDocument)
	   DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      ScriptView* GetView();

      // ----------------------- MUTATORS ------------------------
   public:
      void          AttachEdit(ScriptEdit& edit);
      virtual bool  FindNext(MatchData& src);
      void          SetSelection(CHARRANGE rng);

	   virtual BOOL OnNewDocument();
      virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
      virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
   
      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptFile  Script;

   private:
      ScriptEdit* Edit;
   };


NAMESPACE_END2(GUI,Documents)

