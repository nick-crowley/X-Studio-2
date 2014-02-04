#pragma once

#include "Common.h"
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "ScriptCommand.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Script error</summary>
         class ErrorToken : public TokenBase
         {
         public:
            /// <summary>Create error for token</summary>
            ErrorToken(const wstring& msg, UINT line, const ScriptToken& t) : TokenBase(t), Message(msg), Line(line)
            {}
            /// <summary>Create error for entire line</summary>
            ErrorToken(const wstring& msg, UINT line, const CHARRANGE& r) : TokenBase(r.cpMin, r.cpMax), Message(msg), Line(line)
            {}
            /// <summary>Create error for character range</summary>
            ErrorToken(const wstring& msg, UINT line, UINT start, UINT end) : TokenBase(start,end), Message(msg), Line(line)
            {}

            const wstring Message;
            const UINT    Line;
         };

         /// <summary>Vector of error tokens</summary>
         class ErrorArray : public vector<ErrorToken> 
         {
         public:
            /// <summary>Create empty array</summary>
            ErrorArray()
            {}
            /// <summary>Create array from single token</summary>
            ErrorArray(const ErrorToken& e) {
               push_back(e);
            }

            /// <summary>Add an error to the array</summary>
            ErrorArray& operator+=(const ErrorToken& t)
            {
               push_back(t);
               return *this;
            }
         };

         class CommandNode;
            
         /// <summary>Shared pointer to a parse tree node</summary>
         class CommandNodePtr : public shared_ptr<CommandNode> 
         {
         public:
            CommandNodePtr() : shared_ptr<CommandNode>(nullptr)
            {}
            CommandNodePtr(CommandNode* node) : shared_ptr<CommandNode>(node)
            {}
         };
            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class CommandNode 
         {
            // ------------------------ TYPES --------------------------
         public:
            typedef vector<CommandNodePtr>     NodeList;
            typedef NodeList::const_iterator   NodeIterator;

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNode();
            CommandNode(CommandNode* parent, CommandNode* target);
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line);
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line);
            ~CommandNode();

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            PROPERTY_GET(BranchLogic,Logic,GetBranchLogic);

            // ---------------------- ACCESSORS ------------------------		
         public:
            BranchLogic   GetBranchLogic() const;
            void          Print(int depth = 0) const;
            void          Verify(const ScriptFile& script, ErrorArray& errors) const;
               
         private:
            bool          Contains(BranchLogic l) const;
            void          EnumLabels(ErrorArray& errors) const;
            NodeIterator  Find(BranchLogic l) const;
            NodeIterator  Find(const CommandNode* child) const;
            CommandNode*  FindNextSibling() const;
            CommandNode*  FindAncestor(BranchLogic l) const;
            void          VerifyLogic(ErrorArray& errors) const;
            void          VerifyParameters(const ScriptFile& script, ErrorArray& errors) const;
               
            // ----------------------- MUTATORS ------------------------
         public:
            CommandNodePtr Add(CommandNodePtr node);
            void           Compile(ScriptFile& script);
            void           Populate(ScriptFile& script);
            
         private:
            void           CompileParameters(ScriptFile& script);
            void           InsertJump(NodeIterator pos, CommandNode* target);
            void           LinkCommands();
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandNode*    Parent;           // Parent node
            NodeList        Children;         // Child commands

            ParameterArray     Parameters,    // script parameters in display order
                               Postfix;       // expression parameters in postfix order
            CommandSyntaxRef   Syntax;        // command syntax
            const CommandNode* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
            UINT               Index;         // 0-based standard codearray index

            Conditional     Condition;        // Conditional
            const UINT      LineNumber;       // 1-based line number
            const CHARRANGE Extent;           // Start/end character offsets
            // Debug
            wstring         LineText;         // Debug: line text
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
