#pragma once

#include "Common.h"
#include "DescriptionFile.h"
#include "BackgroundWorker.h"

namespace Logic
{
   namespace Language
   {
      //class DescriptionFile;

      /// <summary></summary>
      class DescriptionLibrary
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary></summary>
         typedef pair<UINT,GameVersion>  CmdID;

         /// <summary></summary>
         class CommandCollection : public map<CmdID, CommandDescription>
         {
         public:
            /// <summary>Adds a command description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(const CommandDescription& d)
            {
               return insert( value_type(CmdID(d.ID,d.Version), d) ).second;
            }
         };

         /// <summary></summary>
         typedef pair<KnownPage,UINT>  ConstID;

         /// <summary></summary>
         class ConstantCollection : public map<ConstID, ConstantDescription>
         {
         public:
            /// <summary>Adds a command description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(const ConstantDescription& d)
            {
               return insert( value_type(ConstID(d.Page,d.ID), d) ).second;
            }
         };

         /// <summary></summary>
         class MacroCollection : public map<wstring, DescriptionMacro>  
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            MacroCollection()
            {}

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Add a macro to the collection</summary>
            /// <param name="m">The macro.</param>
            /// <returns></returns>
            bool  Add(const DescriptionMacro& m)
            {
               return insert(value_type(m.Name, m)).second;
            }

            /// <summary>Tries to find a macro</summary>
            /// <param name="id">macro name</param>
            /// <param name="out">macro if found, otherwise nullptr</param>
            /// <returns></returns>
            bool  TryFind(const wstring& id, const DescriptionMacro* &out) const
            {
               auto it = find(id);
               out = (it != end() ? &it->second : nullptr);
               return out != nullptr;
            }
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         DescriptionLibrary();
         virtual ~DescriptionLibrary();

         DEFAULT_COPY(DescriptionLibrary);	// Default copy semantics
         DEFAULT_MOVE(DescriptionLibrary);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         static DescriptionLibrary  Instance;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(WorkerData* data);

      private:
         void  Add(DescriptionFile& f);

         // -------------------- REPRESENTATION ---------------------
      public:
         CommandCollection  Commands;
         ConstantCollection Constants;
         MacroCollection    Macros;

      private:
      };

      /// <summary>Description library singleton access</summary>
      #define DescriptionLib     DescriptionLibrary::Instance
   }

}

using namespace Logic::Language;
