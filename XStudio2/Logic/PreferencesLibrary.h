#pragma once

//#include "Common.h"  Included in stdafx.h

namespace Logic
{
   /// <summary>Generate get/set methods+property for a preference</summary>
   /// <param name="valType">Property type</param>
   /// <param name="funcType">Registry function to use, must be Bool, Int or String</param>
   /// <param name="name">Name of preference/property</param>
   /// <param name="defVal">Default value when does not exist</param>
   #define PREFERENCE_PROPERTY(valType,funcType,name,defVal) \
      public: \
         __declspec(property(get=Get##name,put=Set##name)) valType name; \
         \
         valType  Get##name() const { \
            return Get##funcType(L#name, defVal); \
         } \
         void  Set##name(valType val) const { \
            Set##funcType(L#name, val); \
         }

   /// <summary>Generate get/set methods+property for a preference</summary>
   /// <param name="valueType">Property type</param>
   /// <param name="storageType">Whether stored as int, string</param>
   /// <param name="funcType">Registry function to use, must be Bool, Int or String</param>
   /// <param name="name">Name of preference/property</param>
   /// <param name="defaultVal">Default value when does not exist</param>
   #define PREFERENCE_PROPERTY2(valueType,storageType,funcType,name,defaultVal) \
      public: \
         __declspec(property(get=Get##name,put=Set##name)) valueType name; \
         \
         valueType  Get##name() const { \
            return (valueType)Get##funcType(L#name, (storageType)defaultVal); \
         } \
         void  Set##name(valueType val) const { \
            Set##funcType(L#name, (storageType)val); \
         }

   /// <summary>Generate get/set methods+property for a enum based preference</summary>
   /// <param name="valueType">Property type</param>
   /// <param name="name">Name of preference/property</param>
   /// <param name="defaultVal">Default value when does not exist</param>
   #define PREFERENCE_PROPERTY_ENUM(valueType,name,defaultVal)       PREFERENCE_PROPERTY2(valueType,int,Int,name,defaultVal)

   /// <summary>Generate get/set methods+property for a list based preference</summary>
   /// <param name="valueType">List element type</param>
   /// <param name="funcType">Registry function to use, must be StringList</param>
   /// <param name="name">Name of preference/property</param>
   #define PREFERENCE_PROPERTY_LIST(valType,funcType,name)\
      public: \
         __declspec(property(get=Get##name,put=Set##name)) list<valType> name; \
         \
         list<valType>  Get##name() const { \
            return Get##funcType(L#name); \
         } \
         void  Set##name(const list<valType>& val) const { \
            Set##funcType(L#name, val); \
         }

   /// <summary></summary>
   class PreferencesLibrary
   {
      // ------------------------ TYPES --------------------------
   private:

      // --------------------- CONSTRUCTION ----------------------
   private:
      PreferencesLibrary();
   public:
      virtual ~PreferencesLibrary();

      DEFAULT_COPY(PreferencesLibrary);	// Default copy semantics
      DEFAULT_MOVE(PreferencesLibrary);	// Default move semantics

      // ------------------------ STATIC -------------------------
   public:
      static PreferencesLibrary  Instance;

      // --------------------- PROPERTIES ------------------------
   public:
      /// <summary>Show large menus throughout app</summary>
      PREFERENCE_PROPERTY(bool,Bool,LargeMenus,false);

      /// <summary>Show large toolbars throughout app</summary>
      PREFERENCE_PROPERTY(bool,Bool,LargeToolbars,false);

      /// <summary>Show line numbers in the script view</summary>
      PREFERENCE_PROPERTY(bool,Bool,ShowLineNumbers,true);

      /// <summary>Show options in the find dialog</summary>
      PREFERENCE_PROPERTY(bool,Bool,ShowFindOptions,true);

      /// <summary>Game data folder</summary>
      PREFERENCE_PROPERTY(GuiString,String,GameDataFolder,L"");

      /// <summary>Game data version</summary>
      PREFERENCE_PROPERTY_ENUM(GameVersion,GameDataVersion,GameVersion::TerranConflict);

      /// <summary>Find dialog search terms</summary>
      PREFERENCE_PROPERTY_LIST(wstring,StringList,SearchTerms);

      /// <summary>Find dialog replace terms</summary>
      PREFERENCE_PROPERTY_LIST(wstring,StringList,ReplaceTerms);

      // ---------------------- ACCESSORS ------------------------			
   private:
      /// <summary>Get boolean preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">default value</param>
      /// <returns></returns>
      bool  GetBool(const wchar* name, bool defaultValue) const
      {
         return theApp.GetProfileInt(L"Settings", name, defaultValue) != FALSE;
      }

      /// <summary>Get integer preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">default value</param>
      /// <returns></returns>
      int  GetInt(const wchar* name, int defaultValue) const
      {
         return theApp.GetProfileInt(L"Settings", name, defaultValue);
      }
      
      /// <summary>Get string preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">default value</param>
      /// <returns></returns>
      wstring  GetString(const wchar* name, const wchar* defaultValue) const
      {
         return (LPCWSTR)theApp.GetProfileString(L"Settings", name, defaultValue);
      }

      /// <summary>Set boolean preference</summary>
      /// <param name="name">name</param>
      /// <param name="value">value</param>
      /// <returns></returns>
      void  SetBool(const wchar* name, bool value) const
      {
         theApp.WriteProfileInt(L"Settings", name, value ? TRUE : FALSE);
      }

      /// <summary>Set integer preference</summary>
      /// <param name="name">name</param>
      /// <param name="value">value</param>
      /// <returns></returns>
      void  SetInt(const wchar* name, int value) const
      {
         theApp.WriteProfileInt(L"Settings", name, value);
      }
      
      /// <summary>Set string preference</summary>
      /// <param name="name">name</param>
      /// <param name="value">value</param>
      /// <returns></returns>
      void  SetString(const wchar* name, const wstring& value) const
      {
         theApp.WriteProfileString(L"Settings", name, value.c_str());
      }
      
      /// <summary>Get preference stored as list of strings</summary>
      /// <param name="name">name</param>
      /// <returns>List of strings, empty if does not exist</returns>
      list<wstring>  GetStringList(const wchar* name) const
      {
         CSettingsStore s(FALSE, TRUE);
         list<wstring>  values;
         GuiString      key = theApp.GetProfileSectionPath(L"Settings", name);

         // DEBUG:
         //Console << "Reading from key " << key << ENDL;

         // Open key
         if (s.Open(key.c_str()))
         {
            CString str;
            UINT    i = 0;
            
            // Read values until not found
            while (s.Read(GuiString(L"Item%d",i++).c_str(), str))
               values.push_back((const wchar*)str);

            // Cleanup
            s.Close();
         }

         // Return values/empty-list
         return values;
      }
      
      /// <summary>Set preference stored as list of strings</summary>
      /// <param name="name">name</param>
      /// <param name="values">values</param>
      /// <exception cref="Logic::Win32Exception">Registry error<exception>
      void  SetStringList(const wchar* name, const list<wstring>& values) const
      {
         CSettingsStore ss(FALSE, FALSE);
         GuiString      key = theApp.GetProfileSectionPath(L"Settings", name);

         // Create/Open key
         if (!ss.CreateKey(key.c_str()))
            throw Win32Exception(HERE, GuiString(L"Unable to create registry key '%s'", key.c_str()));

         // Write list
         UINT val = 0;
         for (auto& str : values)
         {
            auto entry = GuiString(L"Item%d",val++);
            if (!ss.Write(entry.c_str(), str.c_str()))
               throw Win32Exception(HERE, GuiString(L"Unable to write '%s' to registry key '%s\\%s'", str.c_str(), key.c_str(), entry.c_str()));
         }

         // Cleanup
         ss.Close();
      }

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   
   private:
   };

   /// <summary>Preferences library singleton access</summary>
   #define  PrefsLib   PreferencesLibrary::Instance
}

using namespace Logic;
