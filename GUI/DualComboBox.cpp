#include "stdafx.h"
#include "DualComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(DualComboBox, CComboBox)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Create empty DualComboBox</summary>
   DualComboBox::DualComboBox() : Images(nullptr)
   {
   }

   /// <summary>Create renderer for control</summary>
   /// <param name="d">Owner draw data</param>
   /// <param name="c">Control</param>
   DualComboBox::ControlRenderer::ControlRenderer(DRAWITEMSTRUCT& d, CComboBox* c)
      : ComboBoxOwnerDraw(d, c),
        Item(reinterpret_cast<ItemData*>(d.itemData))
   {
   }
   
   DualComboBox::~DualComboBox()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Appends a new item to the combo-box</summary>
   /// <param name="txt">Item text.</param>
   /// <param name="alt">Alternate item text.</param>
   /// <param name="icon">ImageList icon index.</param>
   void DualComboBox::AddItem(wstring txt, wstring alt, UINT icon)
   {
      __super::AddString((LPCWSTR)new ItemData(txt, alt, icon));
   }
   
   /// <summary>Deletes an item when removed.</summary>
   /// <param name="pd">Message data.</param>
   void DualComboBox::DeleteItem(LPDELETEITEMSTRUCT pd)
   {
      try
      {
         ItemData* data = reinterpret_cast<ItemData*>(pd->itemData);
         
         // Validate item
         if (pd->itemID == CB_ERR)
            throw AlgorithmException(HERE, L"Cannot delete item -1");
         else if (data == nullptr)
            throw AlgorithmException(HERE, VString(L"Missing item data for item #%d", pd->itemID));
            
         // Cleanup item data
         delete data;

         // Base
         __super::DeleteItem(pd);
      }
      catch (ExceptionBase& e)  {
         Console.Log(HERE, e);
      }
   }

   /// <summary>Find item data by index.</summary>
   /// <param name="index">Zero-based index.</param>
   /// <returns></returns>
   /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
   DualComboBox::ItemData* DualComboBox::FindItem(UINT index) const
   {
      // Validate index
      if (index >= (UINT)GetCount())
         throw IndexOutOfRangeException(HERE, index, GetCount());

      // Lookup item
      return reinterpret_cast<ItemData*>(GetItemData(index));
   }

   /// <summary>Draws an item</summary>
   /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
   /// <exception cref="Logic::InvalidOperationException">Invalid window styles</exception>
   void DualComboBox::ControlRenderer::DrawItem()
   {
      // Ensure ComboBox doesn't hold strings
      if (HasStrings())
         throw InvalidOperationException(HERE, L"Cannot owner-draw DualComboBox with CBS_HASSTRINGS");

      // BACKGROUND
      DrawBackground(Bounds, COLOR_WINDOW);

      // Edit Mode
      if (EditMode || Index == CB_ERR)
      {
         // Create left/right/top/bottom border
         Bounds.DeflateRect(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));

         // TEXT
         if (ComboBox->GetCount())
         {
            // Get selected item, if any
            auto sel = ComboBox->GetCurSel();
            ItemData* it = dynamic_cast<DualComboBox*>(ComboBox)->FindItem(sel != CB_ERR ? sel : 0);

            DrawText(it->Left, Bounds, DT_LEFT, COLOR_WINDOWTEXT);
         }
      }
      else
      {
         // Create left/right border
         Bounds.DeflateRect(GetSystemMetrics(SM_CXEDGE), 0);
      
         // TEXT
         DrawText(Item->Left, Bounds, DT_LEFT, COLOR_WINDOWTEXT);
         DrawText(Item->Right, Bounds, DT_RIGHT, COLOR_GRAYTEXT);
      }
   }


   /// <summary>Draws an item.</summary>
   /// <param name="pd">Owner draw data.</param>
   void DualComboBox::DrawItem(LPDRAWITEMSTRUCT pd)
   {
      try
      {
         ControlRenderer g(*pd, this);
         g.DrawItem();
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e, VString(L"Unable to draw item #%d", pd->itemID));
      }
   }

   /// <summary>Sets the image list.</summary>
   /// <param name="list">The list.</param>
   /// <exception cref="Logic::ArgumentNullException">List is nullptr</exception>
   void DualComboBox::SetImageList(CImageList* list)
   {
      REQUIRED(list);

      // Replace image list
      Images = list;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)


