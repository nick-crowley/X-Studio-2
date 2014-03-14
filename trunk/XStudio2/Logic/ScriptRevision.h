#pragma once
#include "Common.h"


namespace Logic
{
   namespace Projects
   {
      /// <summary></summary>
      class ScriptRevision
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty revision.</summary>
         ScriptRevision()
         {}
         /// <summary>Create script revision.</summary>
         /// <param name="title">Revision title.</param>
         /// <param name="path">Script path.</param>
         /// <param name="content">Script content.</param>
         /// <param name="script">script properties.</param>
         ScriptRevision(const wstring& title, IO::Path path, const wstring& content, const ScriptFile& script)
            : Date(_time64(nullptr)),
              Title(title), 
              FullPath(path), 
              Content(content), 
              ScriptName(script.Name), 
              Description(script.Description), 
              Game(script.Game), 
              CommandID(script.CommandID.Int),
              Version(script.Version)
         {}
         virtual ~ScriptRevision()
         {}

         DEFAULT_COPY(ScriptRevision);	// Default copy semantics
         DEFAULT_MOVE(ScriptRevision);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         IO::Path      FullPath;       // Full path of document when committed
         wstring       Title,          // Revision description
                       Content;        // Revision text
         COleDateTime  Date;           // Revision date

         wstring      ScriptName,      // Script name
                      Description;     // Script description
         GameVersion  Game;            // Script engine version
         UINT         CommandID,       // Script command ID
                      Version;         // Script version
      };


   }
}

using namespace Logic::Projects;
