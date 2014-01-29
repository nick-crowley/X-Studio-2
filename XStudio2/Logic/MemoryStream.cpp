#include "stdafx.h"
#include "MemoryStream.h"

namespace Logic
{
   namespace IO
   {
      MemoryStream::MemoryStream(BYTE* buffer, DWORD length, FileAccess access) : Access(access), Length(length), Buffer(buffer), DestroyOnClose(false), Position(0)
      {
         REQUIRED(buffer);
      }

      MemoryStream::MemoryStream(DWORD length, FileAccess access) : Access(access), Length(length), DestroyOnClose(true), Position(0)
      {
         Buffer = Alloc(Length);
      }

      MemoryStream::~MemoryStream() 
      {
         Close();
      }

      DWORD  MemoryStream::GetLength() const
      { 
         return Length;
      }
      
      DWORD  MemoryStream::GetPosition() const
      {
         return Position;
      }

      void  MemoryStream::Close()
      {
         if (DestroyOnClose && Buffer != NULL) 
         {
            delete [] Buffer;
            Buffer = NULL;
         }
      }

      void  MemoryStream::Flush()
      {
      }

      void  MemoryStream::Seek(DWORD  offset, SeekOrigin  mode)
      {
         switch (mode)
         {
         case SeekOrigin::Begin:     Position = (DWORD)offset;                break;
         case SeekOrigin::Current:   Position += (DWORD)offset;               break;
         case SeekOrigin::End:       Position = GetLength() - (DWORD)offset;  break;
         }
      }

      void  MemoryStream::SetLength(DWORD  newLength)
      {
         if (!DestroyOnClose)
            throw InvalidOperationException(HERE, L"cannot re-allocate an external buffer");

         // Re-allocate buffer
         BYTE* oldBuffer = Buffer;
         Buffer = Alloc(newLength);

         // Copy contents across
         memcpy(Buffer, oldBuffer, Length);
         Length = newLength;
         delete [] oldBuffer;

         // Ensure position is valid
         if (Position >= Length)
            Position = Length-1;
      }

      DWORD  MemoryStream::Read(BYTE* buf, DWORD length)
      {
         REQUIRED(buf);
         
         // Check access
         if (!CanRead())
            throw NotSupportedException(HERE, GuiString(ERR_NO_READ_ACCESS));

         // Copy contents from stream
         memcpy(buf, Buffer, length);
         return length;
      }


      DWORD  MemoryStream::Write(BYTE* buf, DWORD length)
      {
         REQUIRED(buf);
         
         // Check access
         if (!CanWrite())
            throw NotSupportedException(HERE, GuiString(ERR_NO_WRITE_ACCESS));

         // Copy contents to stream
         memcpy(Buffer, buf, length);
         return length;
      }

      
      BYTE* MemoryStream::Alloc(DWORD len)
      {
         if (len == 0)
            throw InvalidOperationException(HERE, L"Cannot allocate a zero length buffer");

         // Allocate and zero buffer
         BYTE* buf = new BYTE[len];
         ZeroMemory(buf, len);
         return buf;
      }


   }
}