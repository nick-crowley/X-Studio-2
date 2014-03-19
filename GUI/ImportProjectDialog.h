#pragma once
#include "ImageListEx.h"
#include "DocumentBase.h"
#include "DialogBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class ImportProjectDialog : public DialogBase
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_IMPORT_PROJECT };

   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      ImportProjectDialog(Path legacyProj, CWnd* parent = nullptr);
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
      
   private:
      void CreateFolder(const Path& f);

      // -------------------- REPRESENTATION ---------------------
   public:
      Path  NewPath;

   protected:
      Path  LegacyFile,
            Folder;
   };
   
NAMESPACE_END2(GUI,Windows)
