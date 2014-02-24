#pragma once

#include "Common.h"
#include "DescriptionFile.h"
#include "BackgroundWorker.h"
#include "DescriptionParser.h"

namespace Logic
{
   namespace Language
   {
      /// <summary>Occurs when a tooltip is not found</summary>
      class DescriptionNotFoundException : public ExceptionBase
      {
      public:
         /// <summary>Create a DescriptionNotFoundException for a command</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="cmd">Command syntax</param>
         DescriptionNotFoundException(wstring  src, CommandSyntaxRef cmd) 
            : ExceptionBase(src, GuiString(L"No tooltip for '%s'", cmd.Text.c_str()))
         {}

         /// <summary>Create a DescriptionNotFoundException for a script object</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="obj">Object</param>
         DescriptionNotFoundException(wstring  src, const ScriptObject& obj) 
            : ExceptionBase(src, GuiString(L"No tooltip for %s script object '%s'", GetString(obj.Group).c_str(), obj.Text.c_str()))
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
            bool  Contains(CommandSyntaxRef cmd) const
            {
               // Iterate thru compatible versions
               for (GameVersion v : {GameVersion::Threat, GameVersion::Reunion, GameVersion::TerranConflict, GameVersion::AlbionPrelude})
                  // Lookup description
                  if ((cmd.Versions & (UINT)v) && find(CommandID(cmd.ID, v)) != end())
                     return true;

               return false;
            }

            /// <summary>Finds a script command description.</summary>
            /// <param name="id">command ID</param>
            /// <param name="ver">game version</param>
            /// <returns>Description text</returns>
            /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
            /// <exception cref="Logic::Language::RegularExpressionException">RegEx error</exception>
            /// <exception cref="Logic::Language::DescriptionNotFoundException">Description not present</exception>
            wstring  Find(CommandSyntaxRef cmd) const
            {
               // Iterate thru compatible versions
               for (GameVersion v : {GameVersion::Threat, GameVersion::Reunion, GameVersion::TerranConflict, GameVersion::AlbionPrelude})
                  // Lookup description
                  if ((cmd.Versions & (UINT)v) && find(CommandID(cmd.ID, v)) != end())
                  {
                     // Parse and Populate description source
                     wstring src = find(CommandID(cmd.ID, v))->second.Text;
                     return DescriptionParser(src, cmd).Text;
                  }

               // Missing: Error
               throw DescriptionNotFoundException(HERE, cmd);
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
            /// <param name="obj">script object</param>
            /// <returns></returns>
            bool  Contains(const ScriptObject& obj) const
            {
               return find(ConstantID(obj.Group, obj.ID)) != end();
            }

            /// <summary>Finds a script object description</summary>
            /// <param name="obj">script object</param>
            /// <returns>Description text</returns>
            /// <exception cref="Logic::FileFormatException">Macro contains wrong number of parameters</exception>
            /// <exception cref="Logic::Language::RegularExpressionException">RegEx error</exception>
            /// <exception cref="Logic::Language::DescriptionNotFoundException">Description not present</exception>
            wstring  Find(const ScriptObject& obj) const
            {
               // Lookup object
               auto it = find(ConstantID(obj.Group, obj.ID));

               // Parse text
               if (it != end())
                  return DescriptionParser(it->second.Text).Text;

               // Missing: Error
               throw DescriptionNotFoundException(HERE, obj);
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
      private:
         DescriptionLibrary();
      public:
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
