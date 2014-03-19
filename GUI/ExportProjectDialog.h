#pragma once
#include "ImageListEx.h"
#include "DocumentBase.h"
#include "../Logic/ProjectFile.h"

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
      class ExportPath
      {
      public:
         ExportPath(const Path& full, const Path& sub) : FullPath(full), SubPath(sub)
         {}

      public:
         Path FullPath,
              SubPath;
      };

      typedef list<ExportPath> ExportPaths;

      // --------------------- CONSTRUCTION ----------------------
   public:
      ExportProjectDialog(CWnd* parent = nullptr);
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
      void EnumerateFiles(ProjectItem& root, Path folder, ExportPaths& files);
      void ExportFiles(ExportPaths& files);

      afx_msg void OnOptionChanged();
      
   private:
      void CreateFolder(const Path& f);
      void Abort(list<Path> created);

      // -------------------- REPRESENTATION ---------------------
   protected:
      const static UINT OPTION_ZIP = 0,
                        OPTION_COPY = 1;

      Path  FileName,
            Folder;
      int   Option;
   };
   
NAMESPACE_END2(GUI,Windows)
