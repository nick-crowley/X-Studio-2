#pragma once
#include "DocumentBase.h"
#include "../Logic/BackupFile.h"
#include "../Logic/ProjectFile.h"
#include "../Logic/Event.h"

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
      bool IsLegacyProject(Path path) const;

      // ----------------------- MUTATORS ------------------------
   public:
      Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch) override;
      CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther) override;
      CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible) override;
   };

   /// <summary>Project item added event</summary>
   /// <typeparam name="item">Item added</typeparam>
   /// <typeparam name="parent">Parent of item added</typeparam>
   typedef Event<ProjectItem*, ProjectItem*>   ProjectItemAddedEvent;
   typedef ProjectItemAddedEvent::Handler      ProjectItemAddedHandler;

   /// <summary>Project item changed/removed event</summary>
   /// <typeparam name="item">Item changed/removed</typeparam>
   typedef Event<ProjectItem*>                 ProjectItemEvent;
   typedef ProjectItemEvent::Handler           ProjectItemHandler;

   /// <summary>Backup revisions added/removed event</summary>
   /// <typeparam name="doc">Document</typeparam>
   //typedef Event<DocumentBase&>                BackupEvent;

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
      static SimpleEvent            Closed,
                                    Loaded;

      static ProjectItemAddedEvent  ItemAdded;

      static ProjectItemEvent       BackupChanged,
                                    ItemChanged,
                                    ItemRemoved;

      static ProjectDocument*  GetActive();
	  
      // --------------------- PROPERTIES ------------------------
   public:

      // ---------------------- ACCESSORS ------------------------			
   public:
      bool        Contains(Path path) const;
      bool        Contains(const DocumentBase& doc) const;
      Path        GetBackupPath(const ScriptDocument& doc) const;
      bool        HasBackup(const ScriptDocument& doc) const;
      BackupFile  LoadBackupFile(const ScriptDocument& doc) const;

      // ----------------------- MUTATORS ------------------------
   public:
      bool  AddFile(Path path);
      bool  AddFile(Path path, ProjectItem& folder);
      void  AddFolder(const wstring& name, ProjectItem& folder);
      bool  Commit(const ScriptDocument& doc, const wstring& title);
      void  DeleteRevision(const ScriptDocument& doc, UINT index);
      void  MoveItem(ProjectItem& item, ProjectItem& folder);
      void  OnDocumentEvent(DocumentEvent deEvent) override;
      BOOL  OnImportDocument(Path legacy, Path upgrade);
      BOOL  OnNewDocument() override;
      BOOL  OnOpenDocument(LPCTSTR lpszPathName) override;
      BOOL  OnSaveDocument(LPCTSTR lpszPathName) override;
      void  RefreshRevisions(const DocumentBase& doc);
      void  RemoveItem(ProjectItem& item);
      void  RenameItem(ProjectItem& item, const wstring& name);
      void  SetFullPath(Path path) override;
      void  SetModifiedFlag(BOOL bModified = TRUE) override;
      void  UpdateItem(const DocumentBase& doc);

   protected:
      afx_msg void  OnCommandCloseProject()       { OnPerformCommand(ID_FILE_PROJECT_CLOSE);   }
      afx_msg void  OnCommandSaveProject()        { OnPerformCommand(ID_FILE_PROJECT_SAVE);    }
      afx_msg void  OnCommandSaveProjectAs()      { OnPerformCommand(ID_FILE_PROJECT_SAVE_AS); }
      afx_msg void  OnPerformCommand(UINT nID);
      afx_msg void  OnQueryCommand(CCmdUI* pCmdUI);

      // -------------------- REPRESENTATION ---------------------
   public:
      ProjectFile  Project;

   protected:
      bool  IsClosing;
   };

   

/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

