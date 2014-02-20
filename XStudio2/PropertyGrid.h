#pragma once

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   class ValidatingProperty : public CMFCPropertyGridProperty
   {
   public:
	   ValidatingProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE)
         : CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
      {}

	   ValidatingProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		   LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL)
         : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
      {}


      BOOL OnUpdateValue() override
      {
         CString strText;
	      m_pWndInPlace->GetWindowText(strText);
         
         // Validate value
         if (!OnValidateValue((const wchar*)strText))
            return FALSE;

         // Success: Update property
         CMFCPropertyGridProperty::OnUpdateValue();
         OnValueChanged((const wchar*)strText);
         return TRUE;
      }

      virtual bool OnValidateValue(wstring value)
      {
         return true;
      }

      virtual void OnValueChanged(wstring value)
      {
      }

      bool Contains(const wstring& value)
      {
         auto val = value.c_str();
         
         // Search options
         for (auto pos = m_lstOptions.GetHeadPosition(); pos; )
            if (!StrCmpI(m_lstOptions.GetNext(pos), val))
               return true;

         return false;
      }

      UINT Find(const wstring& value)
      {
         auto val = value.c_str();

         // Search options
         UINT i = 0;
         for (auto pos = m_lstOptions.GetHeadPosition(); pos; ++i)
            if (!StrCmpI(m_lstOptions.GetNext(pos), val))
               return i;

         // Error
         throw AlgorithmException(HERE, L"Property value not found");
      }
   };

   /// <summary>Find/Replace terms comboBox</summary>
   class PropertyGrid : public CMFCPropertyGridCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      PropertyGrid();
	   virtual ~PropertyGrid();

      // ------------------------ STATIC -------------------------
	   
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------	
   public:
      
      // ----------------------- MUTATORS ------------------------
   protected:
      //BOOL ValidateItemData(CMFCPropertyGridProperty* pProp) override;

      // -------------------- REPRESENTATION ---------------------
   protected:
   };


NAMESPACE_END2(GUI,Controls)

