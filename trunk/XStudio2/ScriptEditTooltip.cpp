#include "stdafx.h"
#include "ScriptEditTooltip.h"
#include "RichTextRenderer.h"
#include "Logic/StringParser.h"

/// <summary>Print debugging stuff</summary>
//#define PRINT_CONSOLE

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Sentinel for displaying no tooltip</summary>
   const ScriptEditTooltip::TooltipData  ScriptEditTooltip::NoTooltip(L"Nothing", L"Nothing", 1);

   /// <summary>Undocumented command tooltip</summary>
   const ScriptEditTooltip::TooltipData  ScriptEditTooltip::UndocumentedTooltip(L"Command has no description", L"", 1);

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptEditTooltip, CToolTipCtrl)

   BEGIN_MESSAGE_MAP(ScriptEditTooltip, CToolTipCtrl)
      ON_NOTIFY_REFLECT(TTN_SHOW, &ScriptEditTooltip::OnShow)
      ON_WM_PAINT()
      ON_WM_ERASEBKGND()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptEditTooltip::ScriptEditTooltip()
   {
   }

   ScriptEditTooltip::~ScriptEditTooltip()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Creates the tooltip</summary>
   /// <param name="pParentWnd">The parent WND.</param>
   /// <param name="dwStyle">The style.</param>
   /// <returns></returns>
   bool  ScriptEditTooltip::Create(CWnd* view, CWnd* edit)
   {
      // Create window
      if (!__super::Create(view, 0))
         throw Win32Exception(HERE, L"");

      // Add tool
      AddTool(edit, L"Title placeholder: Quick brown bear jumped over the lazy fox"); 

      // Set timings
      SetDelayTime(TTDT_INITIAL, 1500);
      SetDelayTime(TTDT_AUTOPOP, 30*1000);
      SetDelayTime(TTDT_RESHOW, 3000);

      // Activate
      Activate(TRUE);
      return TRUE;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Gets the icon size.</summary>
   /// <returns></returns>
   CSize  ScriptEditTooltip::GetIconSize()
   {
      return CSize(24,24);
   }
   
   /// <summary>Gets the tooltip data from the parent</summary>
   void ScriptEditTooltip::GetTooltipData()
   {
      // Request data
      TooltipData data;
      RequestData.Raise(&data);

      // Parse
      Description = StringParser(data.Description).Output;
      Label = StringParser(data.Label).Output;

      // DEBUG:
      //Console << Cons::Yellow << data.Description << ENDL << ENDL;
      //Console << Cons::Green << Description << ENDL << ENDL;
   }
   
   /// <summary>Draws the background</summary>
   /// <param name="pDC">The dc.</param>
   /// <param name="rect">The rect.</param>
   void ScriptEditTooltip::OnDrawBackground(CDC* dc, CRect wnd)
   {
      if (HTHEME theme = OpenThemeData(m_hWnd, L"TOOLTIP"))
      {
         DrawThemeBackground(theme, dc->m_hDC, TTP_STANDARD, TTSS_NORMAL, wnd, nullptr);
         CloseThemeData(theme);
      }
      else
         dc->FillSolidRect(wnd, ::GetSysColor(COLOR_INFOBK));
   }


   /// <summary>Draws the description</summary>
   /// <param name="pDC">The dc.</param>
   /// <param name="rect">The rect.</param>
   /// <param name="bCalcOnly">calculate only.</param>
   /// <returns>Description size</returns>
	CSize  ScriptEditTooltip::OnDrawDescription(CDC* pDC, CRect rect, bool bCalcOnly)
   {
      TooltipRect rc(rect);

#ifdef PRINT_CONSOLE
      Console << "Desc: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rc << ENDL;
#endif

      // Draw/Calculate 
      RichTextRenderer::DrawLines(pDC, rc, Description, bCalcOnly ? DT_CALCRECT : NULL);

      // Calculate: Adjust rectangle
      if (bCalcOnly)
      {
         // Too high: Widen
         while (rc.Ratio > 0.8f)
         {
            rc.right += 100;
            RichTextRenderer::DrawLines(pDC, rc, Description, DT_CALCRECT);
         }
      }

#ifdef PRINT_CONSOLE
      Console << "Desc: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rc << ENDL;
#endif
         
      // return size
      return rc.Size();   
   }
   
   /// <summary>Draws the icon</summary>
   /// <param name="pDC">The dc.</param>
   /// <param name="rect">The rect.</param>
   void  ScriptEditTooltip::OnDrawIcon(CDC* pDC, CRect rectImage)
   {
#ifdef PRINT_CONSOLE
      Console << "Icon: " << rectImage << ENDL;
#endif

      // Draw random icon
      CImageList il;
      il.Create(24, 24, ILC_COLOR24|ILC_MASK, 1, 1);
      il.Add(theApp.LoadIconW(IDR_GAME_OBJECTS, 24));
      il.Draw(pDC, 0, rectImage.TopLeft(), ILD_TRANSPARENT);

      /*auto icon = theApp.LoadIconW(IDR_GAME_OBJECTS, 24);
      pDC->DrawIcon(rectImage.TopLeft(), icon);*/
      
   }

   /// <summary>Draws the label</summary>
   /// <param name="pDC">The dc.</param>
   /// <param name="rect">The rect.</param>
   /// <param name="bCalcOnly">calculate only.</param>
   /// <returns>Label size</returns>
   CSize  ScriptEditTooltip::OnDrawLabel(CDC* pDC, CRect rect, bool bCalcOnly)
   {
#ifdef PRINT_CONSOLE
      Console << "Label: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rect << ENDL;
#endif

      // Draw/Calculate rectangle
      RichTextRenderer::DrawLines(pDC, rect, Label, (bCalcOnly ? DT_CALCRECT : NULL));

#ifdef PRINT_CONSOLE
      Console << "Label: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rect << ENDL;
#endif

      // return size
      return rect.Size();
   }
   
   /// <summary>Erases the background.</summary>
   /// <param name="pDC">The dc.</param>
   /// <returns></returns>
   BOOL ScriptEditTooltip::OnEraseBkgnd(CDC* pDC)
   {
      OnDrawBackground(pDC, ClientRect(this));

      return TRUE;
   }

   /// <summary>Resizes the tooltip accordingly</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void ScriptEditTooltip::OnShow(NMHDR *pNMHDR, LRESULT *pResult)
   {
      static const UINT  MOVED = 1, UNMOVED = 0, MARGIN = 4;

      ClientRect wnd(this);
      CClientDC  dc(this);

      // Prepare
      auto font = dc.SelectObject(&afxGlobalData.fontTooltip);

      try
      {
         // Request tooltip data
         GetTooltipData();

         // Set window width according to description 
         auto desc = OnDrawDescription(&dc, wnd, true);

         // Set header height according to icon/label height
         auto label = CRect(GetIconSize().cx, 0, desc.cx, GetIconSize().cy);
         auto header = OnDrawLabel(&dc, label, true);
         header.cy = max(GetIconSize().cy, header.cy);

         // Set window dimensions
         wnd.right = desc.cx;
         wnd.bottom = header.cy + desc.cy;

         // Set drawing rectangles
         rcIcon = CRect(CPoint(0, 0), GetIconSize());
         rcLabel = label;
         rcDesc = CRect(CPoint(0, GetIconSize().cy), desc);

         // Set margins
         wnd.InflateRect(2*MARGIN, 2*MARGIN);
         for (CRect* r : {&rcIcon,&rcLabel,&rcDesc})
            r->OffsetRect(MARGIN,MARGIN);

         // Set internal margins
         rcLabel.OffsetRect(MARGIN,0);
         rcDesc.OffsetRect(0,MARGIN);

         // Size window
         SetWindowPos(nullptr, -1, -1, wnd.Width(), wnd.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
         *pResult = MOVED;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         *pResult = UNMOVED;
      }

      // Cleanup
      dc.SelectObject(font);
   }
   


   /// <summary>Paints the tooltip</summary>
   void ScriptEditTooltip::OnPaint()
   {
      CPaintDC dc(this); 
      ClientRect wnd(this);

      // Prepare
      auto font = dc.SelectObject(&afxGlobalData.fontTooltip);
      dc.SetBkMode(TRANSPARENT);

      try
      {
         OnDrawIcon(&dc, rcIcon);
         OnDrawLabel(&dc, rcLabel, false);
         OnDrawDescription(&dc, rcDesc, false);
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      // Cleanup
      dc.SetBkMode(OPAQUE);
      dc.SelectObject(font);
   }

   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)

