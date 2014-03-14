#pragma once
#include "DocumentBase.h"
#include "Logic/BackupFile.h"
#include "Logic/ProjectFile.h"
#include "Logic/Event.h"

/// <summary>Forward declaration</summary>
FORWARD_DECLARATION2(GUI,Documents,class ScriptDocument)

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary>MSCI Script document template</summary>
   class ProjectDocTemplate : public CSingleDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ProjectDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(ProjectDocTemplate)

      // ---------------------- ACCESSORS ------------------------	
   protected:
      bool IsLegacyProject(IO::Path path) const;

      // ----------------------- MUTATORS ------------------------
   public:
      Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch) override;
      CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) override;
      CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible) override;
   };

   /// <summary>Project item added/changed/removed event</summary>
   /// <typeparam name="item">Item added/changed/removed</typeparam>
   /// <typeparam name="parent">Parent of item added (otherwise nullptr)</typeparam>
   typedef Event<ProjectItem*, ProjectItem*>   ProjectItemAddedEvent;
   typedef Event<ProjectItem*>                 ProjectItemEvent;

   /// <summary></summary>
   class ProjectDocument : public DocumentBase
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      ProjectDocument();    // Protected constructor used by dynamic creation
   public:
      virtual ~ProjectDocument();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(ProjectDocument)
      DECLARE_MESSAGE_MAP()

   public:
      static SimpleEvent            Loaded;
      static ProjectItemAddedEvent  ItemAdded;
      static ProjectItemEvent       ItemChanged,
                                    ItemRemoved;

      static ProjectDocument*  GetActive();
	  
      // --------------------- PROPERTIES ------------------------
   public:

      // ---------------------- ACCESSORS ------------------------			
   public:
      bool        Contains(IO::Path path) const;
      BackupFile  GetBackupFile(ScriptDocument& doc) const;

      // ----------------------- MUTATORS ------------------------
   public:
      bool  AddFile(IO::Path path, ProjectItem& folder);
      void  AddFolder(const wstring& name, ProjectItem& folder);
      void  Commit(ScriptDocument& doc, const wstring& title);
      void  MoveItem(ProjectItem& item, ProjectItem& folder);
      
      void  OnDocumentEvent(DocumentEvent deEvent) override;
      BOOL  OnNewDocument() override;
      BOOL  OnImportDocument(IO::Path legacy, IO::Path upgrade);
      BOOL  OnOpenDocument(LPCTSTR lpszPathName) override;
      BOOL  OnSaveDocument(LPCTSTR lpszPathName) override;

      void  RemoveItem(ProjectItem& item);
      void  Rename(const wstring& name) override;
      void  RenameItem(ProjectItem& item, const wstring& name);

   protected:
      void  InitialCommit(const IO::Path& folder, const ProjectItem& item);

      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);

      // -------------------- REPRESENTATION ---------------------
   public:
      ProjectFile  Project;

   protected:
   
   };

   

/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

