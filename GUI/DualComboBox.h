#pragma once
#include "ComboBoxOwnerDraw.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>ComboBox with item icons and optional 'alternate' item text</summary>
   class DualComboBox : public CComboBox
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Item data for each item</summary>
      class ItemData
      {
      public:
         /// <summary>Create data for an item.</summary>
         /// <param name="l">Item text.</param>
         /// <param name="r">Alternate item text.</param>
         /// <param name="icon">Item icon.</param>
         ItemData(const wstring& l, const wstring& r, UINT icon) : Left(l), Right(r), Icon(icon)
         {}

      public:
         wstring Left,     // Item text (lhs)
                 Right;    // Item text (rhs)
         UINT    Icon;     // Item icon
      };
   
      /// <summary>Custom implementation of owner draw</summary>
      class ControlRenderer : public ComboBoxOwnerDraw
      {
      public:
         ControlRenderer(DRAWITEMSTRUCT& d, CComboBox* c);

      public:
         void DrawItem() override;

      protected:
         ItemData*  Item;      // Item Data
      };
	  
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
      ItemData* FindItem(UINT index) const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  AddItem(wstring txt, wstring alt, UINT icon);
      void  DrawItem(LPDRAWITEMSTRUCT pd) override;
      void  DeleteItem(LPDELETEITEMSTRUCT pd) override;
      void  SetImageList(CImageList* list);

      // -------------------- REPRESENTATION ---------------------
   protected:
      CImageList* Images;
   };


NAMESPACE_END2(GUI,Controls)

