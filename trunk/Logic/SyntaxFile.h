#pragma once

#include "CommandSyntax.h"

namespace Logic
{
   namespace Scripts
   {
      class SyntaxFile;

      /// <summary>Represents an X-Studio 1 or 2 syntax file</summary>
      class LogicExport SyntaxFile
      {
      public:
         /// <summary>Command syntax collection organised by ID</summary>
         class CommandCollection : public multimap<UINT, CommandSyntax>
         {
         public:
            /// <summary>Adds command syntax.</summary>
            /// <param name="s">syntax.</param>
            void  Add(CommandSyntaxRef s)
            { 
               insert(value_type(s.ID, s)); 
            }
         };

         /// <summary>Maps names of parameter types to their ID</summary>
         class TypeCollection : public map<wstring,ParameterType>
         {
         public:
            /// <summary>Adds parameter type.</summary>
            /// <param name="name">name.</param>
            /// <param name="type">type.</param>
            /// <returns></returns>
            bool  Add(const wstring& name, ParameterType type)
            { 
               return insert(value_type(name, type)).second; 
            }

            /// <summary>Find parameter type</summary>
            /// <param name="name">name.</param>
            /// <returns></returns>
            /// <exception cref="Logic::GenericException">Missing</exception>
            ParameterType  operator[](const wstring& name)
            {
               // Lookup item
               auto pos = find(name);
               if (pos != end())
                  return pos->second;

               // Not found:
               throw GenericException(HERE, VString(L"Cannot find a parameter type '%s'", name.c_str()));
            }
         }; 

         /// <summary>Maps names of command groups to their ID</summary>
         class GroupCollection : public map<wstring,CommandGroup>
         {
         public:
            /// <summary>Adds command group.</summary>
            /// <param name="name">name.</param>
            /// <param name="group">group.</param>
            /// <returns></returns>
            bool  Add(const wstring& name, CommandGroup group) 
            { 
               return insert(value_type(name, group)).second; 
            }
            
            /// <summary>Find command group</summary>
            /// <param name="name">name.</param>
            /// <returns></returns>
            /// <exception cref="Logic::GenericException">Missing</exception>
            CommandGroup  operator[](const wstring& name)
            {
               // Lookup item
               auto pos = find(name);
               if (pos != end())
                  return pos->second;

               // Not found:
               throw GenericException(HERE, VString(L"Cannot find a command group '%s'", name.c_str()));
            }
         }; 

         // --------------------- CONSTRUCTION ----------------------
      public:
         SyntaxFile();
         SyntaxFile(const wstring& title, const wstring& version);
         virtual ~SyntaxFile();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets ident string</summary>
         /// <returns></returns>
         GuiString GetIdent() const
         {
            return VString(L"%s %s", Title.c_str(), Version.c_str());
         }

		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         CommandCollection  Commands;        // Command syntax
         GroupCollection    Groups;          // CommandGroup name->ID map
         TypeCollection     Types;           // ParameterType name->ID map

         wstring            Title,           // File title
                            Version;         // File version
      private:
      };

   }
}

using namespace Logic::Scripts;
