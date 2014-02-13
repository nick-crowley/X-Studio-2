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
   

NAMESPACE_END2(GUI,Utils)
