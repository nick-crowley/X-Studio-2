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
      const CommandSyntax  CommandSyntax::Unrecognised;

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create the 'unknown' command syntax sentinel value</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax() 
         : ID(CMD_UNRECOGNISED), 
           Versions((UINT)GameVersion::Threat|(UINT)GameVersion::Reunion|(UINT)GameVersion::TerranConflict|(UINT)GameVersion::AlbionPrelude),
           Type(CommandType::Standard),
           Group(CommandGroup::HIDDEN),
           Execution(ExecutionType::Serial)
      {
      }

      /// <summary>Create command syntax from a declaration</summary>
      /// <param name="d">Declaration</param>
      CommandSyntax::CommandSyntax(const Declaration& d)
         : ID(d.ID),
           URL(d.URL), 
           Type(d.Type),
           Text(d.Syntax), 
           Group(d.Group),  
           Versions(d.Versions), 
           Parameters(d.Params), 
           Execution(d.Execution),
           Hash(GenerateHash(d.Syntax))
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
         //typedef pair<UINT,UINT> Index;   // {Physical,Display}

         ParamSyntaxArray params;
         //vector<Index>    order;
         //
         //// Build array of {Physical,Display} tuples sorted by physical index
         //transform(Parameters.begin(), Parameters.end(), back_inserter(order), [](const ParameterSyntax& p)->Index 
         //{ 
         //   return Index(p.PhysicalIndex, p.DisplayIndex); 
         //});

         //// Build array by display index
         //for (const ParameterSyntax& ps : Parameters)
         //   params.push_back( Parameters[order[ps.PhysicalIndex].second] );

         // Re-order by display index (ascending)
         for (UINT i = 0; i < Parameters.size(); ++i)
         {
            auto p = find_if(Parameters.begin(), Parameters.end(), [i](const ParameterSyntax& ps) {return ps.DisplayIndex == i;});
            params.push_back(*p);
         }

         // DEBUG: Verify sizes equal
         if (params.size() != Parameters.size())
            throw AlgorithmException(HERE, L"");

         // DEBUG: Verify sorted by display index
         for (UINT i = 0; i < params.size(); i++)
            if (params[i].DisplayIndex != i)
            {
               wstring debug;
               for_each(params.begin(), params.end(), [&debug](ParameterSyntax& ps)
               {
                  debug += GuiString(L"physical %d : display %d : %s\n", ps.PhysicalIndex, ps.DisplayIndex, GetString(ps.Type).c_str());
               });
               throw AlgorithmException(HERE, GuiString(L"Parameter %d of %d has display index %d instead of %d : %s\n%s", 
                                                        i+1, params.size(), params[i].DisplayIndex, i, Text.c_str(), debug.c_str()));
            }

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

