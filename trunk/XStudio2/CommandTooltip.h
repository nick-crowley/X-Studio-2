#pragma once
#include "Logic/Event.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /*class CommandTooltip;
   class CommandTooltip::TooltipData;*/
   
   
   

   /// <summary></summary>
   class CommandTooltip : public CMFCToolTipCtrl
   {
      // ------------------------ TYPES --------------------------
   public:
	   /// <summary></summary>
      class TooltipData
      {
         // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------

      public:
         TooltipData()
         {}
         virtual ~TooltipData()
         {}
		 
		   DEFAULT_COPY(TooltipData);	// Default copy semantics
		   DEFAULT_MOVE(TooltipData);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring  Label, 
                  Description;
         int      Icon;
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      CommandTooltip();    
      virtual ~CommandTooltip();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(CommandTooltip)
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
      void  OnShow(NMHDR *pNMHDR, LRESULT *pResult);

   protected:

      // -------------------- REPRESENTATION ---------------------
   public:
      Event<TooltipData*>  RequestData;

   private:
      TooltipData   Data;
      UINT          LabelHeight;
      CRect         DrawRect;
   };

   /// <summary></summary>
   typedef Event<CommandTooltip::TooltipData*>    TooltipEvent;
   

NAMESPACE_END2(GUI,Controls)
