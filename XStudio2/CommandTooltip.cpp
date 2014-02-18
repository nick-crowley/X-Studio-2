#include "stdafx.h"
#include "CommandTooltip.h"
#include "Logic/DescriptionLibrary.h"
#include "Logic/SyntaxLibrary.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(CommandTooltip, CMFCToolTipCtrl)

   BEGIN_MESSAGE_MAP(CommandTooltip, CMFCToolTipCtrl)
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
   
   CSize  CommandTooltip::GetIconSize()
   {
      return CSize(24,24);
   }

   BOOL  CommandTooltip::OnDrawIcon(CDC* pDC, CRect rectImage)
   {
      try
      {
         // DEBUG:
         Console << "OnDrawIcon: " << Cons::Yellow << "rectImage=" << rectImage << ENDL;

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
         // DEBUG:
         Console << "OnDrawLabel: " << Cons::Yellow << "rect=" << rect << " Size=" << rect.Size() << " bCalcOnly=" << bCalcOnly << ENDL;

         // Get random title
         wstring sz = SyntaxLib.Find(133, GameVersion::Threat).Text;
      
         // Draw/Calculate rectangle
         pDC->DrawText(sz.c_str(), rect, DT_LEFT|DT_WORDBREAK | (bCalcOnly?DT_CALCRECT:NULL));

         // DEBUG:
         if (bCalcOnly)
            Console << "OnDrawLabel: " << Cons::Yellow << "Calculated Rect=" << rect << " Size=" << rect.Size() << ENDL;
      
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
         Console << "OnDrawDescription: " << Cons::Yellow << "rect=" << rect << " Size=" << rect.Size() << " bCalcOnly=" << bCalcOnly << ENDL;

         // Get random description
         wstring sz = DescriptionLib.Commands.Find(133, GameVersion::Threat);
         //Console << sz << ENDL << ENDL;
      
         // Draw/Calculate rectangle
         pDC->DrawText(sz.c_str(), rect, DT_LEFT|DT_WORDBREAK | (bCalcOnly?DT_CALCRECT:NULL));

         // DEBUG:
         if (bCalcOnly)
            Console << "OnDrawDescription: " << Cons::Yellow << "Calculated Rect=" << rect << " Size=" << rect.Size() << ENDL;
      
         // return size
         return rect.Size();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, L"Unable to draw description tooltip text");
         return CSize();
      }
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)
