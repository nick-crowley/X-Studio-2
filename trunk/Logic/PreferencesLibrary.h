#pragma once
#include <afxglobals.h>
#include <afxsettingsstore.h>

// Do not include common.h, this file included via stdafx.h


#define MACRO_REGION

namespace Logic
{
#ifdef MACRO_REGION

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
   #define PREFERENCE_PROPERTY_EX(valueType,storageType,funcType,name,defaultVal) \
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
   #define PREFERENCE_PROPERTY_ENUM(valueType,name,defaultVal)       PREFERENCE_PROPERTY_EX(valueType,int,Int,name,defaultVal)

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

#endif

   /// <summary></summary>
   class LogicExport PreferencesLibrary
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
      // Application/GUI:
      /// <summary>Show large menus throughout app</summary>
      PREFERENCE_PROPERTY(bool,Bool,LargeMenus,false)

      /// <summary>Show large toolbars throughout app</summary>
      PREFERENCE_PROPERTY(bool,Bool,LargeToolbars,false)

      /// <summary>Tool window font</summary>
      PREFERENCE_PROPERTY(LOGFONT,LogFont,ToolWindowFont,L"Regular");

      /// <summary>Tooltip font</summary>
      PREFERENCE_PROPERTY(LOGFONT,LogFont,TooltipFont,L"Tooltip");

      /// <summary>FullPaths of previously open documents</summary>
      PREFERENCE_PROPERTY_LIST(wstring,StringList,WorkspaceDocuments);


      // Script Editor:
      /// <summary>Script editor font</summary>
      PREFERENCE_PROPERTY(LOGFONT,LogFont,ScriptViewFont,L"Regular");

      /// <summary>Script editor line indentation (in characters)</summary>
      PREFERENCE_PROPERTY(int,Int,ScriptIndentation,3);

      /// <summary>Script editor tooltip display delay (seconds)</summary>
      PREFERENCE_PROPERTY(int,Int,ScriptTooltipDelay,3);

      /// <summary>Auto-commit documents upon save</summary>
      PREFERENCE_PROPERTY(bool,Bool,CommitOnSave,true);

      /// <summary>Increment script version upon save</summary>
      PREFERENCE_PROPERTY(bool,Bool,IncrementOnSave,true);
      
      /// <summary>Show line numbers in the script view</summary>
      PREFERENCE_PROPERTY(bool,Bool,ShowLineNumbers,true);

      /// <summary>Show tooltips in the script view</summary>
      PREFERENCE_PROPERTY(bool,Bool,ShowScriptTooltips,true);


      /// <summary>Script editor line numbers colour</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,LineNumberColour,RGB(255,255,255));

      /// <summary>Script editor background colour</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,BackgroundColour,RGB(0,0,0));

      /// <summary>Diff viewer alteration highlight</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,DifferenceHighlight,RGB(80,80,80));

      /// <summary>Refactor edit symbol highlight</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,RefactorHighlight,RGB(128,128,128));


      /// <summary>Syntax highlighting colour for arguments</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,ArgumentColour,RGB(255,128,40));

      /// <summary>Syntax highlighting colour for comments</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,CommentColour,RGB(128,128,128));

      /// <summary>Syntax highlighting colour for constants</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,ConstantColour,RGB(255,223,0));
      
      /// <summary>Syntax highlighting colour for game-objects</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,GameObjectColour,RGB(0,255,255));

      /// <summary>Syntax highlighting colour for keywords</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,KeywordColour,RGB(0,104,230));
      
      /// <summary>Syntax highlighting colour for labels</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,LabelColour,RGB(255,0,255));
      
      /// <summary>Syntax highlighting colour for null</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,NullColour,RGB(0,255,0));

      /// <summary>Syntax highlighting colour for numbers</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,NumberColour,RGB(255,0,0));

      /// <summary>Syntax highlighting colour for operators</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,OperatorColour,RGB(255,255,255));
      
      /// <summary>Syntax highlighting colour for script-objects</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,ScriptObjectColour,RGB(255,255,0));
      
      /// <summary>Syntax highlighting colour for strings</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,StringColour,RGB(255,0,0));

      /// <summary>Syntax highlighting colour for command text</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,TextColour,RGB(255,255,255));

      /// <summary>Syntax highlighting colour for variables</summary>
      PREFERENCE_PROPERTY_EX(COLORREF,int,Int,VariableColour,RGB(0,255,0));


      // Compiler:
      /// <summary>Enable background compiler</summary>
      PREFERENCE_PROPERTY(bool,Bool,BackgroundCompiler,true);

      /// <summary>Check name of arguments in varg script calls</summary>
      PREFERENCE_PROPERTY(bool,Bool,CheckArgumentNames,true);

      /// <summary>Static type checking of varg script calls</summary>
      PREFERENCE_PROPERTY(bool,Bool,CheckArgumentTypes,true);

      /// <summary>Use case senstive variable names</summary>
      PREFERENCE_PROPERTY(bool,Bool,CaseSensitiveVariables,true);

      /// <summary>Use 'do-if' syntax when translating 'skip-if-not' conditionals</summary>
      PREFERENCE_PROPERTY(bool,Bool,UseDoIfSyntax,true);

      /// <summary>Use C++ operators when translating operators</summary>
      PREFERENCE_PROPERTY(bool,Bool,UseCppOperators,false);


      // Find Dialog
      /// <summary>Show options in the find dialog</summary>
      PREFERENCE_PROPERTY(bool,Bool,ShowFindOptions,true);

      /// <summary>Find dialog search terms</summary>
      PREFERENCE_PROPERTY_LIST(wstring,StringList,SearchTerms);

      /// <summary>Find dialog replace terms</summary>
      PREFERENCE_PROPERTY_LIST(wstring,StringList,ReplaceTerms);


      // NewDocument dialog
      /// <summary>Last new document folder</summary>
      PREFERENCE_PROPERTY_EX(Path,LPCWSTR,String,NewDocumentFolder,L"");


      // Game Data:
      /// <summary>Continue loading game data if a language file fails to load</summary>
      PREFERENCE_PROPERTY(bool,Bool,SkipBrokenFiles,true);

      /// <summary>Game data folder</summary>
      PREFERENCE_PROPERTY_EX(Path,LPCWSTR,String,GameDataFolder,L"");

      /// <summary>Game data version</summary>
      PREFERENCE_PROPERTY_ENUM(GameVersion,GameDataVersion,GameVersion::TerranConflict);

      /// <summary>Game data language</summary>
      PREFERENCE_PROPERTY_ENUM(GameLanguage,GameDataLanguage,GameLanguage::English);

      // ---------------------- ACCESSORS ------------------------			
   private:
      /// <summary>Get boolean preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">default value</param>
      /// <returns></returns>
      bool  GetBool(const wchar* name, bool defaultValue) const
      {
         return AfxGetApp()->GetProfileInt(L"Settings", name, defaultValue) != FALSE;
      }

      /// <summary>Get font preference</summary>
      /// <param name="name">preference name</param>
      /// <param name="font">default font name</param>
      /// <returns></returns>
      LOGFONT  GetLogFont(const wchar* name, const wstring& font) const
      {
         LOGFONT lf;
         BYTE* buf = nullptr;
         UINT  len = 0;

         // Read value
         if (AfxGetApp()->GetProfileBinary(L"Settings", name, &buf, &len))
         {
            // Success: Extract data
            lf = *reinterpret_cast<LOGFONT*>(buf);
            delete [] buf;
         }
         // Regular
         else if (font == L"Regular")
            GetGlobalData()->fontRegular.GetLogFont(&lf);
         // Tooltip
         else if (font == L"Tooltip")
            GetGlobalData()->fontTooltip.GetLogFont(&lf);
         // Custom
         else
         {
            CFont f;
            // Failed/Missing: Create default font, size 10pt
            f.CreatePointFont(10*10, font.c_str());
            f.GetLogFont(&lf);
         }

         return lf;
      }

      /// <summary>Get integer preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">default value</param>
      /// <returns></returns>
      int  GetInt(const wchar* name, int defaultValue) const
      {
         return AfxGetApp()->GetProfileInt(L"Settings", name, defaultValue);
      }
      
      /// <summary>Get string preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">default value</param>
      /// <returns></returns>
      wstring  GetString(const wchar* name, const wchar* defaultValue) const
      {
         return (LPCWSTR)AfxGetApp()->GetProfileString(L"Settings", name, defaultValue);
      }

      /// <summary>Get path preference</summary>
      /// <param name="name">name</param>
      /// <param name="defaultValue">Ignored</param>
      /// <returns></returns>
      /*Path  GetPath(const wchar* name, const wchar* defaultValue) const
      {
         return (LPCWSTR)AfxGetApp()->GetProfileString(L"Settings", name, L"");
      }*/

      /// <summary>Set boolean preference</summary>
      /// <param name="name">name</param>
      /// <param name="value">value</param>
      /// <returns></returns>
      void  SetBool(const wchar* name, bool value) const
      {
         AfxGetApp()->WriteProfileInt(L"Settings", name, value ? TRUE : FALSE);
      }

      /// <summary>Set font preference</summary>
      /// <param name="name">name</param>
      /// <param name="font">font data</param>
      /// <returns></returns>
      void  SetLogFont(const wchar* name, LOGFONT font) const
      {
         AfxGetApp()->WriteProfileBinary(L"Settings", name, reinterpret_cast<BYTE*>(&font), sizeof(LOGFONT));
      }

      /// <summary>Set integer preference</summary>
      /// <param name="name">name</param>
      /// <param name="value">value</param>
      /// <returns></returns>
      void  SetInt(const wchar* name, int value) const
      {
         AfxGetApp()->WriteProfileInt(L"Settings", name, value);
      }
      
      /// <summary>Set string preference</summary>
      /// <param name="name">name</param>
      /// <param name="value">value</param>
      /// <returns></returns>
      void  SetString(const wchar* name, const wstring& value) const
      {
         AfxGetApp()->WriteProfileString(L"Settings", name, value.c_str());
      }
      
      /// <summary>Get preference stored as list of strings</summary>
      /// <param name="name">name</param>
      /// <returns>List of strings, empty if does not exist</returns>
      list<wstring>  GetStringList(const wchar* name) const
      {
         CSettingsStore s(FALSE, TRUE);
         list<wstring>  values;
         GuiString      key = GetProfileSectionPath(L"Settings", name);

         // DEBUG:
         //Console << "Reading from key " << key << ENDL;

         // Open key
         if (s.Open(key.c_str()))
         {
            CString str;
            UINT    i = 0;
            
            // Read values until not found
            while (s.Read(VString(L"Item%d",i++).c_str(), str))
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
         GuiString      key = GetProfileSectionPath(L"Settings", name);

         // Clear previous
         ss.DeleteKey(key.c_str());

         // Create/Open key
         if (!ss.CreateKey(key.c_str()))
            throw Win32Exception(HERE, VString(L"Unable to create registry key '%s'", key.c_str()));

         // Write list
         UINT val = 0;
         for (auto& str : values)
         {
            auto entry = VString(L"Item%d",val++);
            if (!ss.Write(entry.c_str(), str.c_str()))
               throw Win32Exception(HERE, VString(L"Unable to write '%s' to registry key '%s\\%s'", str.c_str(), key.c_str(), entry.c_str()));
         }

         // Cleanup
         ss.Close();
      }

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   private:
      /// <summary>Gets the registry path of section</summary>
      /// <param name="section">section name</param>
      /// <returns></returns>
      GuiString  GetProfileSectionPath(const wstring& section) const
      {
         return VString(L"SOFTWARE\\Bearware\\X-Studio II\\%s", section.c_str());
      }

      /// <summary>Gets the registry path of section</summary>
      /// <param name="section">section name</param>
      /// <param name="subsection">sub-section name</param>
      /// <returns></returns>
      GuiString  GetProfileSectionPath(const wstring& section, const wstring& subsection) const
      {
         return VString(L"SOFTWARE\\Bearware\\X-Studio II\\%s\\%s", section.c_str(), subsection.c_str());
      }
   };

   /// <summary>Preferences library singleton access</summary>
   #define  PrefsLib   PreferencesLibrary::Instance
}

using namespace Logic;
