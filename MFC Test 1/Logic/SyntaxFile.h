#pragma once
#include "Common.h"
#include "CommandSyntax.h"

namespace Library
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class SyntaxFile
      {
      public:
         class SyntaxCollection : multimap<UINT, CommandSyntax>
         {
            friend SyntaxFile;

         private:
            SyntaxCollection() {}

         public:
            bool  Add(CommandSyntax&& s) { insert(SyntaxCollection::value_type(s.ID, s)); return true; }
         };

         class TypeCollection : map<wstring,ParameterType>
         {
            friend SyntaxFile;

         private:
            TypeCollection() {}

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
