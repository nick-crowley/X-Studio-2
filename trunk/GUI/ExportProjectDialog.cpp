#include "stdafx.h"
#include "ExportProjectDialog.h"
#include "ProjectDocument.h"
#include "afxdialogex.h"
#include "../Logic/ZipFile.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)


   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(ExportProjectDialog, CDialogEx)

   BEGIN_MESSAGE_MAP(ExportProjectDialog, CDialogEx)
      ON_BN_CLICKED(IDC_ARCHIVE_RADIO, OnOptionChanged)
      ON_BN_CLICKED(IDC_FOLDER_RADIO, OnOptionChanged)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Initializes a new export project dialog.</summary>
   /// <param name="parent">parent window.</param>
   ExportProjectDialog::ExportProjectDialog(CWnd* parent /*= nullptr*/)
      : CDialogEx(ExportProjectDialog::IDD, parent), Image(IDB_EXPORT_PROJECT)
   {
      // Sanity check
      if (!ProjectDocument::GetActive())
         throw AlgorithmException(HERE, L"Project does not exist");

      // Setup dialog
      Folder = PrefsLib.ExportProjectFolder;
      FileName = PrefsLib.ExportProjectFileName;
      Option = OPTION_ZIP;
   }

   ExportProjectDialog::~ExportProjectDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   void AFXAPI DDX_Static(CDataExchange* pDX, int id, CStatic& ctrl)
   {
      DDX_Control(pDX, id, ctrl);

      // Ensure as OwnerDraw
      if ((ctrl.GetStyle() & SS_OWNERDRAW) == 0)
         ctrl.ModifyStyle(0, SS_OWNERDRAW, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Called when initialize dialog.</summary>
   /// <returns></returns>
   BOOL ExportProjectDialog::OnInitDialog()
   {
      __super::OnInitDialog();

      // Enable 'Filename' for archive mode
      GetDlgItem(IDC_FILENAME_EDIT)->EnableWindow(TRUE);

      // Autosize image
      Image.ShinkToFit();
      return TRUE;
   }


   /// <summary>Called when ok.</summary>
   void ExportProjectDialog::OnOK()
   {
      ExportPaths paths;

      try
      {
         // Get data
         UpdateData(TRUE);

         // Require folder [+ filename for zip)
         if (Folder.Empty() || (Option == OPTION_ZIP && FileName.Empty()))
            return;

         // Feedback
         Console << Cons::UserAction << "Exporting project to " << (Option == OPTION_ZIP ? Folder+FileName : Folder) << ENDL;

         // Check folder exists
         if (!Folder.Exists())
         {
            // Query user
            if (theApp.ShowMessage(Cons::Warning, L"The folder does not exist, would you like to create it?", MB_YESNO|MB_ICONQUESTION) == IDNO)
               return;

            // Create folder tree
            CreateFolder(Folder);
         }

         // Store prefs
         PrefsLib.ExportProjectFolder = Folder;
         PrefsLib.ExportProjectFileName = FileName;

         // Export project files
         EnumerateFiles(ProjectDocument::GetActive()->Project.Root, L"", paths);
         ExportFiles(paths);

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

      DDX_Static(pDX, IDC_IMAGE_STATIC, Image);
      DDX_Static(pDX, IDC_TITLE_STATIC, Title);
      DDX_Static(pDX, IDC_HEADING1_STATIC, Location);
      DDX_Static(pDX, IDC_HEADING2_STATIC, Method);
   }

   /// <summary>Generates the output paths for each file in the project</summary>
   /// <param name="root">Item to enumerate, should be project root on first call</param>
   /// <param name="folder">Current folder subpath, should be empty on first call</param>
   /// <param name="files">generated path, should be empty on first call</param>
   void  ExportProjectDialog::EnumerateFiles(ProjectItem& root, Path folder, ExportPaths& files)
   {
      for (auto& item : root.Children)
      {
         switch (item.Type)
         {
         // File: Generate subpath: 'subfolder1\subfolder2\...\filename.ext'
         case ProjectItemType::File:
            files.push_back( ExportPath(item.FullPath, folder+item.FullPath.FileName) );
            break;

         // Folder: Output children into a sub-folder
         case ProjectItemType::Folder:
            // Custom: Use folder name
            if (!item.Fixed)
               EnumerateFiles(item, folder+item.Name, files);

            // Fixed: Use known folder names
            else if (item.Name == L"MSCI Scripts")
               EnumerateFiles(item, folder+L"scripts", files);

            else if (item.Name == L"MD Scripts")
               EnumerateFiles(item, folder+L"director", files);

            else if (item.Name == L"Language Files")
               EnumerateFiles(item, folder+L"t", files);

            else if (item.Name == L"Other Files")
               EnumerateFiles(item, folder+L"misc", files);
            break;
         }
      }
   }

   /// <summary>Exports the files.</summary>
   /// <param name="files">The files.</param>
   void  ExportProjectDialog::ExportFiles(ExportPaths& files)
   {
      list<Path> created;

      try
      {
         // ZIP: Add all files to archive
         if (Option == OPTION_ZIP)
         {
            // Create zip
            ZipFile zip(Folder+FileName);

            // Add files to ZIP
            for (auto& f : files)
            {
               Console << "Archiving " << f.FullPath << " to " << f.SubPath << ENDL;
               zip.Add(f.FullPath, f.SubPath.c_str());
            }

            // Close
            zip.Close();
         }
         else
         {
            // FILES: Copy all files to a folder
            for (auto& f : files)
            {
               Path newPath = Folder+f.SubPath;

               // Exists: Query user how to proceed
               if (newPath.Exists())
               {
                  VString msg(L"The file '%s' already exists, overwrite?", newPath.c_str());
                  switch (theApp.ShowMessage(Cons::Warning, msg, MB_YESNOCANCEL|MB_ICONINFORMATION))
                  {
                  // Skip or Abort
                  case IDNO:     continue;
                  case IDCANCEL: Abort(created); return;
                  }
               }

               // Ensure folder tree exists
               CreateFolder(newPath.Folder);

               // Feedback
               Console << "Copying " << Path(f.FullPath.FileName) << " to " << f.SubPath << ENDL;

               // Attempt to copy file
               if (!CopyFile(f.FullPath.c_str(), newPath.c_str(), FALSE))
               {
                  VString msg(L"Unable to copy '%s' to '%s' : %s. Would you like to continue?", f.FullPath.c_str(), newPath.c_str(), SysErrorString().c_str());
                  if (theApp.ShowMessage(Cons::Warning, msg, MB_YESNO|MB_ICONERROR) == IDNO)
                  {
                     Abort(created);
                     return;
                  }
               }
            }
         }

         // Feedback
         theApp.ShowMessage(Cons::UserAction, L"All files exported successfully", MB_OK|MB_ICONINFORMATION);
      }
      // Unexpected error
      catch (ExceptionBase& )
      {
         for (auto& f : created)
            DeleteFile(f.c_str());
         throw;
      }
   }

   /// <summary>Set state of filename box.</summary>
   void ExportProjectDialog::OnOptionChanged()
   {
      // Get data
      UpdateData(TRUE);

      // Only 'Filename' only for archive mode
      GetDlgItem(IDC_FILENAME_EDIT)->EnableWindow(Option == OPTION_ZIP ? TRUE : FALSE);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   /// <summary>Creates a folder and any intermediate folders.</summary>
   /// <param name="f">The folder.</param>
   void ExportProjectDialog::CreateFolder(const Path& f)
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

   /// <summary>Aborts the operation and deletes created files.</summary>
   /// <param name="created">created files.</param>
   void ExportProjectDialog::Abort(list<Path> created)
   {
      // Cleanup
      for (auto& f : created)
         DeleteFile(f.c_str());

      // Feedback
      theApp.ShowMessage(Cons::Warning, L"Export project aborted", MB_OK|MB_ICONINFORMATION);
   }

   
NAMESPACE_END2(GUI,Windows)


