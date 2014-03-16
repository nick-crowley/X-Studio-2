#pragma once



namespace Logic
{
   namespace Projects
   {
      /// <summary></summary>
      class LogicExport ScriptRevision
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
         ScriptRevision(const wstring& title, Path path, const wstring& content, const ScriptFile& script)
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
      public:
#ifdef LOGIC_COMPILER_FIX
         operator bool() const 
            { throw AlgorithmException(HERE, L"LogicLibrary compiler-fix code executed"); }
         const ScriptRevision& operator==(const ScriptRevision& r) const 
            { throw AlgorithmException(HERE, L"LogicLibrary compiler-fix code executed"); }
#endif
         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         Path      FullPath;       // Full path of document when committed
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
