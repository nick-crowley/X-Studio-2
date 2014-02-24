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
  
   IMPLEMENT_DYNCREATE(CommandTooltip, CMFCToolTipCtrl)

   BEGIN_MESSAGE_MAP(CommandTooltip, CMFCToolTipCtrl)
      ON_NOTIFY_REFLECT(TTN_SHOW, &CommandTooltip::OnShow)
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
   BOOL  CommandTooltip::Create(CWnd* view, CWnd* edit)
   {
      // Create window
      if (!__super::Create(view, 0))
         throw Win32Exception(HERE, L"");

      SetFont(&afxGlobalData.fontRegular);

      // Add tool
      AddTool(edit, L"Title placeholder: Quick brown bear jumped over the lazy fox"); 

      // Set display parameters
      CMFCToolTipInfo params;
      params.m_bBoldLabel = TRUE;
      params.m_bDrawDescription = TRUE;
      params.m_bDrawIcon = TRUE;
      params.m_bRoundedCorners = TRUE;
      params.m_bDrawSeparator = TRUE;
      params.m_nMaxDescrWidth = 800;

      /*params.m_clrFill = RGB(255, 255, 255);
      params.m_clrFillGradient = RGB(228, 228, 240);
      params.m_clrText = RGB(61, 83, 80);
      params.m_clrBorder = RGB(144, 149, 168);*/
      SetParams(&params);

      // Set timings
      SetDelayTime(TTDT_INITIAL, 1500);
      SetDelayTime(TTDT_AUTOPOP, 30*1000);
      SetDelayTime(TTDT_RESHOW, 3000);

      // Activate
      Activate(TRUE);
      return TRUE;
   }

   CSize  CommandTooltip::GetIconSize()
   {
      return CSize(24,24);
   }

   BOOL  CommandTooltip::OnDrawIcon(CDC* pDC, CRect rectImage)
   {
      try
      {
         // Draw random icon
         auto icon = theApp.LoadIconW(IDR_GAME_OBJECTS, 24);
         return pDC->DrawIcon(rectImage.TopLeft(), icon);
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, L"Unable to draw description tooltip icon");
         return FALSE;
      }
   }

	CSize  CommandTooltip::OnDrawLabel(CDC* pDC, CRect rect, BOOL bCalcOnly)
   {
      try
      {
         // Draw/Calculate rectangle
         auto prev = pDC->SelectObject(&afxGlobalData.fontTooltip);
         RichTextRenderer::DrawLines(pDC, rect, Label, (bCalcOnly ? DT_CALCRECT : NULL));
         pDC->SelectObject(prev);

         // CMFCTooltip Fix: Store height of label
         if (bCalcOnly)
            LabelHeight = rect.Height();

         // return size
         return rect.Size();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, L"Unable to draw description tooltip title");
         return CSize();
      }
   }

	CSize  CommandTooltip::OnDrawDescription(CDC* pDC, CRect rect, BOOL bCalcOnly)
   {
      auto prev = pDC->SelectObject(&afxGlobalData.fontTooltip);

      try
      { 
         // Draw/Calculate 
         TooltipRect rc(rect);
         RichTextRenderer::DrawLines(pDC, rc, Description, bCalcOnly ? DT_CALCRECT : NULL);

         // Calculate: Adjust rectangle
         if (!bCalcOnly)
         {
            // Too high: Widen
            while (rc.Ratio < 0.66f)
            {
               rc.right += 50;
               RichTextRenderer::DrawLines(pDC, rc, Description, DT_CALCRECT);
            }
            // Too wide: thin
            while (rc.Ratio > 1.33f)
            {
               rc.right -= 50;
               RichTextRenderer::DrawLines(pDC, rc, Description, DT_CALCRECT);
            }
         }
         
         // return size
         pDC->SelectObject(prev);
         return rc.Size();   
      }
      catch (ExceptionBase& e) 
      {
         Console.Log(HERE, e, L"Unable to draw description tooltip text");
         pDC->SelectObject(prev);
         return rect.Size();
      }
   }
   
   void CommandTooltip::OnShow(NMHDR *pNMHDR, LRESULT *pResult)
   {
      Console << "CommandTooltip::OnShow() received" << ENDL;

      // Request data
      TooltipData data;
      RequestData.Raise(&data);

      // Parse
      Description = StringParser(data.Description).Output;
      Label = StringParser(data.Label).Output;

      // DEBUG:
      Console << Cons::Yellow << data.Description << ENDL << ENDL;
      Console << Cons::Green << Description << ENDL << ENDL;

      // Set description placeholder
      SetDescription(L"Description placeholder: Quick brown bear jumped over the lazy fox");

      // Set size?
      //SetFixedWidth(400, 600);

      // Show tooltip
      __super::OnShow(pNMHDR, pResult);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)

