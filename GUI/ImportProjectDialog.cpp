#include "stdafx.h"
#include "ImportProjectDialog.h"
#include "ProjectDocument.h"
#include "ScriptDocument.h"
#include "LanguageDocument.h"
#include "afxdialogex.h"
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)


   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(ImportProjectDialog, DialogBase)

   BEGIN_MESSAGE_MAP(ImportProjectDialog, DialogBase)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Initializes a new instance of the <see cref="ImportProjectDialog"/> class.</summary>
   /// <param name="legacyProject">Full path of legacy project.</param>
   ImportProjectDialog::ImportProjectDialog(Path legacyProject, CWnd* parent /*= nullptr*/)
      : DialogBase(ImportProjectDialog::IDD, parent, IDB_IMPORT_PROJECT),
        LegacyFile(legacyProject),
        Folder(legacyProject.Folder)
   {
   }

   ImportProjectDialog::~ImportProjectDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Called when [initialize dialog].</summary>
   /// <returns></returns>
   BOOL ImportProjectDialog::OnInitDialog()
   {
      return __super::OnInitDialog();
   }

   /// <summary>Called when [ok].</summary>
   void ImportProjectDialog::OnOK()
   {
      try
      {
         // Get data
         UpdateData(TRUE);

         // Require folder 
         if (Folder.Empty())
            return;

         // Check folder exists
         if (!Folder.Exists())
            CreateFolder(Folder);

         // Require different folder
         if (Folder.Folder == LegacyFile.Folder)
            throw ApplicationException(HERE, L"Cannot import project into its current folder");

         // Set new path
         NewPath = Folder + LegacyFile.FileName;

         // Close
         __super::OnOK();
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void ImportProjectDialog::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
      DDX_Text(pDX, IDC_FILENAME_EDIT, LegacyFile);
      DDX_Text(pDX, IDC_FOLDER_EDIT, Folder);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
   /// <summary>Creates a folder and any intermediate folders.</summary>
   /// <param name="f">The folder.</param>
   void ImportProjectDialog::CreateFolder(const Path& f)
   {
      // Create
      switch (auto res = SHCreateDirectory(m_hWnd, f.c_str()))
      {
      case ERROR_SUCCESS:
      case ERROR_ALREADY_EXISTS:
      case ERROR_FILE_EXISTS:
         break;

      default:
         throw IOException(HERE, SysErrorString(res));
      }  
   }

   
NAMESPACE_END2(GUI,Windows)


