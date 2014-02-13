#include "stdafx.h"
#include "CustomDrawImpl.h"

namespace GUI
{
   namespace Utils
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a custom draw implementation</summary>
      /// <param name="wnd">Owner window</param>
      /// <param name="flags">Desired paint handlers</param>
      CustomDrawImpl::CustomDrawImpl(CWnd* wnd, UINT flags) : Wnd(wnd), Flags(flags)
      {
         // Validate flags
         if ((flags & ((UINT)DrawCycle::Paint | (UINT)DrawCycle::Erase)) == NULL)
            throw ArgumentException(HERE, L"flags", L"Must specify DrawCycle::Paint and/or DrawCycle::Erase");
      }


      CustomDrawImpl::~CustomDrawImpl()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Invokes the appropriate paint handler</summary>
      /// <param name="pDraw">draw data</param>
      /// <returns>Custom draw message return code</returns>
      LRESULT  CustomDrawImpl::OnCustomDraw(NMCUSTOMDRAW* pDraw)
      {
         LRESULT retCode = (LRESULT)RetCode::SystemDrawn;

         switch (pDraw->dwDrawStage)
         {
            // [PRE-CYCLE] Request appropriate notifications
         case CDDS_PREERASE:
         case CDDS_PREPAINT:
            Console << "CDDS_PREPAINT" << ENDL;
            // Items: Request item drawing
            if (Flags & (UINT)DrawCycle::Items)
               retCode |= (LRESULT)RetCode::NotifyItem;

            // PostPaint/PostErase: Request notification
            if (Flags & (UINT)DrawCycle::Paint)
               retCode |= (LRESULT)RetCode::NotifyPostPaint;
            if (Flags & (UINT)DrawCycle::Erase)
               retCode |= (LRESULT)RetCode::NotifyPostErase;
            break;

            // [PRE-ITEM] 
         case CDDS_ITEMPREERASE:
         case CDDS_ITEMPREPAINT:
            Console << "CDDS_ITEMPREPAINT" << ENDL;
            // SubItems: Request sub-item drawing
            if (Flags & (UINT)DrawCycle::SubItems)
               retCode |= (LRESULT)RetCode::NotifyItem;

            // Paint/Erase item
            if (onDrawItem(pDraw, pDraw->dwDrawStage == CDDS_ITEMPREPAINT ? Stage::Paint : Stage::Erase))
               retCode |= (LRESULT)RetCode::UserDrawn;
            else
               return (LRESULT)RetCode::SystemDrawn;
            break;

            // [PRE-SUBITEM] 
         case CDDS_SUBITEM | CDDS_ITEMPREERASE:
         case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
            Console << "CDDS_SUBITEM | CDDS_ITEMPREPAINT" << ENDL;
            // Draw sub-item
            if (onDrawSubItem(pDraw, pDraw->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT) ? Stage::Paint : Stage::Erase))
               return (LRESULT)RetCode::UserDrawn;
            break;

            //// [POST-ITEM] 
            //case CDDS_ITEMPOSTERASE:
            //case CDDS_ITEMPOSTPAINT:
            //   // Paint/Erase item
            //   if (onDrawItem(pDraw, pDraw->dwDrawStage == CDDS_ITEMPOSTPAINT ? Stage::PostPaint : Stage::PostErase))
            //      return (LRESULT)RetCode::UserPostPaint; 
            //   break;

            //// [POST-SUBITEM] 
            //case CDDS_SUBITEM | CDDS_ITEMPOSTERASE:
            //case CDDS_SUBITEM | CDDS_ITEMPOSTPAINT:
            //   // Draw sub-item
            //   if (!onDrawSubItem(pDraw, pDraw->dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPOSTPAINT) ? Stage::PostPaint : Stage::PostErase))
            //      return (LRESULT)RetCode::SystemDrawn;
            //   break;

            //// [POST-CYLE] 
            //case CDDS_POSTERASE:
            //case CDDS_POSTPAINT:
            //   break;
         }

         return retCode;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   }
}

