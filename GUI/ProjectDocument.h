#pragma once
#include "DocumentBase.h"
#include "PropertySource.h"
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
   public:
      /// <summary>Project property base</summary>
      class ProjectProperty : public ValidatingProperty
      {
      public:
         ProjectProperty(ProjectDocument& d, ProjectItem& item, wstring name, _variant_t val, wstring desc)
            : Document(d), Item(item), ValidatingProperty(name.c_str(), val, desc.c_str(), NULL, nullptr, nullptr, nullptr)
         {}

      protected:
         /// <summary>Modify document</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Document.SetModifiedFlag(TRUE);
         }

      protected:
         ProjectDocument&  Document;
         ProjectItem&      Item;
      };
	   
      /// <summary>Project item backup filename property grid item</summary>
      class BackupNameProperty : public ProjectProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create item backup filename property.</summary>
         /// <param name="doc">project.</param>
         /// <param name="item">item.</param>
         BackupNameProperty(ProjectDocument& doc, ProjectItem& item)
            : ProjectProperty(doc, item, L"Revisions File", item.BackupName.c_str(), L"Name of file used to store revisions")
         {
            Enable(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Rename backup file</summary>
         /// <param name="value">value text</param>
         /// <exception cref="Logic::ApplicationException">New path already exists, or project already contains new path</exception>
         /// <exception cref="Logic::IOException">Unable to rename file</exception>
         void OnValueChanged(GuiString value) override
         {
            // TODO:
         }
         
         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Project item name property grid item</summary>
      class ItemNameProperty : public ProjectProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create item name property.</summary>
         /// <param name="doc">project.</param>
         /// <param name="item">item.</param>
         /// <param name="description">description.</param>
         ItemNameProperty(ProjectDocument& doc, ProjectItem& item, const wchar* description)
            : ProjectProperty(doc, item, L"Name", item.Name.c_str(), description)
         {
            Enable(!item.Fixed);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update name</summary>
         /// <param name="value">value text</param>
         /// <exception cref="Logic::ApplicationException">New path already exists, or project already contains new path</exception>
         /// <exception cref="Logic::IOException">Unable to rename file</exception>
         void OnValueChanged(GuiString value) override
         {
            // Attempt to Rename file/document/variable
            Document.RenameItem(Item, value);
         }
         
         // -------------------- REPRESENTATION ---------------------
      protected:
      };
      
      /// <summary>Project file-item 'file type' property grid item</summary>
      class FileTypeProperty : public ProjectProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'file type' property.</summary>
         /// <param name="doc">project.</param>
         /// <param name="item">item.</param>
         FileTypeProperty(ProjectDocument& doc, ProjectItem& item)
            : ProjectProperty(doc, item, L"Content", GetString(item.FileType).c_str(), L"Contents of file")
         {
            // Read-Only
            Enable(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Stub</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
         }
         
         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Project item name property grid item</summary>
      class FullPathProperty : public ProjectProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create item path property.</summary>
         /// <param name="doc">project.</param>
         /// <param name="item">item.</param>
         /// <param name="description">description.</param>
         FullPathProperty(ProjectDocument& doc, ProjectItem& item, const wchar* description)
            : ProjectProperty(doc, item, L"Path", item.FullPath.c_str(), description)
         {
            // Read-Only
            Enable(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Stub</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
         }
         
         // -------------------- REPRESENTATION ---------------------
      protected:
      };
      
      /// <summary>Project variable value property grid item</summary>
      class ValueProperty : public ProjectProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create item name property.</summary>
         /// <param name="doc">project.</param>
         /// <param name="item">item.</param>
         ValueProperty(ProjectDocument& doc, ProjectItem& item)
            : ProjectProperty(doc, item, L"Value", item.Value, L"Value of this variable")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update name</summary>
         /// <param name="value">value text</param>
         /// <exception cref="Logic::ApplicationException">New path already exists, or project already contains new path</exception>
         /// <exception cref="Logic::IOException">Unable to rename file</exception>
         void OnValueChanged(GuiString value) override
         {
            Item.Value = value.ToInt();
         }
         
         /// <summary>Validates and optionally modifies the value being input</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            // Require 0 < value <= 9999
            return value.IsNumeric() && value.ToInt() > 0 && value.ToInt() <= 9999;
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };
      
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

