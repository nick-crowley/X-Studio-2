#pragma once

#include "Common.h"
#include "DescriptionFile.h"
#include "BackgroundWorker.h"

namespace Logic
{
   namespace Language
   {
      /// <summary>Occurs when a tooltip is not found</summary>
      class DescriptionNotFoundException : public ExceptionBase
      {
      public:
         /// <summary>Create a DescriptionNotFoundException</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="msg">Message</param>
         DescriptionNotFoundException(wstring  src, wstring  msg) 
            : ExceptionBase(src, wstring(L"Invariant violated: ") + msg)
         {}
      };

      /// <summary></summary>
      class DescriptionLibrary
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary>Defines an association between command ID and version</summary>
         typedef pair<UINT,GameVersion>  CommandID;

         /// <summary>Collection of script command descriptions</summary>
         class CommandCollection : public map<CommandID, CommandDescription>
         {
         public:
            /// <summary>Adds a command description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(const CommandDescription& d)
            {
               return insert( value_type(CommandID(d.ID,d.Version), d) ).second;
            }

            /// <summary>Query whether command description exists</summary>
            /// <param name="id">command ID</param>
            /// <param name="ver">Lowest compatible game version</param>
            /// <returns></returns>
            bool  Contains(UINT id, GameVersion ver) const
            {
               return find(CommandID(id,ver)) != end();
            }
         };

         /// <summary>Defines an association between constant ID and page</summary>
         typedef pair<ScriptObjectGroup,UINT>  ConstantID;

         /// <summary>Collection of script object descriptions</summary>
         class ConstantCollection : public map<ConstantID, ConstantDescription>
         {
         public:
            /// <summary>Adds a script object description</summary>
            /// <param name="d">description</param>
            /// <returns></returns>
            bool  Add(const ConstantDescription& d)
            {
               return insert( value_type(ConstantID(d.Group,d.ID), d) ).second;
            }

            /// <summary>Query whether script object description exists</summary>
            /// <param name="group">script object group</param>
            /// <param name="id">object id</param>
            /// <returns></returns>
            bool  Contains(ScriptObjectGroup group, UINT id) const
            {
               return find(ConstantID(group,id)) != end();
            }
         };

         /// <summary>Collection of description macros</summary>
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
