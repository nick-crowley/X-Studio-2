#pragma once


namespace GUI
{
   namespace Utils
   {
      /// <summary>Types of custom draw available</summary>
      enum class DrawCycle : UINT { Off = 0, Items = 1, SubItems = 2, Paint = 4, Erase = 8 };


      /// <summary>Base class for custom draw implementations</summary>
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
         CustomDrawImpl(CWnd* wnd, UINT flags = (UINT)DrawCycle::Items | (UINT)DrawCycle::SubItems | (UINT)DrawCycle::Paint);
         virtual ~CustomDrawImpl();

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
         LRESULT  OnCustomDraw(NMCUSTOMDRAW* pDraw);

      protected:
         virtual bool  onDrawItem(NMCUSTOMDRAW* pDraw, Stage stage) { return false; }
         virtual bool  onDrawSubItem(NMCUSTOMDRAW* pDraw, Stage stage) { return false; }
         /*bool  onEraseItem(NMCUSTOMDRAW* pDraw, Stage stage);
         bool  onEraseSubItem(NMCUSTOMDRAW* pDraw, Stage stage);*/

         // -------------------- REPRESENTATION ---------------------

      protected:
         CWnd*  Wnd;

      private:
         UINT   Flags;
      };

   }
}

using namespace GUI;
