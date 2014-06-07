
#pragma once


namespace GUI
{
   namespace Utils
   {
   
      /// <summary>Base-class for an implementation of owner draw</summary>
      class OwnerDrawImpl
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      public:
         OwnerDrawImpl(DRAWITEMSTRUCT& d, CWnd* c);
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
         bool     Disabled,      // Whether item is disabled
                  Focused,       // Whether item has focus
                  Selected;      // Whether item is selected
         CRect    Bounds;        // Item bounds
         SharedDC DC;            // Output DC
         UINT     CtrlID,        // Control ID
                  CtrlType,      // Control Type
                  Index;         // Item Index
         CWnd*    Ctrl;          // Control

      private:
         CFont*   PrevFont;      // Initially selected DC font
      };
   
   }
}

using namespace GUI::Utils;
