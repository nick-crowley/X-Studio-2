#pragma once

#include "Common.h"
#include "XmlWriter.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class ScriptFileWriter : public XmlWriter
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptFileWriter(StreamPtr out);
         virtual ~ScriptFileWriter();

         NO_COPY(ScriptFileWriter);	   // No copy semantics
         NO_MOVE(ScriptFileWriter);	   // No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(const ScriptFile& sf);

      private:
         UINT  CalculateSize(const ScriptCommand& cmd);
         XmlElementPtr  WriteArray(XmlElementPtr parent, UINT size);
         void  WriteArgument(XmlElementPtr parent, const ScriptVariable& var);
         void  WriteArgument(XmlElementPtr parent, UINT index, const ScriptVariable& var);
         void  WriteCommand(XmlElementPtr parent, const ScriptCommand& cmd);
         void  WriteParameter(XmlElementPtr parent, const ScriptParameter& p);
         void  WriteInt(XmlElementPtr parent, int val);
         void  WriteString(XmlElementPtr parent, const wstring& val);
         void  WriteValue(XmlElementPtr parent, const ParameterValue& val);

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
