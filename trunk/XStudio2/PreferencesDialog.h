#pragma once
#include "GeneralPage.h"
#include "CompilerPage.h"
#include "EditorPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class PreferencesDialog : public CMFCPropertySheet
   {
      // ------------------------ TYPES --------------------------
   public:

      // --------------------- CONSTRUCTION ----------------------
   public:
      PreferencesDialog(CWnd* parent);    
      virtual ~PreferencesDialog();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(PreferencesDialog)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
   public:
      PROPERTY_GET_SET(bool,Modified,GetModified,SetModified);

      // ---------------------- ACCESSORS ------------------------			
   public:
      /// <summary>Gets whether modified.</summary>
      /// <returns></returns>
      bool  GetModified() const
      {
         return IsModified;
      }

      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Sets whether modified.</summary>
      /// <param name="m">Modified</param>
      void  SetModified(bool m)
      {
         IsModified = m;
      }

   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
   
      // -------------------- REPRESENTATION ---------------------
   protected:
      CompilerPage  Compiler;
      EditorPage    Editor;
      GeneralPage   General;
      bool          IsModified;
   };
   
NAMESPACE_END2(GUI,Preferences)
