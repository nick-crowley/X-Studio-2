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
      class NewDocumentTemplate
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         NewDocumentTemplate(wstring name, DocumentType type, LPCWSTR defExt, UINT icon, LPCWSTR desc, LPCWSTR subpath) 
            : Name(name), Type(type), Extension(defExt), Icon(icon), Description(desc), SubPath(subpath)
         {}

         // --------------------- PROPERTIES ------------------------
	  
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         const wstring      Name;
         const LPCWSTR      Extension,
                            Description,
                            SubPath;
         const DocumentType Type;
         const UINT         Icon;
      };

      /// <summary></summary>
      typedef list<NewDocumentTemplate>  TemplateList;

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
   
   private:
      const NewDocumentTemplate*  GetTemplate(UINT index) const;

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
      Path       //FileName,
                     Folder;
      CButton        AddProject;
      ImageListEx    Images;
   };
   
NAMESPACE_END2(GUI,Windows)
