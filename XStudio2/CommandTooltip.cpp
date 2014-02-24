#include "stdafx.h"
#include "CommandTooltip.h"
#include "RichTextRenderer.h"
#include "Logic/StringParser.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Sentinel for displaying no tooltip</summary>
   const CommandTooltip::TooltipData  CommandTooltip::NoTooltip(L"Nothing", L"Nothing", 1);

   /// <summary>Undocumented command tooltip</summary>
   const CommandTooltip::TooltipData  CommandTooltip::UndocumentedTooltip(L"Command has no description", L"", 1);

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(CommandTooltip, CToolTipCtrl)

   BEGIN_MESSAGE_MAP(CommandTooltip, CToolTipCtrl)
      ON_NOTIFY_REFLECT(TTN_SHOW, &CommandTooltip::OnShow)
      ON_WM_PAINT()
      ON_WM_ERASEBKGND()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   CommandTooltip::CommandTooltip()
   {
   }

   CommandTooltip::~CommandTooltip()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Creates the tooltip</summary>
   /// <param name="pParentWnd">The parent WND.</param>
   /// <param name="dwStyle">The style.</param>
   /// <returns></returns>
   bool  CommandTooltip::Create(CWnd* view, CWnd* edit)
   {
      // Create window
      if (!__super::Create(view, 0))
         throw Win32Exception(HERE, L"");

      //SetFont(&afxGlobalData.fontRegular);

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
   
   CSize  CommandTooltip::GetIconSize()
   {
      return CSize(24,24);
   }
   
   void CommandTooltip::GetTooltipData()
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
   
   void CommandTooltip::OnDrawBackground(CDC* dc, CRect wnd)
   {
      //CDrawingManager dm(*dc);
		//dm.FillGradient2(wnd, ::GetSysColor(COLOR_INFOTEXT), ::GetSysColor(COLOR_INFOTEXT), 90);

      //dc->FillRect(wnd, 
      dc->FillSolidRect(wnd, ::GetSysColor(COLOR_INFOBK));
   }

	CSize  CommandTooltip::OnDrawDescription(CDC* pDC, CRect rect, bool bCalcOnly)
   {
      TooltipRect rc(rect);

      // DEBUG
      Console << "Desc: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rc << ENDL;


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

      // DEBUG
      Console << "Desc: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rc << ENDL;
         
      // return size
      return rc.Size();   
   }
   
   void  CommandTooltip::OnDrawIcon(CDC* pDC, CRect rectImage)
   {
      // DEBUG
      Console << "Icon: " << rectImage << ENDL;

      // Draw random icon
      auto icon = theApp.LoadIconW(IDR_GAME_OBJECTS, 24);
      pDC->DrawIcon(rectImage.TopLeft(), icon);
   }

	CSize  CommandTooltip::OnDrawLabel(CDC* pDC, CRect rect, bool bCalcOnly)
   {
      // DEBUG
      Console << "Label: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rect << ENDL;

      // Draw/Calculate rectangle
      RichTextRenderer::DrawLines(pDC, rect, Label, (bCalcOnly ? DT_CALCRECT : NULL));

      // DEBUG
      Console << "Label: " << (bCalcOnly ? Cons::Yellow : Cons::Green) << rect << ENDL;

      // return size
      return rect.Size();
   }
   
   BOOL CommandTooltip::OnEraseBkgnd(CDC* pDC)
   {
      OnDrawBackground(pDC, ClientRect(this));

      return TRUE;
   }

   void CommandTooltip::OnShow(NMHDR *pNMHDR, LRESULT *pResult)
   {
      static const UINT  MOVED = 1, UNMOVED = 0;

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
         wnd.right = desc.cx;

         // get label rect
         auto labelRect = CRect(GetIconSize().cx, 0, wnd.right, GetIconSize().cy);

         // Ensure enough height for label/icon 
         auto label = OnDrawLabel(&dc, labelRect, true);
         wnd.bottom = max(GetIconSize().cy, label.cy) + desc.cy;

         // TODO: Adjust for margins

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
   


   void CommandTooltip::OnPaint()
   {
      CPaintDC dc(this); 
      ClientRect wnd(this);

      // Prepare
      auto font = dc.SelectObject(&afxGlobalData.fontTooltip);
      dc.SetBkMode(TRANSPARENT);

      try
      {
         // Background
         //OnDrawBackground(&dc, wnd);

         // Icon
         CRect icon(CPoint(0,0), GetIconSize());
         OnDrawIcon(&dc, icon);

         // Label
         CRect label(icon.right, 0, wnd.right, icon.bottom);
         auto height = OnDrawLabel(&dc, label, false).cy;

         // Description
         CRect desc(0, max(height, icon.Height()), wnd.right, wnd.bottom);
         OnDrawDescription(&dc, desc, false);
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

