#pragma once

/// <summary>User interface windows</summary>
NAMESPACE_BEGIN2(GUI,Window)
   
      /// <summary>Find text dialog</summary>
      class FindDialog : public CDialogEx
      {
         // ------------------------ TYPES --------------------------
      public:
         enum { IDD = IDD_FINDREPLACE };

         // --------------------- CONSTRUCTION ----------------------
      public:
	      FindDialog(CWnd* pParent = NULL);   // standard constructor
	      virtual ~FindDialog();

         // ------------------------ STATIC -------------------------

         DECLARE_DYNAMIC(FindDialog)
         DECLARE_MESSAGE_MAP()

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      protected:
	      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

         // -------------------- REPRESENTATION ---------------------
      
      };

/// <summary>User interface windows</summary>
NAMESPACE_END2(GUI,Window)