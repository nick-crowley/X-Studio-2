#include "stdafx.h"
#include "NewDocumentDialog.h"
#include "ProjectDocument.h"
#include "ScriptDocument.h"
#include "LanguageDocument.h"
#include "afxdialogex.h"
#include "Helpers.h"
#include "../Logic/TemplateFileReader.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(NewDocumentDialog, CDialogEx)

   BEGIN_MESSAGE_MAP(NewDocumentDialog, CDialogEx)
      ON_NOTIFY(LVN_ITEMCHANGED, IDC_TEMPLATE_LIST, &NewDocumentDialog::OnItemStateChanged)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   NewDocumentDialog::NewDocumentDialog() : CDialogEx(NewDocumentDialog::IDD)
   {
   }

   NewDocumentDialog::~NewDocumentDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Called when [initialize dialog].</summary>
   /// <returns></returns>
   BOOL NewDocumentDialog::OnInitDialog()
   {
      __super::OnInitDialog();

      // Images
      Images.Create(IDB_NEWDOCUMENT_ICONS, 32, 4, RGB(255,0,255));
      ListView.SetImageList(&Images, LVSIL_NORMAL);
      ListView.ModifyStyle(0, LVS_NOSCROLL);
      ListView.SetExtendedStyle(LVS_EX_SNAPTOGRID);
      //ListView.SendMessage(LVM_SETICONSPACING, 0, MAKELPARAM(32,32));

      // Enable/disable 'add project' check
      AddProject.EnableWindow(ProjectDocument::GetActive() != nullptr);

      // Set folder
      if (PrefsLib.NewDocumentFolder.Empty())
         Folder = PrefsLib.GameDataFolder+L"scripts";
      else
         Folder = PrefsLib.NewDocumentFolder;
      
      // Center + Display 
      CenterWindow();
      UpdateData(FALSE);

      try
      {
         // Read templates
         StreamPtr file(new FileStream(AppPath(L"templates.xml"), FileMode::OpenExisting, FileAccess::Read));
         AllTemplates = TemplateFileReader(file).ReadFile();

         // Populate templates
         int i = 0;
         for (auto& doc : AllTemplates)
            InsertTemplate(i++, doc);

         // Select first template
         ListView.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      }
      // Error: Failed to read templates file
      catch (ExceptionBase& e) {
         ListView.DeleteAllItems();
         theApp.ShowError(HERE, e);
      }
      
      return TRUE;  
   }


   /// <summary>Create new document</summary>
   void NewDocumentDialog::OnOK()
   {
      try
      {
         UpdateData(TRUE);
         
         // Require selection + filename + folder
         if (ListView.GetSelectedCount() == 0 || Folder.Empty() || FileName.Empty())
            return;

         // Get selected template
         auto fileTemplate = GetTemplate(-1);
         if (!fileTemplate)
            throw AlgorithmException(HERE, L"Selected template has no data");

         // Feedback
         Console << Cons::UserAction << "Creating new document " << (Folder + FileName) 
                                     << " from template " << Cons::Yellow << fileTemplate->Name << ENDL;

         // MD: Not supported yet
         if (fileTemplate->Type == FileType::Mission)
            throw NotImplementedException(HERE, L"Mission director support");

         // Check template exists
         if (!AppPath(fileTemplate->SubPath).Exists())
            throw ApplicationException(HERE, VString(L"Cannot find the template file '%s'", fileTemplate->SubPath.c_str()));

         // Check target folder exists
         if (!Folder.Exists() || !Folder.IsDirectory())
            throw ApplicationException(HERE, L"The folder does not exist");

         // Append default path, if none
         if (!FileName.HasExtension(L".pck") && !FileName.HasExtension(L".xml") && !FileName.HasExtension(L".xprj"))
            FileName = FileName.ToString() + fileTemplate->Extension;      // Cannot use 'RenameExtension' because of likelyhood of dots in the file name

         // Check path is valid
         Path path(Folder + FileName);
         if (path.Exists())
            throw ApplicationException(HERE, L"A file with that name already exists");

         // Prefs: Save folder
         PrefsLib.NewDocumentFolder = Folder;

         // Lookup document template
         DocTemplateBase* docTemplate = nullptr;
         switch (fileTemplate->Type)
         {
         case FileType::Script:   docTemplate=theApp.GetDocumentTemplate<ScriptDocTemplate>();    break;
         case FileType::Language: docTemplate=theApp.GetDocumentTemplate<LanguageDocTemplate>();  break;

         default: throw AlgorithmException(HERE, L"Unexpected document template");
         }

         // Open document using file template
         auto doc = docTemplate->OpenDocumentTemplate(path, *fileTemplate, TRUE);

         // Project: Add to active project
         auto proj = ProjectDocument::GetActive();
         if (proj && AddProject.GetCheck())
            proj->AddFile(path);

         // Close
         __super::OnOK();
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, ApplicationException(e), L"Unable to create document");
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void NewDocumentDialog::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
      DDX_Control(pDX, IDC_TEMPLATE_LIST, ListView);
      DDX_Control(pDX, IDC_ADD_PROJECT_CHECK, AddProject);
      DDX_Control(pDX, IDC_DESCRIPTION_EDIT, Description);
      DDX_Text(pDX, IDC_FILENAME_EDIT, FileName);
      DDX_Text(pDX, IDC_FOLDER_EDIT, Folder);
   }
   
   /// <summary>Gets a template by index.</summary>
   /// <param name="index">Zero-based index, or -1 for the currently selected item.</param>
   /// <returns></returns>
   const TemplateFile*  NewDocumentDialog::GetTemplate(UINT index) const
   {
      // Get Selected
      if (index == -1)
         index = ListView.GetNextItem(-1, LVNI_SELECTED);

      // Lookup data
      return reinterpret_cast<const TemplateFile*>(ListView.GetItemData(index));
   }
   
   /// <summary>Inserts an item and stores template as item data</summary>
   /// <param name="index">The index.</param>
   /// <param name="t">template.</param>
   void NewDocumentDialog::InsertTemplate(UINT index, const TemplateFile& t)
   {
      UINT icon = 0;

      // Set icon
      switch (t.Type)
      {
      case FileType::Script:   icon = 0;  break;
      case FileType::Language: icon = 1;  break;
      case FileType::Mission:  icon = 2;  break;
      case FileType::Project:  icon = 3;  break;
      }

      // Insert + Store Data
      ListView.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, index, t.Name.c_str(), 0, 0, icon, reinterpret_cast<LPARAM>(&t));
   }

   /// <summary>Display appropriate description</summary>
   /// <param name="pNMHDR">Item data</param>
   /// <param name="pResult">Notify result.</param>
   void NewDocumentDialog::OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LPNMLISTVIEW pItem = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
      
      // Change description
      if ((pItem->uOldState | pItem->uNewState) & LVIS_SELECTED)     // Selection (not focus) has changed
      {
         if (auto t = GetTemplate(pItem->iItem))
            Description.SetWindowTextW(t->Description.c_str());
      }

      *pResult = 0;
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)


