#include "stdafx.h"
#include "FileStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Opens a file stream</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">Unable to create/open file</exception>
      FileStream::FileStream(Path path, FileMode mode, FileAccess access, FileShare share) : FullPath(path), Mode(mode), Access(access), Share(share) 
      {
         // Open file
         Handle = CreateFile(path.c_str(), (DWORD)access, (DWORD)share, NULL, (DWORD)mode, FILE_ATTRIBUTE_NORMAL, NULL);

         if (Handle == INVALID_HANDLE_VALUE) 
         {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
               throw FileNotFoundException(HERE, path);
            else if (GetLastError() == ERROR_PATH_NOT_FOUND)
               throw DirectoryNotFoundException(HERE, path.Folder);
            else
               throw IOException(HERE, SysErrorString());
         }
      }

      //FileStream::FileStream(const FileStream& s) : FullPath(s.FullPath), Mode(s.Mode), Access(s.Access), Share(s.Share) 
      //{
      //   // Duplicate handle
      //   if (!DuplicateHandle(GetCurrentProcess(), s.Handle, GetCurrentProcess(), &Handle, NULL, TRUE, DUPLICATE_SAME_ACCESS))
      //      throw IOException(HERE, SysErrorString());
      //}

      /// <summary>Closes the stream without throwing</summary>
      FileStream::~FileStream() 
      {
         SafeClose();
      }

      // ------------------------------- STATIC  METHODS ------------------------------

      /// <summary>Converts a text file into a WCHAR buffer based on the byte ordering mark, if any</summary>
      /// <param name="s">The input stream</param>
      /// <param name="length">Stream length on input, character length on output</param>
      /// <returns>Wide char buffer</returns>
      /// <exception cref="Logic::IOException">Conversion failed</exception>
      CharArrayPtr  FileStream::ConvertFileBuffer(StreamPtr  s, DWORD&  Length)
      {
         BYTE         utf8[3] = { 0xEF, 0xBB, 0xBF },    // UTF-8 byte ordering header
                      utf16[2] = { 0xFF, 0xFE };         // UTF-16 byte ordering header
         UINT         newLength;
         CharArrayPtr output;

         // Read entire file
         auto input = s->ReadAllBytes();

         // UTF16: Convert byte array -> wchar array
         if (Length > 2 && memcmp(input.get(), utf16, 2) == 0)
         {
            // Conv byte length -> char length  (minus 2byte BOM)
            newLength = (Length -= 2) / 2;

            // Copy exluding 2 byte BOM
            output = CharArrayPtr(new WCHAR[newLength+1]);
            memcpy(output.get(), input.get()+2, Length);
         }
         // UTF8: Convert UTF8 to UTF16
         if (Length > 3 && memcmp(input.get(), utf8, 3) == 0)
         {
            // Calculate UTF16 length (minus 3byte BOM)
            newLength = 2 * (Length -= 3);

            // Copy excluding 3 byte BOM
            output = CharArrayPtr(new WCHAR[newLength+1]);
            newLength = MultiByteToWideChar(CP_UTF8, NULL, (char*)input.get()+3, Length, output.get(), newLength);
         }
         // ASCII: Convert ASCII to UTF16
         else
         {
            // Simple double in length
            newLength = 2*Length;
            output = CharArrayPtr(new WCHAR[newLength+1]);
            newLength = MultiByteToWideChar(1250, NULL, (char*)input.get(), Length, output.get(), newLength);
         }

         // Failed: Throw
         if (Length > 0 && (Length=newLength) == 0)
            throw IOException(HERE, SysErrorString());

         // Null terminate + return
         output.get()[newLength] = '\0';
         return output;
      }


      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the stream.</summary>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::Close()
      {
         if (Handle != INVALID_HANDLE_VALUE) 
         {
            if (!CloseHandle(Handle))
               throw IOException(HERE, SysErrorString());

            Handle = INVALID_HANDLE_VALUE;
         }
      }

      /// <summary>Flushes any data to the stream.</summary>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::Flush()
      {
         if (!FlushFileBuffers(Handle))
            throw IOException(HERE, SysErrorString());
      }

      
      /// <summary>Gets the file length</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::GetLength()
      { 
         LARGE_INTEGER  size = {0LL};

         // Lookup file size
         if (!GetFileSizeEx(Handle, &size))
            throw IOException(HERE, SysErrorString());

         return (DWORD)size.QuadPart;
      }
      
      /// <summary>Gets the current seek position.</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::GetPosition() const
      {
         DWORD  position = 0;
         
         // Get position from null move
         if ((position=SetFilePointer(Handle, 0, NULL, (DWORD)SeekOrigin::Current)) == INVALID_SET_FILE_POINTER)
            throw IOException(HERE, SysErrorString());

         return position;
      }

      /// <summary>Closes the stream without throwing.</summary>
      void  FileStream::SafeClose()
      {
         if (Handle != INVALID_HANDLE_VALUE) 
         {
            CloseHandle(Handle);
            Handle = INVALID_HANDLE_VALUE;
         }
      }

      /// <summary>Seeks to the specified offset.</summary>
      /// <param name="offset">The offset.</param>
      /// <param name="mode">The seek origin</param>
      /// <exception cref="Logic::NotSupportedException">Stream is not seekable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::Seek(LONG  offset, SeekOrigin  mode)
      {
         // Check access
         if (!CanSeek())
            throw NotSupportedException(HERE, GuiString(ERR_NO_SEEK_ACCESS));

         // Set position 
         if (SetFilePointer(Handle, offset, NULL, (DWORD)mode) == INVALID_SET_FILE_POINTER)
            throw IOException(HERE, SysErrorString());
      }

      /// <summary>Sets the length of the file</summary>
      /// <param name="length">The length.</param>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::SetLength(DWORD  length)
      {
         DWORD  position = GetPosition();

         // Seek to desired position and set EOF
         Seek(length, SeekOrigin::Begin);
         if (!SetEndOfFile(Handle))
            throw IOException(HERE, SysErrorString());

         // Restore original position
         Seek(position, SeekOrigin::Begin);
      }

      /// <summary>Reads from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not readable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanRead())
            throw NotSupportedException(HERE, GuiString(ERR_NO_READ_ACCESS));

         // Read bytes
         DWORD count = 0;
         if (!ReadFile(Handle, buffer, length, &count, NULL))
            throw IOException(HERE, SysErrorString());

         // Return bytes read
         return count;
      }

      /// <summary>Writes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not writeable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanWrite())
            throw NotSupportedException(HERE, GuiString(ERR_NO_WRITE_ACCESS));

         // Write bytes
         DWORD count = 0;
         if (!WriteFile(Handle, buffer, length, &count, NULL))
            throw IOException(HERE, SysErrorString());

         // Return bytes written
         return count;
      }

      
		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------

   }
}