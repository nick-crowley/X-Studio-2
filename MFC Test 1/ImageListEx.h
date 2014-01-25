#pragma once
#include <afxwin.h>

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)
   
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
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif  
      // ----------------------- MUTATORS ------------------------
   public:
      BOOL Create(UINT nResID, UINT nSize, UINT nInitial, DWORD dwMask);

      // -------------------- REPRESENTATION ---------------------
      
   private:
   };
   

NAMESPACE_END(GUI)
