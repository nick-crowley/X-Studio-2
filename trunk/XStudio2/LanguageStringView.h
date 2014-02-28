#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "LanguagePageView.h"
#include "ImageListEx.h"
#include "ListViewCustomDraw.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)
   
   /// <summary></summary>
   class LanguageStringView : public CListView
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Custom draw implementation</summary>
      class StringCustomDraw : public ListViewCustomDraw
      {
      public:
         StringCustomDraw(LanguageStringView* view) : ListViewCustomDraw(view, view->GetListCtrl())
         {}

      protected:
         void  onDrawSubItem(CDC* dc, ItemData& item) override;
      };

   public:
      /// <summary>Base class for all Language document properties</summary>
      class StringPropertyBase : public LanguageDocument::LanguagePropertyBase
      {
      public:
         /// <summary>Create string property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="str">string.</param>
         /// <param name="name">name.</param>
         /// <param name="val">value</param>
         /// <param name="desc">description.</param>
         StringPropertyBase(LanguageDocument& doc, LanguageString& str, wstring name, _variant_t val, wstring desc)
            : String(str), LanguagePropertyBase(doc, name, val, desc)
         {}

      protected:
         LanguageString& String;
      };
      
      /// <summary>colour tags property grid item</summary>
      class ColourTagProperty : public StringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create string title property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="string">string.</param>
         /// <exception cref="Logic::ArgumentException">Colour tags are still undetermined</exception>
         ColourTagProperty(LanguageDocument& doc, LanguageString& string) 
            : StringPropertyBase(doc, string, L"Colour Tags", GetString(string.TagType).c_str(),  L"Determines whether to use named colour tags or escape codes")
         {
            // Require tags to be already determined
            if (String.TagType == ColourTag::Undetermined)
               throw ArgumentException(HERE, L"string", L"Colour tags are undetermined");

            // Populate
            AddOption(GetString(ColourTag::Unix).c_str());
            AddOption(GetString(ColourTag::Message).c_str());
            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ------------------------ STATIC -------------------------
      protected:
         /// <summary>Gets colour tag string.</summary>
         /// <param name="tag">The tag.</param>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Unknown tag</exception>
         static wstring  GetString(ColourTag tag)
         {
            switch (tag)
            {
            case ColourTag::Unix:          return L"Colour Codes";
            case ColourTag::Message:       return L"Message Tags";
            case ColourTag::Undetermined:  return L"Undetermined";
            }
            throw ArgumentException(HERE, L"tag", L"Unrecognised Colour tag enumeration");
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update colour tag type</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            String.TagType = (value == L"Colour Codes" ? ColourTag::Unix : ColourTag::Message);
            __super::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>ID property grid item</summary>
      class IDProperty : public StringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create string ID property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="page">page containing string.</param>
         /// <param name="string">string.</param>
         IDProperty(LanguageDocument& doc, LanguagePage& page, LanguageString& string)
            : Page(page), StringPropertyBase(doc, string, L"ID", string.ID,  L"string ID")
              
         {}

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures ID is unique</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return value.length() && value.IsNumeric() 
                && (String.ID == value.ToInt() || !Page.Contains(value.ToInt()));   // ID is Unchanged or available
         }

         /// <summary>Update ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // TODO: Change ID
            //File.ID = value.ToInt();
            __super::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         LanguagePage& Page;
      };

      /// <summary>version property grid item</summary>
      class VersionProperty : public StringPropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create game version property.</summary>
         /// <param name="doc">document.</param>
         /// <param name="string">string.</param>
         VersionProperty(LanguageDocument& doc, LanguageString& str)
            : StringPropertyBase(doc, str, L"Version", VersionString(str.Version).c_str(),  L"Version of game that string originates")
         {
            // Populate game versions
            for (int i = 0; i < 4; i++)
               AddOption(VersionString(GameVersionIndex(i).Version).c_str());

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update game version</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            // Convert zero-based index into GameVersion
            String.Version = GameVersionIndex(Find(value.c_str())).Version;      
            // Modify document
            __super::OnValueChanged(value);    
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      // --------------------- CONSTRUCTION ----------------------
   protected:
      LanguageStringView();    // Protected constructor used by dynamic creation
   public:
      virtual ~LanguageStringView();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(LanguageStringView)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      LanguageDocument* GetDocument() const;

   protected:
      LanguageString*   GetSelected() const;

      // ----------------------- MUTATORS ------------------------
   public:
      handler void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;

   protected:
      void AdjustLayout();

      afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
      handler void OnInitialUpdate() override;
      afx_msg void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
      handler void onPageSelectionChanged();
      afx_msg void OnQueryRemoveSelected(CCmdUI* pCmdUI);
      afx_msg void OnRemoveSelected();
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      

   private:
      SelectionChangedHandler  fnPageSelectionChanged;
      ImageListEx              Images;
      StringCustomDraw         CustomDraw;
   };
   

NAMESPACE_END2(GUI,Views)
