#pragma once
#include <afxwin.h>

/// <summary>User interface Utilities</summary>
NAMESPACE_BEGIN2(GUI,Utils)
   
   /// <summary></summary>
   class ImageListEx : public CImageList
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      ImageListEx();  
      virtual ~ImageListEx();
       
      // ------------------------ STATIC -------------------------

      DECLARE_DYNCREATE(ImageListEx)
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL Create(UINT nResID, UINT nSize, UINT nInitial, DWORD dwMask);
      BOOL Draw(CDC& dc, UINT image, CRect rc, CSize sz, UINT state = 0);
      
      BOOL Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle)
      {
         return __super::Draw(pDC, nImage, pt, nStyle);
      }

      // -------------------- REPRESENTATION ---------------------
      
   private:
   };
   

NAMESPACE_END2(GUI,Utils)
