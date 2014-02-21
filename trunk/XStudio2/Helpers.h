#pragma once
#include "stdafx.h"
#include <strsafe.h>

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary>Listview group helper</summary>
   class LVGroup : public LVGROUP
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      LVGroup(UINT id, const wstring& txt, UINT flags = LVGF_HEADER | LVGF_GROUPID) : Header(txt)
      {
         ZeroMemory((LVGROUP*)this, sizeof(LVGROUP));

         cbSize    = sizeof(LVGROUP);
         mask      = flags;
         iGroupId  = id;
         pszHeader = (WCHAR*)Header.c_str();
      }
      
      // ------------------------ STATIC -------------------------
      
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
      
      // -------------------- REPRESENTATION ---------------------
      
   private:
      wstring Header;
   };
   
   /// <summary>Listview item helper</summary>
   class LVItem : public LVITEM
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      /// <summary>Create empty item for retrieving text and properties</summary>
      /// <param name="item">Item</param>
      /// <param name="subItem">SubItem</param>
      /// <param name="bufLen">buffer length</param>
      /// <param name="flags">desired properties</param>
      LVItem(UINT item, UINT subItem, UINT bufLen, UINT flags = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE | LVIF_PARAM | LVIF_GROUPID)
      {
         ZeroMemory((LVITEM*)this, sizeof(LVITEM));

         // Allocate text buffer
         if (bufLen > 0)
         {
            Text = CharArrayPtr(new wchar[bufLen]);
            Text.get()[0] = '\0';
         }

         mask       = flags;
         pszText    = Text.get();
         cchTextMax = bufLen;
         iItem      = item;
         iSubItem   = subItem;
      }

      /// <summary>Create a populated item for setting text and properties</summary>
      /// <param name="item">item number</param>
      /// <param name="txt">text.</param>
      /// <param name="group">group.</param>
      /// <param name="flags">properties.</param>
      LVItem(UINT item, const wstring& txt, UINT group, UINT flags = LVIF_TEXT | LVIF_GROUPID) : Text(CopyText(txt))
      {
         ZeroMemory((LVITEM*)this, sizeof(LVITEM));

         mask     = flags;
         iItem    = item;
         iGroupId = group;
         pszText  = Text.get();
      }
      
      // ------------------------ STATIC -------------------------
   private:
      /// <summary>Duplicates the input string.</summary>
      /// <param name="str">The string.</param>
      /// <returns>new wide char array</returns>
      static wchar* CopyText(const wstring& str)
      {
         wchar* buf = new wchar[str.length()+1];
         StringCchCopy(buf, str.length()+1, str.c_str());
         return buf;
      }

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
      
      // -------------------- REPRESENTATION ---------------------
   protected:
      CharArrayPtr  Text;
   };


   /// <summary>TreeView item helper</summary>
   class TVItem : public TVITEMEX
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      /// <summary>Create empty item for retrieving text and properties</summary>
      /// <param name="node">node</param>
      /// <param name="bufLen">buffer length</param>
      /// <param name="flags">desired properties</param>
      TVItem(UINT bufLen, UINT flags = TVIF_TEXT | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM)
      {
         ZeroMemory(dynamic_cast<TVITEMEX*>(this), sizeof(TVITEMEX));

         // Allocate text buffer
         Text = CharArrayPtr(new wchar[bufLen+1]);
         Text.get()[0] = L'\0';

         // Set properties
         mask       = flags;
         pszText    = Text.get();
         cchTextMax = bufLen;
      }

      /// <summary>Create a populated item for setting text and properties</summary>
      /// <param name="node">node</param>
      /// <param name="txt">text.</param>
      /// <param name="flags">properties.</param>
      /*TVItem(const wstring& txt, UINT flags = TVIF_TEXT) : Text(CopyText(txt))
      {
         ZeroMemory(dynamic_cast<TVITEMEX*>(this), sizeof(TVITEMEX));

         mask     = flags;
         pszText  = Text.get();
      }*/
      
      // ------------------------ STATIC -------------------------
   private:
      /// <summary>Duplicates the input string.</summary>
      /// <param name="str">The string.</param>
      /// <returns>new wide char array</returns>
      /*static wchar* CopyText(const wstring& str)
      {
         wchar* buf = new wchar[str.length()+1];
         StringCchCopy(buf, str.length()+1, str.c_str());
         return buf;
      }*/

      // --------------------- PROPERTIES ------------------------
	   
      // ---------------------- ACCESSORS ------------------------			
   public:
      
      // ----------------------- MUTATORS ------------------------
   public:
      /// <summary>Sets the text</summary>
      /// <param name="str">The string.</param>
      void  SetText(const wstring& str)
      {
         StringCchCopy(Text.get(), cchTextMax, str.c_str());
      }

      // -------------------- REPRESENTATION ---------------------
   protected:
      CharArrayPtr  Text;
   };

NAMESPACE_END2(GUI,Controls)