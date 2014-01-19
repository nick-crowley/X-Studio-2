
#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   /////////////////////////////////////////////////////////////////////////////
   // CViewTree window

   class CViewTree : public CTreeCtrl
   {
   // Construction
   public:
	   CViewTree();

   // Overrides
   protected:
	   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

   // Implementation
   public:
	   virtual ~CViewTree();

   protected:
	   DECLARE_MESSAGE_MAP()
   };
   
NAMESPACE_END(GUI)

