#pragma once

#include "Common.h"
#include <regex>


namespace Logic
{
   namespace Language
   {
      class LanguageString;

      
      /// <summary>Occurs when an invalid regular expression is detected</summary>
      class RegularExpressionException : public ExceptionBase
      {
      public:
         /// <summary>Create a RegularExpressionException from an stl regex exception</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="err">regex error</param>
         RegularExpressionException(wstring  src, const regex_error&  err) 
            : ExceptionBase(src, L"Regular expression error: " + StringResource::Convert(err.what(), CP_ACP))
         {}
      };


      /// <summary></summary>
      class StringResolver
      {
         // ------------------------ TYPES --------------------------
      private:
         // --------------------- CONSTRUCTION ----------------------

      public:
         StringResolver(const LanguageString& str);
         virtual ~StringResolver();

         DEFAULT_COPY(StringResolver);	// Default copy semantics
         DEFAULT_MOVE(StringResolver);	// Default move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const wregex DefaultMarker, FullMarker, RemoveComment;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      protected:
         wstring  OnFullMarker(wsmatch& match);
         wstring  OnDefaultMarker(wsmatch& match);

      private:
         void  Parse();

         // -------------------- REPRESENTATION ---------------------
      public:
         const UINT  Page;
         wstring     Text;
         
      private:
         UINT  Position;
      };

   }
}

using namespace Logic::Language;
