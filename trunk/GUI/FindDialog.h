#pragma once
#include "afxwin.h"
#include "SearchOperation.h"
#include "FindComboBox.h"

/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
      /// <summary>Find text dialog</summary>
      class FindDialog : public CDialogEx
      {
         // ------------------------ TYPES --------------------------
      public:
         enum { IDD = IDD_FINDREPLACE };

      private:
         typedef unique_ptr<SearchOperation> OperationPtr;
         
         // --------------------- CONSTRUCTION ----------------------
      public:
	      FindDialog(CWnd* pParent = NULL);   // standard constructor
	      virtual ~FindDialog();

         // ------------------------ STATIC -------------------------

         DECLARE_DYNAMIC(FindDialog)
         DECLARE_MESSAGE_MAP()

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      protected:
         Operation    GetOutputPane() const;
         wstring      GetReplaceTerm() const;
         SearchTarget GetSearchTarget() const;
         wstring      GetSearchTerm() const;

         // ----------------------- MUTATORS ------------------------
      protected:
         void Expand(bool expand);

	      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
         virtual BOOL OnInitDialog();
         
         afx_msg void OnFind_Click();
         afx_msg void OnFind_TextChanged();
         afx_msg void OnFindAll_Click();
         afx_msg void OnOptions_Click();
         afx_msg void OnReplace_Click();
         afx_msg void OnReplace_TextChanged();
         afx_msg void OnReplaceAll_Click();
         afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
         afx_msg void OnTarget_TextChanged();
         afx_msg void OnOptions_Changed(UINT nID);

      private:
         void Feedback(const wstring& operation);
         void NewSearch(SearchCommand cmd);
         void Reset();
         void SetState(bool active);
         bool ValidateState(SearchCommand cmd);

         // -------------------- REPRESENTATION ---------------------
      protected:
         bool Expanded;
         BOOL UseRegEx;
         BOOL MatchCase;
         BOOL MatchWholeWord;
         CButton FindButton;
         CButton FindAllButton;
         CButton OptionsButton;
         CButton ReplaceButton;
         CButton ReplaceAllButton;
         FindComboBox FindCombo;
         FindComboBox ReplaceCombo;
         CComboBox TargetCombo;
         
         OperationPtr  Search;
      public:
         afx_msg void OnClose();
         afx_msg void OnDestroy();
      };

/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Windows)
