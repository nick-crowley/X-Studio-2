#pragma once

//#include "Common.h"  Included in stdafx.h

namespace Logic
{
   /// <summary>Generate get/set methods+property for a preference</summary>
   /// <param name="valType">Property type</param>
   /// <param name="funcType">Registry function to use, must be Bool, Int or String</param>
   /// <param name="name">Name of preference/property</param>
   /// <param name="defVal">Default value when does not exist</param>
   /// <returns></returns>
   #define PREFERENCE_PROPERTY(valType,funcType,name,defVal)   public: __declspec(property(get=Get##name,put=Set##name)) valType name;   \
                                                               valType Get##name()  const           { return Get##funcType(L#name, defVal); }  \
                                                               void    Set##name(valType val) const { Set##funcType(L#name, val); }

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
      PREFERENCE_PROPERTY(bool,Bool,LargeMenus,false);
      PREFERENCE_PROPERTY(bool,Bool,LargeToolbars,false);
      PREFERENCE_PROPERTY(bool,Bool,ShowLineNumbers,true);

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
      void  SetString(const wchar* name, const wchar* value) const
      {
         theApp.WriteProfileString(L"Settings", name, value);
      }

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   
   private:
   };

   /// <summary>Preferences library singleton access</summary>
   #define  PrefsLib   PreferencesLibrary::Instance
}

using namespace Logic;
