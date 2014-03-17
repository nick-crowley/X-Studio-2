#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>About dialog</summary>
   class CAboutDlg : public CDialogEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_ABOUTBOX };

      // --------------------- CONSTRUCTION ----------------------
   public:
	   CAboutDlg();

	   // ------------------------ STATIC -------------------------
   protected:
      DECLARE_MESSAGE_MAP()

      // ---------------------- ACCESSORS ------------------------	

      // ----------------------- MUTATORS ------------------------
   protected:
	   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   };

NAMESPACE_END2(GUI,Windows)

