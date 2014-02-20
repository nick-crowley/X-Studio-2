
#pragma once
#include "Logic/WorkerFeedback.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>Find/Replace terms comboBox</summary>
   class FindComboBox : public CComboBox
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      FindComboBox();
	   virtual ~FindComboBox();

      // ------------------------ STATIC -------------------------
	   
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------	
   public:
      bool           Contains(const wstring& str, bool matchCase = false) const;
      list<wstring>  GetAllStrings() const;
      
      // ----------------------- MUTATORS ------------------------
   public:
      

      // -------------------- REPRESENTATION ---------------------
   protected:
   };


NAMESPACE_END2(GUI,Controls)

