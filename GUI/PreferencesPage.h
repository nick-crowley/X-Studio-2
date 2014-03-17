#pragma once
#include "PreferencesPage.h"
#include "PropertySource.h"
#include "../Logic/PreferencesLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   /// <summary>Forward declaration</summary>
   class PreferencesDialog;

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

      /// <summary>Base class for colour properties</summary>
      class ColourProperty : public CMFCPropertyGridColorProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create colour property</summary>
         /// <param name="page">owner page.</param>
         /// <param name="name">property name.</param>
         /// <param name="col">colour.</param>
         /// <param name="desc">property description.</param>
         ColourProperty(PreferencesPage& page, const CString& name, COLORREF col, const wchar* desc)
            : Page(page), CMFCPropertyGridColorProperty(name, col, nullptr, desc, 0)
         {
            // Enable more colours dialog
            EnableOtherButton(L"More Colours", FALSE, TRUE);
         }

         // ------------------------ STATIC -------------------------

         // ---------------------- ACCESSORS ------------------------	
   
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Called when value is being updated</summary>
         /// <returns></returns>
         BOOL OnUpdateValue() override
         {
            // Update property
            if (!__super::OnUpdateValue())
               return FALSE;

            // Mark page as modified
            Page.SetModified(TRUE);
            return TRUE;
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         PreferencesPage& Page;
      };
      
      /// <summary>Base class for folder properties</summary>
      class FolderProperty : public CMFCPropertyGridFileProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create folder property</summary>
         /// <param name="page">owner page.</param>
         /// <param name="name">property name.</param>
         /// <param name="folder">initial folder.</param>
         /// <param name="desc">property description.</param>
         FolderProperty(PreferencesPage& page, const CString& name, Path folder, const wchar* desc) 
            : Page(page), CMFCPropertyGridFileProperty(name, folder.c_str(), 0, desc)
         {}

         // ---------------------- ACCESSORS ------------------------
         
         /// <summary>Gets folder as path.</summary>
         /// <returns></returns>
         Path GetFolder() const
         {
            return (const wchar*)CString(GetValue());
         }

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Called when value is being updated</summary>
         /// <returns></returns>
         BOOL OnUpdateValue() override
         {
            // Update property
            if (!__super::OnUpdateValue())
               return FALSE;

            // Mark page as modified
            Page.SetModified(TRUE);
            return TRUE;
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         PreferencesPage&  Page;
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

         // ---------------------- ACCESSORS ------------------------	
   
         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Called when value is being updated</summary>
         /// <returns></returns>
         BOOL OnUpdateValue() override
         {
            // Update property
            if (!__super::OnUpdateValue())
               return FALSE;

            // Mark page as modified
            Page.SetModified(TRUE);
            return TRUE;
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
      BOOL OnApply() override;
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void         AdjustLayout();
      virtual void Commit();
      void         DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      virtual void Populate();

	   afx_msg void OnSize(UINT nType, int cx, int cy);

      // -------------------- REPRESENTATION ---------------------
   protected:
      CMFCPropertyGridCtrl  Grid;
   
   };
   
NAMESPACE_END2(GUI,Preferences)

