#include "stdafx.h"
#include "BackupFile.h"
#include "BackupFileWriter.h"
#include "XFileInfo.h"

namespace Logic
{
   namespace Projects
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      BackupFile::BackupFile(BackupType t) : Type(t)
      {
      }

      BackupFile::~BackupFile()
      {}

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Clears this instance.</summary>
      void  BackupFile::Clear()
      {
         Revisions.Clear();
      }

      /// <summary>Removes a revision.</summary>
      /// <param name="index">Zero-based index.</param>
      /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
      void  BackupFile::Remove(UINT index)
      {
         Revisions.Remove(index);
      }
      
      /// <summary>Saves the backup file.</summary>
      /// <param name="path">The path.</param>
      /// <param name="f">The f.</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::GZipException">Unable to inititalise stream</exception>
      /// <exception cref="Logic::IOException">Unable to create file</exception>
      void  BackupFile::Write(const Path& path) const
      {
         auto tmp = TempPath().RenameExtension(L".zip");

         // Write to a temp file to prevent destroying all revisions if case of failure
         BackupFileWriter w(XFileInfo(tmp).OpenWrite(L"revisions.xml"));
         w.WriteFile(*this);
         w.Close();

         // Success: Copy file
         if (!MoveFileEx(tmp.c_str(), path.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            throw IOException(HERE, L"Unable to overwrite backup file: " + SysErrorString());
      }

      /// <summary>Find a revision by index</summary>
      /// <param name="index">The index</param>
      /// <returns></returns>
      /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
      ScriptRevision&  BackupFile::operator[](UINT index) 
      {
         return Revisions.FindByIndex(index);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
