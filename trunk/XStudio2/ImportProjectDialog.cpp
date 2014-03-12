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
  
   IMPLEMENT_DYNAMIC(ImportProjectDialog, CDialogEx)

   BEGIN_MESSAGE_MAP(ImportProjectDialog, CDialogEx)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Initializes a new instance of the <see cref="ImportProjectDialog"/> class.</summary>
   /// <param name="legacyProject">Full path of legacy project.</param>
   ImportProjectDialog::ImportProjectDialog(IO::Path legacyProject)
      : CDialogEx(ImportProjectDialog::IDD),
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
         if (!Folder.Exists() || !Folder.IsDirectory())
            throw ApplicationException(HERE, L"The folder does not exist");

         // Open document
         /*switch (t->Type)
         {
         case DocumentType::Script:   theApp.GetDocumentTemplate<ScriptDocTemplate>()->OpenDocumentFile(nullptr, FALSE, TRUE);    break;
         case DocumentType::Language: theApp.GetDocumentTemplate<LanguageDocTemplate>()->OpenDocumentFile(nullptr, FALSE, TRUE);  break;
         }*/

         // OK
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
      DDX_Text(pDX, IDC_FOLDER_EDIT, Folder);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   

   
NAMESPACE_END2(GUI,Windows)


