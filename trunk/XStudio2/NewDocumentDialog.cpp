#include "stdafx.h"
#include "NewDocumentDialog.h"
#include "ProjectDocument.h"
#include "afxdialogex.h"
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   NewDocumentDialog::TemplateList  NewDocumentDialog::DocTemplates = 
   {
      NewDocumentTemplate(L"Blank MSCI Script", DocumentType::Script, L".xml", 0, L"Blank MSCI script", nullptr),
      NewDocumentTemplate(L"Blank Language File", DocumentType::Language, L".xml", 1, L"Blank Language File", nullptr),
      NewDocumentTemplate(L"Blank MD Script", DocumentType::Mission, L".xml", 2, L"Blank MD Script", nullptr),
      NewDocumentTemplate(L"Blank Project", DocumentType::Project, L".xprj", 3, L"Blank Project", nullptr)
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
         UpdateData(TRUE);

         // Require selection + filename + folder
         if (Templates.GetSelectedCount() == 0 || Folder.Empty() || FileName.Empty() || GetTemplate(-1)->Type == DocumentType::Mission)
            return;

         // Manually check for extension (scripts almost always have dots in their name)
         if (!FileName.HasExtension(L".xml") && !FileName.HasExtension(L".pck") && !FileName.HasExtension(L".xprj"))
         {
            FileName = wstring(FileName.c_str()) + GetTemplate(-1)->Extension;
            UpdateData(FALSE);
         }

         // Check folder exists
         if (!Folder.Exists() || !Folder.IsDirectory())
            throw ApplicationException(HERE, L"The folder does not exist");

         // Check file doesn't exist
         if (FileName.Exists())
            throw ApplicationException(HERE, L"The file already exists");

         // Save folder
         PrefsLib.NewDocumentFolder = Folder;

         // TODO: Open document

         // OK
         CDialogEx::OnOK();
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
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
      //DDX_Control(pDX, IDOK, OK);
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
   const NewDocumentDialog::NewDocumentTemplate*  NewDocumentDialog::GetTemplate(UINT index) const
   {
      // Get Selected
      if (index == -1)
         index = Templates.GetNextItem(-1, LVNI_SELECTED);

      // Lookup data
      return reinterpret_cast<const NewDocumentTemplate*>(Templates.GetItemData(index));
   }
   
NAMESPACE_END2(GUI,Windows)


