#pragma once
//
#include <regex>


namespace Logic
{
   namespace Language
   {
      class LanguageString;

      
      /// <summary>Occurs when an invalid regular expression is detected</summary>
      class LogicExport RegularExpressionException : public ExceptionBase
      {
      public:
         /// <summary>Create a RegularExpressionException from an stl regex exception</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="err">regex error</param>
         RegularExpressionException(wstring  src, const regex_error&  err) 
            : ExceptionBase(src, L"Regular expression error: " + GuiString::Convert(err.what(), CP_ACP))
         {}
      };


      /// <summary></summary>
      class LogicExport StringResolver
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
         static const wregex DefaultMarker;
         static const wregex FullMarker;
         static const wregex RemoveComment;

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
