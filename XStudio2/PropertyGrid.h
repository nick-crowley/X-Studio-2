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

   protected:
      BOOL OnUpdateValue() override
      {
         CString strText;

         // Get editing text
	      m_pWndInPlace->GetWindowText(strText);
         GuiString value((const wchar*)strText);
         
         try
         {
            // Validate/Modify value
            if (!OnValidateValue(value))
               return FALSE;

            // Replace input
            m_pWndInPlace->SetWindowTextW(value.c_str());

            // Success: Update property
            CMFCPropertyGridProperty::OnUpdateValue();
            OnValueChanged(value);
            return TRUE;
         }
         catch (ExceptionBase& e) {
            theApp.ShowError(HERE, e, GuiString(L"Unable to update %s property with value '%s'", GetName(), (const wchar*)strText));
            return FALSE;
         }
      }

   
      /// <summary>Validates and optionally modifies the value being input</summary>
      /// <param name="value">The value.</param>
      /// <returns>True to accept, false to reject</returns>
      virtual bool OnValidateValue(GuiString& value)
      {
         return true;
      }

      /// <summary>Called when value changed.</summary>
      /// <param name="value">value text.</param>
      virtual void OnValueChanged(GuiString value)
      {
      }

   
      /// <summary>Query whether an option exists (case insensitive)</summary>
      /// <param name="value">item text.</param>
      /// <returns></returns>
      bool Contains(const wstring& value)
      {
         auto val = value.c_str();
         
         // Search options
         for (auto pos = m_lstOptions.GetHeadPosition(); pos; )
            if (!StrCmpI(m_lstOptions.GetNext(pos), val))
               return true;

         return false;
      }

      /// <summary>Finds the index of an option from it's text  (case insensitive)</summary>
      /// <param name="value">item text</param>
      /// <returns>Index if found, otherwise -1</returns>
      int Find(const wstring& value)
      {
         auto val = value.c_str();

         // Search options
         int i = 0;
         for (auto pos = m_lstOptions.GetHeadPosition(); pos; ++i)
            if (!StrCmpI(m_lstOptions.GetNext(pos), val))
               return i;

         // Not found
         return -1;
      }

      bool  TryFind(const wstring& value, int& index)
      {
         index = Find(value);
         return index != -1;
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

