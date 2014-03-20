#pragma once
#include "ImageListEx.h"
#include "DocumentBase.h"
#include "../Logic/TemplateFile.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary>New document dialog</summary>
   class NewDocumentDialog : public CDialogEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_NEW_DOCUMENT };


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
      const TemplateFile*  GetTemplate(UINT index) const;

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      void InsertTemplate(UINT index, const TemplateFile& t);

      void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      TemplateList   AllTemplates;

      CListCtrl      ListView;
      CEdit          Description;
      Path           FileName, 
                     Folder;
      CButton        AddProject;
      ImageListEx    Images;
   };
   
NAMESPACE_END2(GUI,Windows)
