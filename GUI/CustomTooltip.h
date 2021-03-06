#pragma once
#include "../Logic/Event.h"
#include "../Logic/RichString.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)



   /// <summary>Custom tooltip for the script editor</summary>
   class CustomTooltip : public CToolTipCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Rectangle used for modifying the tooltip rectangle</summary>
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
	   /// <summary>Data used for displaying a tooltip</summary>
      class TooltipData
      {
         // ------------------------ TYPES --------------------------
      protected:
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Set initial icon</summary>
         /// <param name="iconID">The icon identifier.</param>
         /// <param name="iconSize">Size of the icon.</param>
         TooltipData(int iconID, int iconSize) : IconID(iconID), IconSize(iconSize), Cancelled(false)
         {}
         /// <summary>Set icon and text</summary>
         /// <param name="label">RichText label source.</param>
         /// <param name="desc">RichText description source.</param>
         /// <param name="nIcon">Icon resource ID.</param>
         /// <param name="iconSize">Size of the icon.</param>
         TooltipData(wstring label, wstring desc, int nIcon = 0, int iconSize = 0)
            : LabelSource(label), DescriptionSource(desc), IconID(nIcon), IconSize(iconSize), Cancelled(false)
         {}
         virtual ~TooltipData()
         {}
		 
		   DEFAULT_COPY(TooltipData);	// Default copy semantics
		   DEFAULT_MOVE(TooltipData);	// Default move semantics

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Cancel display of tooltip.</summary>
         void  Cancel()
         {
            Cancelled = true;
         }

         /// <summary>Sets data to a sentinel value</summary>
         /// <param name="r">The r.</param>
         void  ResetTo(const TooltipData& r)
         {
            *this = r;
         }

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring  LabelSource, 
                  DescriptionSource;
         int      IconID,
                  IconSize;
         bool     Cancelled;
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      CustomTooltip();    
      virtual ~CustomTooltip();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(CustomTooltip)
      DECLARE_MESSAGE_MAP()
	
   public:
      const static TooltipData UndocumentedCommand,
                               UndocumentedObject;

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   
      // ----------------------- MUTATORS ------------------------
   public:
	   bool Create(CWnd* parent, CWnd* ctrl);
      void Reset();
      void SetTiming(UINT delay, UINT display);

   protected:
      bool   GetTooltipData();
      CPoint GetTooltipPosition(CRect rc);

      void  OnDrawBackground(CDC* dc, CRect wnd);
      CSize OnDrawDescription(CDC* pDC, CRect rect, bool bCalcOnly);
      void  OnDrawIcon(CDC* pDC, CRect rectImage);
	   CSize OnDrawLabel(CDC* pDC, CRect rect, bool bCalcOnly);
      BOOL  OnEraseBkgnd(CDC* pDC);
      void  OnHide(NMHDR *pNMHDR, LRESULT *pResult);
      void  OnPaint();
      void  OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      void  OnShow(NMHDR *pNMHDR, LRESULT *pResult);
      
      // -------------------- REPRESENTATION ---------------------
   public:
      Event<TooltipData*>  RequestData;

   private:
      int           IconID,
                    IconSize;
      RichString    Description,
                    Label;
      bool          HasDescription,
                    Cancelled;

      CRect         rcIcon,      // Drawing rectangle
                    rcLabel, 
                    rcDesc;
public:
   afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};

   /// <summary></summary>
   typedef Event<CustomTooltip::TooltipData*>    TooltipEvent;
   

NAMESPACE_END2(GUI,Controls)
