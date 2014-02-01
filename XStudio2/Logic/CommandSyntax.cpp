#include "stdafx.h"
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "CommandHash.h"
#include "StringLibrary.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      /// <summary>'unknown' command syntax sentinel value</summary>
      const CommandSyntax  CommandSyntax::Unknown;

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create the 'unknown' command syntax sentinel value</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax() 
         : ID(CMD_UNRECOGNISED), 
           Versions((UINT)GameVersion::Threat|(UINT)GameVersion::Reunion|(UINT)GameVersion::TerranConflict|(UINT)GameVersion::AlbionPrelude),
           Type(CommandType::Standard),
           Group(CommandGroup::HIDDEN)
      {
      }

      /// <summary>Create command syntax from a declaration</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax(const Declaration& d)
         : Group(d.Group), ID(d.ID), Versions(d.Versions), URL(d.URL), Text(d.Syntax), Type(d.Type), Parameters(d.Params), Hash(GenerateHash(d.Syntax))
      {
      }

      CommandSyntax::~CommandSyntax()
      {
      }

      // -------------------------------- STATIC METHODS -------------------------------

      /// <summary>Generates hash from syntax, used when sorting syntax for display</summary>
      /// <param name="syntax">The syntax.</param>
      /// <returns></returns>
      wstring CommandSyntax::GenerateHash(const wstring& syntax)
      {
         CommandLexer lex(syntax, true);
         return CommandHash(lex.begin(), lex.end()).Hash;
      }

      /// <summary>Get command group name</summary>
      GuiString  GetString(CommandGroup g)
      {
         return GuiString(IDS_FIRST_COMMAND_GROUP + (UINT)g);
      }

      /// <summary>Post-Increment command group</summary>
      CommandGroup operator++(CommandGroup& g, int)
      {
         CommandGroup ret = g;
         return (++g, ret);
      }

      /// <summary>Pre-Increment command group</summary>
      CommandGroup& operator++(CommandGroup& g)
      {
         return g = (CommandGroup)((UINT)g + 1);
      }

      /// <summary>Operate on command groups as integers</summary>
      UINT operator-(const CommandGroup& a, const CommandGroup& b)
      {
         return (UINT)a - (UINT)b;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Get populated display text</summary>
      /// <returns></returns>
      /// <exception cref="Logic::PageNotFoundException">Parameter types Page does not exist</exception>
      /// <exception cref="Logic::StringNotFoundException">Parameter type string does not exist</exception>
      wstring  CommandSyntax::GetDisplayText() const
      {
         CommandLexer lex(Text, false);
         wstring      output(Text);

         // Iterate backwards thru tokens
         for (auto tok = lex.Tokens.rbegin(); tok < lex.Tokens.rend(); ++tok)
         {
            if (tok->Type == TokenType::Variable)
            {
               // Lookup parameter name
               auto param = Parameters[tok->Text[1]-48];
               GuiString name(L"<%s>", GetString(param.Type).c_str());

               // Replace $n marker with parameter text
               output.replace(tok->Start, tok->Length, name.c_str());
            }
         }

         return output;
      }

      /// <summary>Gets the parameter syntax sorted by display index</summary>
      /// <returns></returns>
      ParamSyntaxArray  CommandSyntax::GetParametersByDisplay() const
      {
         ParamSyntaxArray params;
         vector<int>      order;
         
         //transform(Parameters.begin(), Parameters.end(), back_inserter(order), [](const ParameterSyntax& p)->int { return p.DisplayIndex; });
         for (const auto& p : Parameters)
            order.push_back(p.DisplayIndex);

         // Sort by DisplayIndex (Ascending)
         sort(order.begin(), order.end(), less<int>()); 
         //sort(params.begin(), params.end(), [](const ParameterSyntax& a, const ParameterSyntax& b)->bool { return a.DisplayIndex < b.DisplayIndex; });
         
         for (int index : order)
            params.push_back(Parameters[index]);

         return params;
      }

      /// <summary>Query command ID</summary>
      bool  CommandSyntax::Is(UINT id) const
      {
         return ID == id;
      }

      /// <summary>Query command type</summary>
      bool  CommandSyntax::Is(CommandType t) const
      {
         return Type == t;
      }

      /// <summary>Determines whether the command is compatible with a game</summary>
      /// <param name="v">The game version</param>
      /// <returns></returns>
      bool  CommandSyntax::IsCompatible(GameVersion v) const
      {
         return (Versions & (UINT)v) != 0;
      }

      /// <summary>Determines whether command is a keyword.</summary>
      /// <returns></returns>
      bool  CommandSyntax::IsKeyword() const
      {
         switch (ID)
         {
         case CMD_END:
         case CMD_ELSE:
         case CMD_BREAK:
         case CMD_END_SUB:
         case CMD_RETURN:
         case CMD_CONTINUE:
         case CMD_GOTO_SUB:
         case CMD_GOTO_LABEL:  return true;
         default:              return false;
         }
      }

      /// <summary>Determines whether command is a script call</summary>
      /// <returns></returns>
      bool  CommandSyntax::IsScriptCall() const
      {
         return find_if(Parameters.begin(), Parameters.end(), [](const ParameterSyntax& s) {return s.Usage == ParameterUsage::ScriptName;} ) != Parameters.end();
      }      

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      
   }
}

