
#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /////////////////////////////////////////////////////////////////////////////
   // CFileTreeCtrl window

   class CFileTreeCtrl : public CTreeCtrl
   {
   // Construction
   public:
	   CFileTreeCtrl();

   // Overrides
   protected:
	   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

   // Implementation
   public:
	   virtual ~CFileTreeCtrl();

   protected:
	   DECLARE_MESSAGE_MAP()
   };
   
NAMESPACE_END2(GUI,Controls)

