#pragma once


/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   /// <summary>Language document</summary>
   class LanguageDocument : public CDocument
   {
	   // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      LanguageDocument();
	   virtual ~LanguageDocument();

      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguageDocument)
   protected:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
     public:
	#ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif
      // ----------------------- MUTATORS ------------------------
   public:
      virtual void Serialize(CArchive& ar);      

   protected:
	   virtual BOOL OnNewDocument();

      // -------------------- REPRESENTATION ---------------------
   
   };


NAMESPACE_END(GUI)

