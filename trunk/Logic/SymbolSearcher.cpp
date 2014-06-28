#include "stdafx.h"
#include "CommandTree.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create visitor for searching for all instances of a symbol</summary>
         /// <param name="name">symbol name</param>
         /// <param name="type">symbol type</param>
         /// <param name="results">On return, this contains the results</param>
         SymbolSearcher::SymbolSearcher(const wstring& name, SymbolType type, SymbolList& results) 
            : Name(name), Type(type), Results(results)
         {
            Results.clear();
         }
         
         /// <summary>Nothing</summary>
         SymbolSearcher::~SymbolSearcher()
         {
         }
         
         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Searches for all instances of a symbol</summary>
         /// <param name="n">Node</param>
         void  SymbolSearcher::VisitNode(CommandNode* n) 
         {
            bool comment = n->CmdComment || n->Is(CMD_COMMENT);

            // Perform Search
            switch (Type)
            {
            // Label: Search for 'define label', 'goto label', 'gosub label'
            case SymbolType::Label:
               if ((n->Is(CMD_DEFINE_LABEL) || n->Is(CMD_GOTO_LABEL) || n->Is(CMD_GOTO_SUB)) && !n->Parameters.empty()) 
                  Results.push_back(Symbol(n->Parameters[0].Token, SymbolType::Label, n->LineNumber, n->LineText, comment));
               break;

            // Variable: Search all commands
            case SymbolType::Variable:
               for (const auto& p : n->Parameters)
                  if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String && p.Token.ValueText == Name)
                     Results.push_back(Symbol(p.Token, SymbolType::Variable, n->LineNumber, n->LineText, comment));
               break;
            }
         }
         
         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
