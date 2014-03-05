#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class CompilerPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_COMPILER_PAGE };
   
   protected:
      /// <summary>Case Sensitive Variables property</summary>
      class CaseSensitiveVariablesProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'case sensitive variables' property</summary>
         /// <param name="page">Owner page.</param>
         CaseSensitiveVariablesProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Variable Names", L"", L"Change whether variables names are case sensitive")
         {
            AddOption(L"Case Sensitive", FALSE);
            AddOption(L"Case Insensitive", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.CaseSensitiveVariables ? 0 : 1));
         }

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(0);
         }

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>CheckArgumentNames property</summary>
      class CheckArgumentNamesProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'CheckArgumentNames' property</summary>
         /// <param name="page">Owner page.</param>
         CheckArgumentNamesProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Script-call Argument Names", L"", L"Check the name of arguments in script-calls")
         {
            AddOption(L"Check", FALSE);
            AddOption(L"Don't check", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.CheckArgumentNames ? 0 : 1));
         }
         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(0);
         }

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>CheckArgumentTypes property</summary>
      class CheckArgumentTypesProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'CheckArgumentTypes' property</summary>
         /// <param name="page">Owner page.</param>
         CheckArgumentTypesProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Script-call Argument Types", L"", L"Check the type of argument values passed via script-calls")
         {
            AddOption(L"Check", FALSE);
            AddOption(L"Don't check", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.CheckArgumentTypes ? 0 : 1));
         }
         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(0);
         }

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>UseDoIfSyntax property</summary>
      class UseDoIfSyntaxProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'UseDoIfSyntax' property</summary>
         /// <param name="page">Owner page.</param>
         UseDoIfSyntaxProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Do-If Conditionals", L"", L"Choose whether to use newer 'do-if' syntax or older 'skip-if-not' syntax by default")
         {
            AddOption(L"Use 'do-if'", FALSE);
            AddOption(L"Use 'skip-if-not'", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.UseDoIfSyntax ? 0 : 1));
         }

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(0);
         }

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>UseCppOperators property</summary>
      class UseCppOperatorsProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'UseCppOperators' property</summary>
         /// <param name="page">Owner page.</param>
         UseCppOperatorsProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Binary Operators", L"", L"Choose whether to use named operators (AND, OR, MOD) or C++ operators (&&, ||, %) by default")
         {
            AddOption(L"X³ Style", FALSE);
            AddOption(L"C++", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.UseCppOperators ? 1 : 0));
         }

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(1);
         }

         // ----------------------- MUTATORS ------------------------
      
         // -------------------- REPRESENTATION ---------------------
      };


      // --------------------- CONSTRUCTION ----------------------
   public:
      CompilerPage();    
      virtual ~CompilerPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(CompilerPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	   void Populate() override;
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      CaseSensitiveVariablesProperty*  CaseSensitiveVariables;
      CheckArgumentNamesProperty*      CheckArgumentNames;
      CheckArgumentTypesProperty*      CheckArgumentTypes;
      UseDoIfSyntaxProperty*           UseDoIfSyntax;
      UseCppOperatorsProperty*         UseCppOperators;
   };
   
NAMESPACE_END2(GUI,Preferences)

