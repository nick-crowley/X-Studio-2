// LanguageDocument.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDocument.h"
#include "LanguageFrame.h"
#include "LanguageEditView.h"
#include "PropertiesWnd.h"
#include "Logic/FileStream.h"
#include "Logic/LanguageFileReader.h"
#include "Logic/FileIdentifier.h"
#include "Logic/WorkerFeedback.h"
#include "Logic/StringLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   // ---------------------------------- TEMPLATE ----------------------------------

   IMPLEMENT_DYNAMIC(LanguageDocTemplate, CMultiDocTemplate)

   /// <summary>Creates language document template</summary>
   LanguageDocTemplate::LanguageDocTemplate() 
      : CMultiDocTemplate(IDR_LANGUAGEVIEW, RUNTIME_CLASS(LanguageDocument), RUNTIME_CLASS(LanguageFrame), nullptr)
   {}

   /// <summary>Queries whether an external file should be opened as a language file</summary>
   /// <param name="lpszPathName">Path of file.</param>
   /// <param name="rpDocMatch">The already open document, if any.</param>
   /// <returns>yesAlreadyOpen if already open, yesAttemptNative if language file, noAttempt if unrecognised</returns>
   CDocTemplate::Confidence LanguageDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)
   {
      Confidence conf;

      // Ensure document not already open
      if ((conf = CMultiDocTemplate::MatchDocType(lpszPathName, rpDocMatch)) == yesAlreadyOpen)
         return yesAlreadyOpen;

      // Identify language file from header
      rpDocMatch = nullptr;
      return GuiString(L"String Library") == lpszPathName || FileIdentifier::Identify(lpszPathName) == FileType::Language ? yesAttemptNative : noAttempt;
   }

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageDocument, DocumentBase)
   
   BEGIN_MESSAGE_MAP(LanguageDocument, DocumentBase)
      ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_SAVE, ID_FILE_SAVE_AS, &LanguageDocument::OnQueryFileCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &LanguageDocument::OnQueryClipboardCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &LanguageDocument::OnQueryClipboardCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_UNDO, ID_EDIT_REDO, &LanguageDocument::OnQueryClipboardCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_COPY, ID_EDIT_CUT, &LanguageDocument::OnQueryClipboardCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_BOLD, ID_EDIT_ADD_BUTTON, &LanguageDocument::OnQueryFormatCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageDocument::OnQueryModeCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageDocument::LanguageDocument() : DocumentBase(DocumentType::Language), CurrentString(nullptr), CurrentPage(nullptr), Virtual(false)
   {
   }

   LanguageDocument::~LanguageDocument()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   
   /// <summary>Retrieves the file/library page collection</summary>
   /// <returns></returns>
   LanguageDocument::PageCollection&   LanguageDocument::GetContent() 
   {
      return Virtual ? Library : Content.Pages;
   }


   /// <summary>Gets the selected page.</summary>
   /// <returns></returns>
   LanguagePage*   LanguageDocument::GetSelectedPage() const
   {
      return CurrentPage;
   }

   /// <summary>Gets the selected string.</summary>
   /// <returns></returns>
   LanguageString* LanguageDocument::GetSelectedString() const
   {
      return CurrentString;
   }
   
   /// <summary>Populates the properties window</summary>
   /// <param name="grid">The grid.</param>
   void  LanguageDocument::OnDisplayProperties(CMFCPropertyGridCtrl& grid)
   {
      // Group: ID/Language
      CMFCPropertyGridProperty* group = new CMFCPropertyGridProperty(Virtual ? L"Library" : L"File");
      if (!Virtual)
         group->AddSubItem(new LanguageDocument::IDProperty(*this));
      group->AddSubItem(new LanguageDocument::GameLanguageProperty(*this));
      grid.AddProperty(group);

      // Group: PageID/Title/Description/Voiced
      if (SelectedPage)
      {
         CMFCPropertyGridProperty* group = new CMFCPropertyGridProperty(_T("Page"));
         group->AddSubItem(new LanguagePageView::IDProperty(*this, *SelectedPage));
         group->AddSubItem(new LanguagePageView::TitleProperty(*this, *SelectedPage));
         group->AddSubItem(new LanguagePageView::DescriptionProperty(*this, *SelectedPage));
         group->AddSubItem(new LanguagePageView::VoicedProperty(*this, *SelectedPage));
         grid.AddProperty(group);
      }

      // Group: StringID/ColourTag/Version
      if (SelectedString)
      {
         CMFCPropertyGridProperty* group = new CMFCPropertyGridProperty(_T("String"));
         group->AddSubItem(new LanguageStringView::IDProperty(*this, *SelectedPage, *SelectedString));
         group->AddSubItem(new LanguageStringView::ColourTagProperty(*this, *SelectedString));
         group->AddSubItem(new LanguageStringView::VersionProperty(*this, *SelectedString));
         grid.AddProperty(group);
      }

      // Group: Library files
      if (Virtual)
      {
         group = new CMFCPropertyGridProperty(_T("Components"));
      
         // Enumerate files [backwards]
         for (auto file = StringLib.Files.crbegin(); file != StringLib.Files.crend(); ++file)
            group->AddSubItem(new FileNameProperty(*this, GuiString(L"ID=%d", file->ID)));
         grid.AddProperty(group);
      }
   }
   
   /// <summary>Called when document closed.</summary>
   void LanguageDocument::OnCloseDocument()
   {
      // Disconnect properties
      CPropertiesWnd::Connect(this, false);

      __super::OnCloseDocument();
   }

   /// <summary>Initializes new document</summary>
   /// <returns></returns>
   BOOL LanguageDocument::OnNewDocument()
   {
	   if (!DocumentBase::OnNewDocument())
		   return FALSE;
	   return TRUE;
   }

   /// <summary>Populates from a file path</summary>
   /// <param name="szPathName">Full path, or 'String Library'</param>
   /// <returns></returns>
   BOOL LanguageDocument::OnOpenDocument(LPCTSTR szPathName)
   {
      WorkerData data(Operation::LoadSaveDocument);

      try
      {
         // Feedback
         Console << Cons::UserAction << L"Loading language file: " << Path(szPathName) << ENDL;
         data.SendFeedback(ProgressType::Operation, 0, GuiString(L"Loading language file '%s'", szPathName));

         // Identify whether file or library
         Virtual = GuiString(L"String Library") == szPathName;
         
         // Library:
         if (Virtual)
         {
            // Copy each page (and strings) into from string library into static snapshot
            for (auto& file : StringLib.Files)
               for (auto& page : file)
                  Library.Add(page);

            // Set language (for display purposes only) from main language file
            if (!StringLib.Files.empty())
               Content.Language = StringLib.Files.begin()->Language;
         }
         else
         {  
            // Parse input file
            StreamPtr fs2( new FileStream(szPathName, FileMode::OpenExisting, FileAccess::Read) );
            Content = LanguageFileReader(fs2).ReadFile(Path(szPathName).FileName);
         }

         data.SendFeedback(Cons::Green, ProgressType::Succcess, 0, L"Language file loaded successfully");
         return TRUE;
      }
      catch (ExceptionBase&  e)
      {
         // Feedback/Display error
         data.SendFeedback(ProgressType::Failure, 0, L"Failed to load language file");
         theApp.ShowError(HERE, e, GuiString(L"Failed to load language file '%s'", szPathName));
         return FALSE;
      }
   }

   /// <summary>Sets the selected page and raises PAGE SELECTION CHANGED.</summary>
   /// <param name="p">The page.</param>
   void  LanguageDocument::SetSelectedPage(LanguagePage* p)
   {
      CurrentPage = p;
      CPropertiesWnd::Connect(this, true);
      PageSelectionChanged.Raise();
   }

   /// <summary>Sets the selected string and raises STRING SELECTION CHANGED.</summary>
   /// <param name="s">The string.</param>
   void  LanguageDocument::SetSelectedString(LanguageString* s)
   {
      CurrentString = s;
      CPropertiesWnd::Connect(this, true);
      StringSelectionChanged.Raise();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Query state of save commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageDocument::OnQueryFileCommand(CCmdUI* pCmd)
   {
      pCmd->SetCheck(FALSE);

      switch (pCmd->m_nID)
      {
      // Save/SaveAs: Require file document
      case ID_FILE_SAVE:
      case ID_FILE_SAVE_AS:
         pCmd->Enable(!Virtual ? TRUE : FALSE);
         break;
      }
   }

   /// <summary>Query state of clipboard commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageDocument::OnQueryClipboardCommand(CCmdUI* pCmd)
   {
      // Delegate to edit view
      GetView<LanguageEditView>()->OnQueryClipboardCommand(pCmd);
   }

   /// <summary>Query state of editor formatting commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageDocument::OnQueryFormatCommand(CCmdUI* pCmd)
   {
      // Delegate to edit view
      GetView<LanguageEditView>()->OnQueryFormatCommand(pCmd);
   }

   /// <summary>Query state of editor mode commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageDocument::OnQueryModeCommand(CCmdUI* pCmd)
   {
      // Delegate to edit view
      GetView<LanguageEditView>()->OnQueryModeCommand(pCmd);
   }


   // ------------------------------- PRIVATE METHODS ------------------------------
   
NAMESPACE_END2(GUI,Documents)


