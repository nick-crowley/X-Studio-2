#pragma once
#include "../Logic/MatchData.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary></summary>
   enum class DocumentType { Script, Language, Mission, Project, Diff };

   /// <summary></summary>
   class FileTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      FileTemplate(wstring name, DocumentType type, LPCWSTR defExt, UINT icon, LPCWSTR desc, LPCWSTR subpath) 
         : Name(name), Type(type), Extension(defExt), Icon(icon), Description(desc), SubPath(subpath)
      {}

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   public:
      const wstring      Name;            // Template name
      const LPCWSTR      Extension,       // Default file extension
                         Description,     // Template description
                         SubPath;         // File SubPath: 'Templates\...'
      const DocumentType Type;            // Document type
      const UINT         Icon;            // ImageList index of document icon 
   };


   /// <summary></summary>
   class DocumentBase : public CDocument
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      DocumentBase();
      DocumentBase(DocumentType type, bool isVirtual);
   public:
      virtual ~DocumentBase();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(DocumentBase)
      DECLARE_MESSAGE_MAP()

   public:
      static DocumentBase* GetActive();

      // --------------------- PROPERTIES ------------------------
   public:
      PROPERTY_GET_SET(Path,FullPath,GetFullPath,SetFullPath);
      PROPERTY_GET_SET(bool,Virtual,GetVirtual,SetVirtual);

      // ---------------------- ACCESSORS ------------------------			
   public:
      virtual bool      FindNext(UINT start, MatchData& m) const;
      virtual CHARRANGE GetSelection() const;
      Path              GetFullPath() const;
      DocumentType      GetType() const;
      bool              GetVirtual() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void          Activate();
      BOOL          DoSave(LPCTSTR szPathName, BOOL bReplace = TRUE) override;
      virtual BOOL  OnOpenTemplate(Path docPath, const FileTemplate& t);
      virtual void  Rename(Path newPath, bool overwriteExists);
      virtual bool  Replace(MatchData& m);
      BOOL          SaveModified() override;
      void          SetFullPath(Path path);
      void          SetModifiedFlag(BOOL bModified = TRUE) override;
      void          SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE) override;
      virtual void  SetSelection(CHARRANGE rng);
      void          SetTitle(LPCTSTR title) override;
      void          SetVirtual(bool v);

   protected:
      /*afx_msg void  OnCommand_Save();
      afx_msg void  OnCommand_SaveAs();*/
      afx_msg void  OnQueryCommand(CCmdUI* pCmdUI);
	   
      // -------------------- REPRESENTATION ---------------------
   protected:
      DocumentType  Type;

   private:
      bool          IsVirtual;
   };

   /// <summary>List of documents</summary>
   typedef list<DocumentBase*>  DocumentList;
   

/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

