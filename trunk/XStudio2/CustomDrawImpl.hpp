#pragma once


namespace GUI
{
   namespace Utils
   {
      /// <summary>Types of custom draw available</summary>
      enum class DrawCycle : UINT { Off = 0, Items = 1, SubItems = 2, Paint = 4, Erase = 8 };

      /// <summary>Base class for custom draw implementations</summary>
      template <typename DATA_T>
      class CustomDrawImpl
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Return code flags</summary>
         enum class RetCode : LRESULT
         {
            UserDrawn = CDRF_SKIPDEFAULT,
            UserPostPaint = CDRF_SKIPPOSTPAINT,
            SystemDrawn = CDRF_DODEFAULT,
            SystemErase = CDRF_DOERASE,
            FontChanged = CDRF_NEWFONT,

            NotifyItem = CDRF_NOTIFYITEMDRAW,
            //NotifySubItem = CDRF_NOTIFYSUBITEMDRAW,
            NotifyPostPaint = CDRF_NOTIFYPOSTPAINT,
            NotifyPostErase = CDRF_NOTIFYPOSTERASE
         };

         /// <summary>Drawing stages</summary>
         enum class Stage
         {
            Erase, Paint, PostErase, PostPaint
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Creates a custom draw implementation</summary>
         /// <param name="wnd">Owner window</param>
         /// <param name="flags">Desired paint handlers</param>
         CustomDrawImpl(CWnd* wnd, UINT flags) : Wnd(wnd), Flags(flags)
         {
            // Validate flags
            if ((flags & ((UINT)DrawCycle::Paint | (UINT)DrawCycle::Erase)) == NULL)
               throw ArgumentException(HERE, L"flags", L"Must specify DrawCycle::Paint and/or DrawCycle::Erase");
         }
         virtual ~CustomDrawImpl()
         {}

         DEFAULT_COPY(CustomDrawImpl);	// Default copy semantics
         DEFAULT_MOVE(CustomDrawImpl);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
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
               if (onDrawItem(reinterpret_cast<DATA_T*>(pDraw), pDraw->dwDrawStage == CDDS_ITEMPREPAINT ? Stage::Paint : Stage::Erase))
                  retCode |= (LRESULT)RetCode::UserDrawn;
               else
                  return (LRESULT)RetCode::SystemDrawn;
               break;

               // [PRE-SUBITEM] 
            case CDDS_SUBITEM | CDDS_ITEMPREERASE:
            case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
               Console << "CDDS_SUBITEM | CDDS_ITEMPREPAINT" << ENDL;
               // Draw sub-item
               if (onDrawSubItem(reinterpret_cast<DATA_T*>(pDraw), pDraw->dwDrawStage == (CDDS_SUBITEM|CDDS_ITEMPREPAINT) ? Stage::Paint : Stage::Erase))
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

      protected:
         virtual bool  onDrawItem(DATA_T* pDraw, Stage stage) { return false; }
         virtual bool  onDrawSubItem(DATA_T* pDraw, Stage stage) { return false; }
         /*bool  onEraseItem(NMCUSTOMDRAW* pDraw, Stage stage);
         bool  onEraseSubItem(NMCUSTOMDRAW* pDraw, Stage stage);*/

         // -------------------- REPRESENTATION ---------------------

      protected:
         CWnd*  Wnd;

      private:
         UINT   Flags;
      };

      /// <summary>Generic custom draw implementation</summary>
      typedef CustomDrawImpl<NMCUSTOMDRAW>  BasicCustomDraw;

      /// <summary>ListView custom draw implementation</summary>
      typedef CustomDrawImpl<NMLVCUSTOMDRAW>  ListViewCustomDraw;
   }
}

using namespace GUI;
