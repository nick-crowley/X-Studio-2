#pragma once

namespace Logic
{
   namespace Scripts
   {
      class ScriptCommand;

      /// <summary>List of script commands</summary>
      class LogicExport CommandList : public list<ScriptCommand>
      {
      public:
         /// <summary>Find command by index.</summary>
         /// <param name="index">The index.</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         ScriptCommand& operator[](UINT index)
         {
            UINT i = 0;

            // Linear search
            for (auto pos = begin(); pos != end(); ++pos)
               if (index == i++)
                  return *pos;

            // Invalid index
            throw IndexOutOfRangeException(HERE, index, size());
         }
      };

   }
}

using namespace Logic::Scripts;
