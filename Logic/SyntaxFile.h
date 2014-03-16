#pragma once

#include "CommandSyntax.h"

namespace Logic
{
   namespace Scripts
   {
      class SyntaxFile;

      

      /// <summary></summary>
      class SyntaxFile
      {
      public:
         /// <summary>Command syntax collection organised by ID</summary>
         class CommandCollection : public multimap<UINT, CommandSyntax>
         {
         public:
            void  Add(CommandSyntax& s)
            { 
               insert(value_type(s.ID, s)); 
            }
         };

         /// <summary>Maps names of parameter types to their ID</summary>
         class TypeCollection : public map<wstring,ParameterType>
         {
         public:
            bool  Add(wstring& name, ParameterType type) 
            { 
               return insert(value_type(name, type)).second; 
            }
         }; 

         /// <summary>Maps names of command groups to their ID</summary>
         class GroupCollection : public map<wstring,CommandGroup>
         {
         public:
            bool  Add(wstring& name, CommandGroup group) 
            { 
               return insert(value_type(name, group)).second; 
            }
         }; 

         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxFile();
         virtual ~SyntaxFile();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         CommandCollection  Commands;
         GroupCollection    Groups;
         TypeCollection     Types;

      private:
      };

   }
}

using namespace Logic::Scripts;
