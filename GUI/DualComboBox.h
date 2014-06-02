
#pragma once

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>Find/Replace terms comboBox</summary>
   class DualComboBox : public CComboBox
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      DualComboBox();
	   virtual ~DualComboBox();

      // ----------------------- STATIC --------------------------
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
   public:

      // ---------------------- ACCESSORS ------------------------	
   public:
      //bool           Contains(const wstring& str, bool matchCase = false) const;
      //list<wstring>  GetAllStrings() const;

      void  AddItem(wstring txt, wstring alt, UINT icon);
      void  DrawItem(LPDRAWITEMSTRUCT pd) override;
      void  DeleteItem(LPDELETEITEMSTRUCT pd) override;
      void  SetImageList(CImageList* list);
      
      // ----------------------- MUTATORS ------------------------
   public:
      
      // -------------------- REPRESENTATION ---------------------
   protected:
      CImageList* Images;
   };


NAMESPACE_END2(GUI,Controls)

