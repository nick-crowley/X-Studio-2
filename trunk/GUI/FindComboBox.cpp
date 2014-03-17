#include "stdafx.h"
#include "FindComboBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(FindComboBox, CComboBox)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   FindComboBox::FindComboBox()
   {
   }

   FindComboBox::~FindComboBox()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Check whether listBox contains a specified string</summary>
   /// <param name="str">The string.</param>
   /// <param name="matchCase">match case.</param>
   /// <returns></returns>
   bool  FindComboBox::Contains(const wstring& str, bool matchCase) const
   {
      auto list = GetAllStrings();

      // Query strings in listBox
      return any_of(list.begin(), list.end(), [&str,matchCase](const wstring& s) {
         return matchCase ? s == str : StrCmpI(s.c_str(), str.c_str()) == 0;
      });
   }

   /// <summary>Gets all strings in the listbox</summary>
   /// <returns></returns>
   list<wstring>  FindComboBox::GetAllStrings() const
   {
      list<wstring> list;
      CString       str;

      // Add all items to list
      for (int i = 0; i < GetCount(); ++i)
      {
         GetLBText(i, str);
         list.push_back((const wchar*)str);
      }

      return list;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   


   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)


