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

   #ifdef _DEBUG
   void ImageListEx::AssertValid() const
   {
	   CImageList::AssertValid();
   }
   
   void ImageListEx::Dump(CDumpContext& dc) const
   {
	   CImageList::Dump(dc);
   }
   #endif //_DEBUG

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
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Utils)

