
#pragma once


namespace GUI
{
   namespace Utils
   {
   
      /// <summary></summary>
      class OwnerDrawImpl
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      public:
         OwnerDrawImpl(DRAWITEMSTRUCT& d, CFont* f);
         virtual ~OwnerDrawImpl();
		 
		   NO_COPY(OwnerDrawImpl);	// Uncopyable
		   NO_MOVE(OwnerDrawImpl);	// Unmoveable

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      public:

         // ----------------------- MUTATORS ------------------------
      public:
         virtual void  DrawItem()=0;

      protected:
         void  DrawBackground(CRect& rc, UINT col = COLOR_WINDOW);
         void  DrawFocus(CRect& rc);
         void  DrawText(const wstring& sz, CRect& rc, UINT flags = DT_LEFT, UINT col = COLOR_WINDOWTEXT);
         void  SetFont(CFont* f);

         // -------------------- REPRESENTATION ---------------------
      protected:
         bool     Disabled,
                  Focused, 
                  Selected;
         SharedDC DC;

      private:
         CFont*   PrevFont;
      };
   
   }
}

using namespace GUI::Utils;
