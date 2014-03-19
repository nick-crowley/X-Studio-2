#include "stdafx.h"
#include "ExportProjectDialog.h"
#include "ProjectDocument.h"
#include "afxdialogex.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)


   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(ExportProjectDialog, CDialogEx)

   BEGIN_MESSAGE_MAP(ExportProjectDialog, CDialogEx)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Initializes a new export project dialog.</summary>
   ExportProjectDialog::ExportProjectDialog()
      : CDialogEx(ExportProjectDialog::IDD)
   {
      // Sanity check
      if (!ProjectDocument::GetActive())
         throw AlgorithmException(HERE, L"Project does not exist");

      // Setup dialog
      Folder = PrefsLib.ExportProjectFolder;
      FileName = PrefsLib.ExportProjectFileName;
      Option = 0;
   }

   ExportProjectDialog::~ExportProjectDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Called when initialize dialog.</summary>
   /// <returns></returns>
   BOOL ExportProjectDialog::OnInitDialog()
   {
      return __super::OnInitDialog();
   }


   /// <summary>Called when ok.</summary>
   void ExportProjectDialog::OnOK()
   {
      try
      {
         // Get data
         UpdateData(TRUE);

         // Require filename + folder 
         if (Folder.Empty() || FileName.Empty())
            return;

         // Check folder exists
         if (!Folder.Exists() || !Folder.IsDirectory())
            throw ApplicationException(HERE, L"The folder does not exist");

         // Store prefs
         PrefsLib.ExportProjectFolder = Folder;
         PrefsLib.ExportProjectFileName = FileName;

         // Export project files
         ExportFiles(*ProjectDocument::GetActive());

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
   void ExportProjectDialog::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
      DDX_Text(pDX, IDC_FILENAME_EDIT, FileName);
      DDX_Text(pDX, IDC_FOLDER_EDIT, Folder);
      DDX_Radio(pDX, IDC_ARCHIVE_RADIO, Option);
   }

   /// <summary>Exports the files.</summary>
   /// <param name="doc">The document.</param>
   void ExportProjectDialog::ExportFiles(ProjectDocument& doc)
   {
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   

   
NAMESPACE_END2(GUI,Windows)


