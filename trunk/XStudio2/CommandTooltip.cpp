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
         // DEBUG:
         //Console << "OnDrawIcon: " << Cons::Yellow << "rectImage=" << rectImage << ENDL;

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
         // FIX: Preserve drawing rect. Set correct height
         if (!bCalcOnly)
         {
            DrawRect = rect;
            rect.bottom = rect.top + LabelHeight;
         }

         // DEBUG:
         //Console << "OnDrawLabel: " << Cons::Yellow << rect << ENDL;

         // Draw/Calculate rectangle
         auto prev = pDC->SelectObject(&afxGlobalData.fontRegular);
         //pDC->DrawText(Data.Label.c_str(), rect, DT_LEFT | DT_WORDBREAK | (bCalcOnly ? DT_CALCRECT : NULL));
         RichTextRenderer::DrawLines(pDC, rect, Label, (bCalcOnly ? DT_CALCRECT : NULL));
         pDC->SelectObject(prev);

         // FIX: Store height of label
         if (bCalcOnly)
            LabelHeight = rect.Height();

         // DEBUG:
         /*if (bCalcOnly)
            Console << "OnDrawLabel: " << Cons::Green << rect << ENDL;*/
      
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
      try
      { 
         // FIX: Use correct drawing rect. Set correct height
         if (!bCalcOnly)
         {
            rect = DrawRect;
            rect.top += LabelHeight + 6;  // +6 for separator
         }

         //Console << "OnDrawDescription: " << Cons::Yellow << rect << ENDL;
      
         // Draw/Calculate rectangle
         auto prev = pDC->SelectObject(&afxGlobalData.fontRegular);
         //pDC->DrawText(Data.Description.c_str(), rect, DT_LEFT | DT_WORDBREAK | (bCalcOnly ? DT_CALCRECT : NULL));
         RichTextRenderer::DrawLines(pDC, rect, Description, (bCalcOnly ? DT_CALCRECT : NULL));
         pDC->SelectObject(prev);

         // DEBUG:
         /*if (bCalcOnly)
            Console << "OnDrawDescription: " << Cons::Green << rect << ENDL;*/
      
         // return size
         return rect.Size();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, L"Unable to draw description tooltip text");
         return CSize();
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

