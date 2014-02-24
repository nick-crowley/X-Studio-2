#pragma once
#include "Logic/Event.h"
#include "Logic/RichString.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /*class ScriptEditTooltip;
   class ScriptEditTooltip::TooltipData;*/
   
   
   

   /// <summary></summary>
   class ScriptEditTooltip : public CToolTipCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary></summary>
      class TooltipRect : public CRect
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         TooltipRect(const CRect& r) : CRect(r)
         {}
         
         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(float,Ratio,GetRatio);
         
         // ---------------------- ACCESSORS ------------------------			
      public:
         float GetRatio() const
         {
            return (float)Height() / (float)Width();
         }
      };

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
         TooltipData(wstring lab, wstring desc, int icon) : Label(lab), Description(desc), Icon(icon)
         {}
         virtual ~TooltipData()
         {}
		 
		   DEFAULT_COPY(TooltipData);	// Default copy semantics
		   DEFAULT_MOVE(TooltipData);	// Default move semantics

         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Clears this instance.</summary>
         void  Clear()
         {
            Label = L"No further information";
            Description.clear();
         }
         // -------------------- REPRESENTATION ---------------------
      public:
         wstring  Label, 
                  Description;
         int      Icon;
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      ScriptEditTooltip();    
      virtual ~ScriptEditTooltip();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(ScriptEditTooltip)
      DECLARE_MESSAGE_MAP()
	
   public:
      const static TooltipData NoTooltip,
                               UndocumentedTooltip;

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   
      // ----------------------- MUTATORS ------------------------
   public:
	   bool Create(CWnd* view, CWnd* edit);

   protected:
      CSize GetIconSize();
      void  GetTooltipData();

      void  OnDrawBackground(CDC* dc, CRect wnd);
      CSize OnDrawDescription(CDC* pDC, CRect rect, bool bCalcOnly);
      void  OnDrawIcon(CDC* pDC, CRect rectImage);
	   CSize OnDrawLabel(CDC* pDC, CRect rect, bool bCalcOnly);
      BOOL  OnEraseBkgnd(CDC* pDC);
      void  OnPaint();
      void  OnShow(NMHDR *pNMHDR, LRESULT *pResult);
      
      // -------------------- REPRESENTATION ---------------------
   public:
      Event<TooltipData*>  RequestData;

   private:
      RichString    Description,
                    Label;
      CRect  rcIcon, rcLabel, rcDesc;
};

   /// <summary></summary>
   typedef Event<ScriptEditTooltip::TooltipData*>    TooltipEvent;
   

NAMESPACE_END2(GUI,Controls)
