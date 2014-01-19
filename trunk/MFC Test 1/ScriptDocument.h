// MFC Test 1Doc.h : interface of the ScriptDocument class
//
#pragma once
#include "Logic/ScriptFile.h"

/// <summary>User interface</summary>
namespace GUI
{

   class ScriptDocument : public CDocument
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------

   protected: // create from serialization only
	   ScriptDocument();
	   DECLARE_DYNCREATE(ScriptDocument)
   public:
	   virtual ~ScriptDocument();

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif

	   virtual BOOL OnNewDocument();
      virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	   virtual void Serialize(CArchive& ar);
   
   
      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptFile  Script;

   protected:
	   DECLARE_MESSAGE_MAP()

   };

}

using namespace GUI;
