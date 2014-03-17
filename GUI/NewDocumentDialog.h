#pragma once
#include "ImageListEx.h"
#include "DocumentBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class NewDocumentDialog : public CDialogEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_NEW_DOCUMENT };

   protected:
      /// <summary></summary>
      typedef list<FileTemplate>  TemplateList;

      // --------------------- CONSTRUCTION ----------------------
   public:
      NewDocumentDialog();    
      virtual ~NewDocumentDialog();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(NewDocumentDialog)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   
   protected:
      const FileTemplate*  GetTemplate(UINT index) const;

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      static TemplateList  DocTemplates;

      CListCtrl      Templates;
      CEdit          Description;
      Path           Folder;
      CButton        AddProject;
      ImageListEx    Images;
   };
   
NAMESPACE_END2(GUI,Windows)
