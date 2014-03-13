#include "stdafx.h"
#include "ImageListEx.h"

/// <summary>User interface Utilities</summary>
NAMESPACE_BEGIN2(GUI,Utils)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ImageListEx, CImageList)

   // -------------------------------- CONSTRUCTION --------------------------------

   ImageListEx::ImageListEx()
   {
   }

   ImageListEx::~ImageListEx()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates an image list for 24-bit icons</summary>
   /// <param name="nResID">The resource identifier.</param>
   /// <param name="nSize">Size of the images</param>
   /// <param name="nInitial">The initial count</param>
   /// <param name="dwMask">The transparency mask colour</param>
   /// <returns></returns>
   BOOL  ImageListEx::Create(UINT nResID, UINT nSize, UINT nInitial, DWORD dwMask)
   {
      CBitmap* bmp = theApp.LoadBitmapW(nResID, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
      CImageList::Create(nSize, nSize, ILC_COLOR24|ILC_MASK, nInitial, 4);
      CImageList::Add(bmp, dwMask);
      delete bmp;
      return TRUE;
   }

   /// <summary>Draws the specified dc.</summary>
   /// <param name="dc">dc.</param>
   /// <param name="image">image index.</param>
   /// <param name="rc">target rectangle.</param>
   /// <param name="sz">image size.</param>
   /// <param name="state">optionally ODS_DISABLED or ODS_SELECTED.</param>
   /// <returns></returns>
   BOOL  ImageListEx::Draw(CDC& dc, UINT image, CRect rc, CSize sz, UINT state /*= NULL*/)
   {
      IMAGELISTDRAWPARAMS   oDrawData;

      // Prepare
      ZeroMemory(&oDrawData, sizeof(IMAGELISTDRAWPARAMS));
      oDrawData.cbSize = sizeof(IMAGELISTDRAWPARAMS);

      // Centre icon within drawing rectangle
      CRect draw(rc.CenterPoint(), CSize(0,0));
      draw.InflateRect(sz.cx/2, sz.cy/2);

      // Set properties
      oDrawData.himl   = GetSafeHandle();
      oDrawData.i      = image;
      oDrawData.hdcDst = dc.m_hDC;
      oDrawData.x      = draw.left;
      oDrawData.y      = draw.top;
      oDrawData.fStyle = ILD_TRANSPARENT;

      // Disabled:
      if (state & ODS_DISABLED)
         oDrawData.fState  = ILS_SATURATE;
      
      // Selected:
      else if (state & ODS_SELECTED)
      {
         oDrawData.rgbFg   = CLR_DEFAULT;
         oDrawData.fStyle |= ILD_BLEND;
      }

      /// Draw icon
      return ImageList_DrawIndirect(&oDrawData);
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Utils)

