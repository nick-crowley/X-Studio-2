#pragma once
#include "Common.h"
#include "CommandSyntax.h"

namespace Logic
{
   namespace Scripts
   {

      /// <summary>Command syntax collection, organised by ID</summary>
      class SyntaxCollection : private multimap<UINT, CommandSyntax>
      {
      public:
         bool  Add(CommandSyntax&& s) { insert(SyntaxCollection::value_type(s.ID, s)); return true; }
      };

      
      /// <summary></summary>
      class SyntaxFile
      {
      public:
         /// <summary></summary>
         class TypeCollection : private map<wstring,ParameterType>
         {
         public:
            bool  Add(wstring& name, ParameterType type) { return insert(TypeCollection::value_type(name, type)).second; }
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
         TypeCollection    Types;

      private:
      };

   }
}

using namespace Logic::Scripts;
