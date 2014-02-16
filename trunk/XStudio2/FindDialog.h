#pragma once
#include "afxwin.h"
#include "SearchOperation.h"

/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Window)
   
      /// <summary>Find text dialog</summary>
      class FindDialog : public CDialogEx
      {
         // ------------------------ TYPES --------------------------
      public:
         enum { IDD = IDD_FINDREPLACE };

      private:
         
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
         SearchTarget GetSearchTarget() const;

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
         afx_msg void OnTarget_TextChanged();

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
         CComboBox FindCombo;
         CComboBox ReplaceCombo;
         CComboBox TargetCombo;
         
         SearchOperation Search;
      };

/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Window)
