#include "stdafx.h"
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "StringLibrary.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create the 'unknown' command syntax sentinel value</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax() 
         : ID(CMD_NONE), 
           Versions((UINT)GameVersion::Threat|(UINT)GameVersion::Reunion|(UINT)GameVersion::TerranConflict|(UINT)GameVersion::AlbionPrelude),
           Type(CommandType::Standard),
           Group(CommandGroup::HIDDEN)
      {
      }

      /// <summary>Create command syntax from a declaration</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax(const Declaration& d)
         : Group(d.Group), ID(d.ID), Versions(d.Versions), URL(d.URL), Text(d.Syntax), Type(d.Type), Parameters(d.Params)
      {
      }

      CommandSyntax::~CommandSyntax()
      {
      }

      // -------------------------------- STATIC METHODS -------------------------------

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
               GuiString name(L"<%s>", StringLib.Find(KnownPage::PARAMETER_TYPES, (UINT)param.Type).Text.c_str());

               // Replace $n marker with parameter text
               output.replace(tok->Start, tok->Length(), name.c_str());
            }
         }

         return output;
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

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      
   }
}

