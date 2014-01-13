#pragma once
#include "Common.h"
#include "CommandSyntax.h"

namespace Logic
{
   namespace Scripts
   {
      class SyntaxFile;

      /// <summary>Command syntax collection, organised by ID</summary>
      class SyntaxCollection : public multimap<UINT, CommandSyntax>
      {
      public:
         bool  Add(CommandSyntax&& s);
         void  Merge(SyntaxFile&& f);
      };

      /// <summary></summary>
      class SyntaxFile
      {
      public:
         /// <summary>Maps names of parameter types to their ID</summary>
         class TypeCollection : public map<wstring,ParameterType>
         {
         public:
            bool  Add(wstring& name, ParameterType type) { return insert(value_type(name, type)).second; }
         }; 

         /// <summary>Maps names of command groups to their ID</summary>
         class GroupCollection : public map<wstring,CommandGroup>
         {
         public:
            bool  Add(wstring& name, CommandGroup group) { return insert(value_type(name, group)).second; }
         }; 

         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxFile();
         virtual ~SyntaxFile();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

         //bool  Add(CommandSyntax&& s) { return false; }

		   // -------------------- REPRESENTATION ---------------------

         SyntaxCollection  Commands;
         GroupCollection   Groups;
         TypeCollection    Types;

      private:
      };

   }
}

using namespace Logic::Scripts;
