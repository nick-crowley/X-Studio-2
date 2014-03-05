#pragma once
#include "PreferencesPage.h"
#include "PropertySource.h"
#include "Logic/PreferencesLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary>Base class for all preference pages</summary>
   class PreferencesPage : public CMFCPropertyPage
   {
      // ------------------------ TYPES --------------------------
   protected:
      
      /// <summary>Base class for all basic properties</summary>
      class PropertyBase : public ValidatingProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create group property.</summary>
         /// <param name="page">Owner page.</param>
         /// <param name="group">Group name.</param>
         /// <param name="valueList">Whether a value list.</param>
         PropertyBase(PreferencesPage& page, const CString& group, bool valueList = false)
            : Page(page), ValidatingProperty(group, 0, valueList)
         {}

         /// <summary>Create item property</summary>
         /// <param name="page">Owner page.</param>
         /// <param name="name">Name.</param>
         /// <param name="value">value.</param>
         /// <param name="desc">description.</param>
         PropertyBase(PreferencesPage& page, const CString& name, const COleVariant& value, const wchar* desc)
            : Page(page), ValidatingProperty(name, value, desc, 0, nullptr, nullptr, nullptr)
         {}

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as int.</summary>
         /// <returns></returns>
         int  GetInt() const
         {
            return GetValue().intVal;
         }

         /// <summary>Gets value as string.</summary>
         /// <returns></returns>
         wstring  GetString() const
         {
            return (const wchar*)CString(GetValue());
         }

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Marks the property page as modified.</summary>
         /// <param name="value">value text.</param>
         void OnValueChanged(GuiString value) override
         {
            Page.SetModified(TRUE);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         PreferencesPage&  Page;
      };

      /// <summary>Base class for boolean properties</summary>
      class BooleanProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create boolean property</summary>
         /// <param name="page">Owner page.</param>
         /// <param name="name">Name.</param>
         /// <param name="value">value.</param>
         /// <param name="desc">description.</param>
         BooleanProperty(PreferencesPage& page, const CString& name, bool value, const wchar* desc)
            : PropertyBase(page, name, value ? L"True" : L"False", desc)
         {
            AddOption(L"True");
            AddOption(L"False");
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == L"True";
         }

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      };
      
      /// <summary>Base class for font properties</summary>
      class FontProperty : public CMFCPropertyGridFontProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create font property</summary>
         /// <param name="page">owner page.</param>
         /// <param name="name">property name.</param>
         /// <param name="font">font properties.</param>
         /// <param name="desc">property description.</param>
         /// <param name="flags">CHOOSEFONT dialog flags.</param>
         FontProperty(PreferencesPage& page, const CString& name, LOGFONT font, const wchar* desc, DWORD flags = CF_FORCEFONTEXIST | CF_SCREENFONTS)
            : Page(page), CMFCPropertyGridFontProperty(name, font, flags, desc, 0, (COLORREF)-1)
         {}

         // ------------------------ STATIC -------------------------
      protected:
         /// <summary>Gets logical font data.</summary>
         /// <param name="font">font name.</param>
         /// <param name="size">size in points.</param>
         /// <returns></returns>
         /*static LOGFONT  LookupFont(LPCWSTR font, int size)
         {
            CFont f;
            LOGFONT lf;
            f.CreatePointFont(size*10, font);
            f.GetLogFont(&lf);
            return lf;
         }*/

         // ---------------------- ACCESSORS ------------------------	
   
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Called when value is being updated</summary>
         /// <returns></returns>
         BOOL OnUpdateValue() override
         {
            // Update property
            __super::OnUpdateValue();

            // Mark page as modified
            Page.SetModified(TRUE);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         PreferencesPage& Page;
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      PreferencesPage(UINT nID);    
      virtual ~PreferencesPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(PreferencesPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void AdjustLayout();
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      virtual void Populate();

	   afx_msg void OnSize(UINT nType, int cx, int cy);

      // -------------------- REPRESENTATION ---------------------
   protected:
      CMFCPropertyGridCtrl  Grid;
   
   };
   
NAMESPACE_END2(GUI,Preferences)

