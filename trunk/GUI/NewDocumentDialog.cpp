#include "stdafx.h"
#include "NewDocumentDialog.h"
#include "ProjectDocument.h"
#include "ScriptDocument.h"
#include "LanguageDocument.h"
#include "afxdialogex.h"
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>New Document File Templates</summary>
   NewDocumentDialog::TemplateList  NewDocumentDialog::DocTemplates = 
   {
      FileTemplate(L"Blank MSCI Script", DocumentType::Script, L".xml", 0, L"Blank MSCI script", L"Templates\\Blank.MSCI.xml"),
      FileTemplate(L"Blank Language File", DocumentType::Language, L".xml", 1, L"Blank Language File", L"Templates\\Blank.Language.xml"),
      FileTemplate(L"Blank MD Script", DocumentType::Mission, L".xml", 2, L"Blank MD Script", nullptr),
      FileTemplate(L"Blank Project", DocumentType::Project, L".xprj", 3, L"Blank Project", nullptr)
   };

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
      CDialogEx::OnInitDialog();

      // Images
      Images.Create(IDB_NEWDOCUMENT_ICONS, 32, 4, RGB(255,0,255));
      Templates.SetImageList(&Images, LVSIL_NORMAL);

      // Populate templates
      int i = 0;
      for (auto& doc : DocTemplates)
         Templates.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, i++, doc.Name.c_str(), 0, 0, doc.Icon, (LPARAM)&doc);
      Templates.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

      // Enable/disable 'add project' check
      AddProject.EnableWindow(ProjectDocument::GetActive() != nullptr);

      // Set folder
      if (PrefsLib.NewDocumentFolder.Empty())
         Folder = PrefsLib.GameDataFolder+L"scripts";
      else
         Folder = PrefsLib.NewDocumentFolder;

      UpdateData(FALSE);
      return TRUE;  
   }


   /// <summary>Called when [ok].</summary>
   void NewDocumentDialog::OnOK()
   {
      try
      {
         // Get data
         UpdateData(TRUE);
         auto fileTemplate = GetTemplate(-1);

         // Require selection + filename + folder
         if (Templates.GetSelectedCount() == 0 || Folder.Empty() || FileName.Empty())
            return;

         // MD: Not supported yet
         if (fileTemplate->Type == DocumentType::Mission)
            throw NotImplementedException(HERE, L"Mission director support");

         // Check folder exists
         if (!Folder.Exists() || !Folder.IsDirectory())
            throw ApplicationException(HERE, L"The folder does not exist");

         // Append default path, if none
         if (!FileName.HasExtension(L".pck") && !FileName.HasExtension(L".xml") && !FileName.HasExtension(L".xprj"))
            FileName = FileName.operator std::wstring() + fileTemplate->Extension;      // Cannot use 'RenameExtension' because of likelyhood of dots in the file name

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
         case DocumentType::Script:   docTemplate=theApp.GetDocumentTemplate<ScriptDocTemplate>();    break;
         case DocumentType::Language: docTemplate=theApp.GetDocumentTemplate<LanguageDocTemplate>();  break;

         default: throw AlgorithmException(HERE, L"Unexpected document template");
         }

         // Open document using file template
         auto doc = docTemplate->OpenDocumentTemplate(path, *fileTemplate, TRUE);

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
      DDX_Control(pDX, IDC_TEMPLATE_LIST, Templates);
      DDX_Control(pDX, IDC_ADD_PROJECT_CHECK, AddProject);
      DDX_Control(pDX, IDC_DESCRIPTION_EDIT, Description);
      DDX_Text(pDX, IDC_FILENAME_EDIT, FileName);
      DDX_Text(pDX, IDC_FOLDER_EDIT, Folder);
   }
   
   /// <summary>Enable OK button when user selects a template</summary>
   /// <param name="pNMHDR">Item data</param>
   /// <param name="pResult">Notify result.</param>
   void NewDocumentDialog::OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LPNMLISTVIEW pItem = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
      
      // Change description
      if ((pItem->uOldState | pItem->uNewState) & LVIS_SELECTED)     // Selection (not focus) has changed
         Description.SetWindowTextW(GetTemplate(pItem->iItem)->Description);

      *pResult = 0;
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   /// <summary>Gets a template by index.</summary>
   /// <param name="index">Zero-based index, or -1 for the currently selected item.</param>
   /// <returns></returns>
   const FileTemplate*  NewDocumentDialog::GetTemplate(UINT index) const
   {
      // Get Selected
      if (index == -1)
         index = Templates.GetNextItem(-1, LVNI_SELECTED);

      // Lookup data
      return reinterpret_cast<const FileTemplate*>(Templates.GetItemData(index));
   }
   
NAMESPACE_END2(GUI,Windows)


