#include "stdafx.h"
#include "CustomDrawImpl.h"

namespace GUI
{

   // -------------------------------- CONSTRUCTION --------------------------------

   CustomDrawImpl::CustomDrawImpl(CWnd* wnd, UINT flags) : Wnd(wnd), Flags(flags)
   {
      // Validate flags
      if ((flags & ((UINT)CustomDraw::Paint | (UINT)CustomDraw::Erase)) == NULL)
         throw ArgumentException(HERE, L"flags", L"Must specify CustomDraw::Paint and/or CustomDraw::Erase");
   }


   CustomDrawImpl::~CustomDrawImpl()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   LRESULT  CustomDrawImpl::OnCustomDraw(NMCUSTOMDRAW* pDraw)
   {
      LRESULT retCode = (LRESULT)RetCode::SystemDrawn;

      switch (pDraw->dwDrawStage)
      {
      // [PRE-CYCLE] Request appropriate notifications
      case CDDS_PREERASE: 
      case CDDS_PREPAINT:
         // Items: Request item drawing
         if (Flags & (LRESULT)CustomDraw::Items)
            retCode |= (LRESULT)RetCode::NotifyItem;

         // PostPaint/PostErase: Request notification
         if (Flags & (LRESULT)CustomDraw::Paint)
            retCode |= (LRESULT)RetCode::NotifyPostPaint;
         if (Flags & (LRESULT)CustomDraw::Erase)
            retCode |= (LRESULT)RetCode::NotifyPostErase;
         break;

      // [PRE-ITEM] 
      case CDDS_ITEMPREERASE:
      case CDDS_ITEMPREPAINT:
         // SubItems: Request sub-item drawing
         if (Flags & (UINT)CustomDraw::SubItems)
            retCode |= (UINT)RetCode::NotifyItem;

         // Paint/Erase item
         if (!onDrawItem(pDraw, pDraw->dwDrawStage == CDDS_ITEMPREPAINT ? Stage::Paint : Stage::Erase))
            return (LRESULT)RetCode::SystemDrawn; 
         break;

      // [PRE-SUBITEM] 
      case CDDS_SUBITEM | CDDS_ITEMPREERASE:
      case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
         // Draw sub-item
         if (!onDrawSubItem(pDraw, pDraw->dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) ? Stage::Paint : Stage::Erase))
            return (LRESULT)RetCode::SystemDrawn;
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

