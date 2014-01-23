
#pragma once
#include "stdafx.h"

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
      LVItem(UINT item, const wstring& txt, UINT group, UINT flags = LVIF_TEXT | LVIF_GROUPID) : Text(txt)
      {
         ZeroMemory((LVITEM*)this, sizeof(LVITEM));

         mask     = flags;
         iItem    = item;
         iGroupId = group;
         pszText  = (WCHAR*)Text.c_str();
      }
      
      // ------------------------ STATIC -------------------------
      
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
      
      // -------------------- REPRESENTATION ---------------------
      
   private:
      wstring Text;
   };

NAMESPACE_END2(GUI,Controls)