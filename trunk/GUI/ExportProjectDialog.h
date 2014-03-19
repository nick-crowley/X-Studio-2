#pragma once
#include "ImageListEx.h"
#include "DocumentBase.h"

FORWARD_DECLARATION2(GUI,Documents,class ProjectDocument)

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class ExportProjectDialog : public CDialogEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_EXPORT_PROJECT };

   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      ExportProjectDialog();
      virtual ~ExportProjectDialog();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(ExportProjectDialog)
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
      void ExportFiles(ProjectDocument& doc);
      
      // -------------------- REPRESENTATION ---------------------
   protected:
      Path  FileName,
            Folder;
      int   Option;
   };
   
NAMESPACE_END2(GUI,Windows)
