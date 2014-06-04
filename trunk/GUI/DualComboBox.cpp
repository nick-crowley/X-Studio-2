#include "stdafx.h"
#include "DualComboBox.h"
#include "OwnerDrawImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // ----------------------------------- GLOBALS ----------------------------------

   class ComboItemData
   {
   public:
      ComboItemData(wstring left, wstring right, UINT icon) : Left(left), Right(right), Icon(icon)
      {}

   public:
      wstring Left, 
              Right;
      UINT    Icon;
   };

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(DualComboBox, CComboBox)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   DualComboBox::DualComboBox() : Images(nullptr)
   {
   }

   DualComboBox::~DualComboBox()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Check whether listBox contains a specified string</summary>
   /// <param name="str">The string.</param>
   /// <param name="matchCase">match case.</param>
   /// <returns></returns>
   //bool  DualComboBox::Contains(const wstring& str, bool matchCase) const
   //{
   //   auto list = GetAllStrings();

   //   // Query strings in listBox
   //   return any_of(list.begin(), list.end(), [&str,matchCase](const wstring& s) {
   //      return matchCase ? s == str : StrCmpI(s.c_str(), str.c_str()) == 0;
   //   });
   //}

   void DualComboBox::AddItem(wstring txt, wstring alt, UINT icon)
   {
      __super::AddString((LPCWSTR)new ComboItemData(txt, alt, icon));
   }
   
   void DualComboBox::DeleteItem(LPDELETEITEMSTRUCT pd)
   {
      // Cleanup item data
      ComboItemData* data = reinterpret_cast<ComboItemData*>(pd->itemData);
      
      Console << "Deleting item #" << pd->itemID << " data=" << data << ENDL;
      delete data;

      __super::DeleteItem(pd);
   }


   void DualComboBox::DrawItem(LPDRAWITEMSTRUCT pd)
   {
      bool editMode = (pd->itemState & ODS_COMBOBOXEDIT) != 0;

      // EDIT MODE
      if (editMode)
         return;

      //CString  str;
      //GetLBText(pData->itemID, str);
      //dc.DrawTextW(str, &pData->rcItem, DT_LEFT);

      
      
      // Prepare
      OwnerDrawImpl g(*pd, GetFont());
      CRect rc(&pd->rcItem);

      ComboItemData* item = reinterpret_cast<ComboItemData*>(pd->itemData);
      Console << "Drawing item ID: " << pd->itemID << " rcItem=" << rc << " data=" << item << ENDL;
      
      // BACKGROUND
      g.DrawBackground(rc, COLOR_WINDOW);

      // Shrink drawing area to create border
      rc.DeflateRect(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE));
      
      // TEXT
      g.DrawText(item->Left, rc, DT_LEFT, COLOR_WINDOWTEXT);
      g.DrawText(item->Right, rc, DT_RIGHT, COLOR_GRAYTEXT);

      // TODO:  Add your code to draw the specified item
      //Console << "Drawing item #" << (UINT)pData->hwndItem << " rcItem=" << CRect(pData->rcItem) << ENDL;
   }


   /// <summary>Gets all strings in the listbox</summary>
   /// <returns></returns>
   //list<wstring>  DualComboBox::GetAllStrings() const
   //{
   //   list<wstring> list;
   //   CString       str;

   //   // Add all items to list
   //   for (int i = 0; i < GetCount(); ++i)
   //   {
   //      GetLBText(i, str);
   //      list.push_back((const wchar*)str);
   //   }

   //   return list;
   //}

   void DualComboBox::SetImageList(CImageList* list)
   {
      // Cleanup previous
      if (Images)
         delete Images;

      // Replace image list
      Images = list;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)


