#pragma once
#include "ScriptToken.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Script error</summary>
         class LogicExport ErrorToken : public TokenBase
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
#ifdef LOGIC_LIB_BUILD_FIX
            ErrorToken() : Line(0) { THROW_LOGIC_LIB_BUILD_FIX; }
#endif
            /// <summary>Create error for token</summary>
            ErrorToken(const GuiString& msg, UINT line, const ScriptToken& tok) 
               : TokenBase(tok), Message(msg), Text(tok.Text), Line(line)
            {}
            /// <summary>Create error for a character range</summary>
            ErrorToken(const GuiString& msg, UINT line, const GuiString& txt, const CHARRANGE& r) 
               : TokenBase(r.cpMin, r.cpMax), Message(msg), Text(txt), Line(line)
            {}
            /// <summary>Create error for a character range</summary>
            ErrorToken(const GuiString& msg, UINT line, const GuiString& txt, UINT start, UINT end) 
               : TokenBase(start,end), Message(msg), Text(txt), Line(line)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------
#ifdef LOGIC_LIB_BUILD_FIX
         public:
            ErrorToken& operator=(const ErrorToken& r) const { THROW_LOGIC_LIB_BUILD_FIX; }
#endif
            // -------------------- REPRESENTATION ---------------------
         public:
            const GuiString Message,   // Error message
                            Text;      // Erroneous token/line text
            const UINT      Line;      // 1-base line number
         };

         /// <summary>Print error token to console</summary>
         ConsoleWnd&  operator<<(ConsoleWnd& c, const ErrorToken& e);

         /// <summary>Vector of error tokens</summary>
         class LogicExport ErrorArray : public vector<ErrorToken> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create empty array</summary>
            ErrorArray()
            {}
            /// <summary>Create array from single token</summary>
            ErrorArray(const ErrorToken& e) {
               push_back(e);
            }

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Add an error to the array</summary>
            ErrorArray& operator+=(const ErrorToken& t)
            {
               push_back(t);
               return *this;
            }

            // -------------------- REPRESENTATION ---------------------

         };

         /// <summary>Print error array to console</summary>
         ConsoleWnd&  operator<<(ConsoleWnd& c, const ErrorArray& errs);
      }
   }
}

using namespace Logic::Scripts::Compiler;
