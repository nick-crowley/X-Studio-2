#pragma once
#include "Stream.h"

using namespace Logic::IO;

namespace Logic
{
   namespace IO
   {
      /// <summary>Defines how a file is opened or created</summary>
      enum class FileMode : DWORD
      { 
         CreateNew = CREATE_NEW,                // Create new, fail if exists
         CreateAlways = CREATE_ALWAYS,          // Create new, overwrite if exists
         OpenAlways = OPEN_ALWAYS,              // Open file, create if does not exist
         OpenExisting = OPEN_EXISTING,          // Open file, fail if does not exist
         TruncateExisting = TRUNCATE_EXISTING   // Truncate existing, fail if does not exist
      };

      /// <summary>Defines read/write access on a stream</summary>
      enum class FileAccess : DWORD 
      { 
         Read = GENERIC_READ,                      // Read access
         Write = GENERIC_WRITE,                    // Write access
         ReadWrite = GENERIC_READ|GENERIC_WRITE    // Read/Write access
      };

      /// <summary>Defines attributes of files and/or their streams</summary>
      enum class FileAttribute : DWORD 
      { 
         ReadOnly = FILE_ATTRIBUTE_READONLY,				               //   
         Hidden = FILE_ATTRIBUTE_HIDDEN,				                  //   
         System = FILE_ATTRIBUTE_SYSTEM,				                  //   
         Directory = FILE_ATTRIBUTE_DIRECTORY,				            //   
         Archive = FILE_ATTRIBUTE_ARCHIVE,				               //   
         Device = FILE_ATTRIBUTE_DEVICE,				                  //   
         Normal = FILE_ATTRIBUTE_NORMAL,				                  //   
         Temporary = FILE_ATTRIBUTE_TEMPORARY,				            //   
         SparseFile = FILE_ATTRIBUTE_SPARSE_FILE,				         //   
         ReparsePoint = FILE_ATTRIBUTE_REPARSE_POINT,				      //   
         Compressed = FILE_ATTRIBUTE_COMPRESSED,				         //   
         Offline = FILE_ATTRIBUTE_OFFLINE,				               //   
         ContentNotIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,		//   
         Encrypted = FILE_ATTRIBUTE_ENCRYPTED,				            //   
         Virtual = FILE_ATTRIBUTE_VIRTUAL,				               //   
         
         WriteThrough = FILE_FLAG_WRITE_THROUGH,				         // 
         Overlapped = FILE_FLAG_OVERLAPPED,				               // 
         NoBuffering = FILE_FLAG_NO_BUFFERING,				            // 
         RandomAccess = FILE_FLAG_RANDOM_ACCESS,				         // 
         SequentialScan = FILE_FLAG_SEQUENTIAL_SCAN,				      // 
         DeleteOnClose = FILE_FLAG_DELETE_ON_CLOSE,				      // 
         BackupSemantics = FILE_FLAG_BACKUP_SEMANTICS,				   // 
         PosixSemantics = FILE_FLAG_POSIX_SEMANTICS,				      // 
         OpenReparsePoint = FILE_FLAG_OPEN_REPARSE_POINT,				// 
         OpenNoRecall = FILE_FLAG_OPEN_NO_RECALL,				         // 
         FirstPipeInstance = FILE_FLAG_FIRST_PIPE_INSTANCE,				// 
      };

      /// <summary>Enables bitwise combinations of file attribute flags.</summary>
      /// <param name="a">flag.</param>
      /// <param name="b">another flag</param>
      /// <returns>Bitwise combination of both flags</returns>
      FileAttribute operator|(const FileAttribute& a, const FileAttribute& b);

      /// <summary>Defines how a stream is shared</summary>
      enum class FileShare : DWORD 
      { 
         AllowRead = FILE_SHARE_READ,        // Permit others to read
         AllowWrite = FILE_SHARE_WRITE,      // Permit others to write
         AllowDelete = FILE_SHARE_DELETE,    // Permit others to delete
         AllowNone = 0                       // Permit nothing
      };

      /// <summary>Provides stream access to files on disc</summary>
      class LogicExport FileStream : public Stream
      {
      public:
         // --------------------- CONSTRUCTION ----------------------

         FileStream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         FileStream(Path path, FileMode mode, FileAccess access, FileAttribute attr, FileShare share = FileShare::AllowRead);
         ~FileStream();

         NO_MOVE(FileStream);
         NO_COPY(FileStream);

         // ------------------------ STATIC -------------------------

         static CharArrayPtr  ConvertFileBuffer(StreamPtr  s, DWORD&  Length);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         bool  CanRead() const   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek() const   { return true; }
         bool  CanWrite() const  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         DWORD GetLength();
         DWORD GetPosition() const;

         // ----------------------- MUTATORS ------------------------

         void  Close();
         void  Flush();
         void  SafeClose();
         void  Seek(LONG  offset, SeekOrigin  mode);
         void  SetLength(DWORD  length);

         DWORD Read(BYTE* buffer, DWORD length);
         DWORD Write(const BYTE* buffer, DWORD length);
      
         // -------------------- REPRESENTATION ---------------------

      protected:
         Path       FullPath;
         FileMode   Mode;
         FileAccess Access;
         FileShare  Share;

      private:
         HANDLE     Handle;
      };

   }
}