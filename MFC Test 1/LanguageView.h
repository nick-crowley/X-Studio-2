#pragma once


/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // LanguageView form view

   class LanguageView : public CFormView
   {
	   DECLARE_DYNCREATE(LanguageView)

   protected:
	   LanguageView();           // protected constructor used by dynamic creation
	   virtual ~LanguageView();

   public:
	   enum { IDD = IDD_LANGUAGEVIEW };
   #ifdef _DEBUG
	   virtual void AssertValid() const;
   #ifndef _WIN32_WCE
	   virtual void Dump(CDumpContext& dc) const;
   #endif
   #endif

   protected:
	   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	   DECLARE_MESSAGE_MAP()
   };


NAMESPACE_END(GUI)

