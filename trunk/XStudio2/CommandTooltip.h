
#pragma once

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class CommandTooltip : public CMFCToolTipCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      CommandTooltip();    
      virtual ~CommandTooltip();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(CommandTooltip)
   protected:
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   
      // ----------------------- MUTATORS ------------------------
   public:
	   BOOL Create(CWnd* view, CWnd* edit);

      CSize GetIconSize() override;
      BOOL  OnDrawIcon(CDC* pDC, CRect rectImage) override;
	   CSize OnDrawLabel(CDC* pDC, CRect rect, BOOL bCalcOnly) override;
	   CSize OnDrawDescription(CDC* pDC, CRect rect, BOOL bCalcOnly) override;

   protected:

      // -------------------- REPRESENTATION ---------------------
      
   private:
   public:
      afx_msg void OnShow(NMHDR *pNMHDR, LRESULT *pResult);
   };
   

NAMESPACE_END2(GUI,Controls)
