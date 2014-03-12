#pragma once
#include "ImageListEx.h"
#include "DocumentBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class ImportProjectDialog : public CDialogEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_IMPORT_PROJECT };

   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      ImportProjectDialog(IO::Path legacyProj);
      virtual ~ImportProjectDialog();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(ImportProjectDialog)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      
      // -------------------- REPRESENTATION ---------------------
   public:
      IO::Path  NewPath;

   protected:
      IO::Path  LegacyFile,
                Folder;
   };
   
NAMESPACE_END2(GUI,Windows)
