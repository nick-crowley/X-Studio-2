#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)
   
   /// <summary>Base class for all MDI child frames</summary>
   class FrameWndBase : public CMDIChildWndEx
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      FrameWndBase(); 
      virtual ~FrameWndBase();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(FrameWndBase)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
	  
      afx_msg void OnDropFiles(HDROP hDropInfo);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   private:
   };
   

NAMESPACE_END2(GUI,Views)
