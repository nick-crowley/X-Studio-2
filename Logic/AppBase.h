#pragma once
#include <afxwinappex.h>

namespace Logic
{
   /// <summary></summary>
   class LogicExport AppBase : public CWinAppEx
   {
      // ------------------------ TYPES --------------------------
   protected:
	  
      // --------------------- CONSTRUCTION ----------------------

   public:
      AppBase();
      virtual ~AppBase();
		 
		NO_COPY(AppBase);	// Default copy semantics
		NO_MOVE(AppBase);	// Default move semantics

      // ------------------------ STATIC -------------------------
      DECLARE_MESSAGE_MAP()
   
   protected:
      static void   OnCriticalError();

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      BOOL  ShowError(const GuiString& src, const exception& e, const GuiString& msg) const;
      BOOL  ShowError(const GuiString& src, const ExceptionBase& e, const GuiString& msg) const;
      BOOL  ShowError(const GuiString& src, const ExceptionBase& e) const;
      BOOL  ShowMessage(const wstring& msg, UINT flags = MB_OK) const;
      BOOL  ShowMessage(Cons c, const wstring& msg, UINT flags = MB_OK) const;

      // ----------------------- MUTATORS ------------------------
   public:
      int   ExitInstance() override;
	   BOOL  InitInstance() override;

      // -------------------- REPRESENTATION ---------------------
   protected:
      HINSTANCE   ResourceLibrary;
   };

   inline AppBase* GetAppBase()
   {
      return (AppBase*)AfxGetApp();
   }
}

using namespace Logic;
