#include "stdafx.h"
#include "ScriptFile.h"
#include "RtfWriter.h"
#include "ScriptObjectLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create empty script-file.</summary>
      /// <param name="path">The path.</param>
      ScriptFile::ScriptFile(Path path) : FullPath(path), Version(0), Game(GameVersion::Threat), LiveData(false)
      {
      }


      ScriptFile::~ScriptFile()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get variable type string</summary>
      LogicExport GuiString  GetString(VariableType t)
      {
         return t == VariableType::Variable ? L"Variable"
              : t == VariableType::Argument ? L"Argument" 
                                            : L"Invalid";
      }

      /// <summary>Write script variable to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptVariable& v)
      {
         return c << "{ScriptVariable: Name=" << v.Name << " ID=" << v.ID << " ParamType=" << GetString(v.ParamType) << " Type=" << GetString(v.Type) << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Clears commands, labels and non-argument variables</summary>
      void  ScriptFile::Clear()
      {
         // Clear labels & commands
         Commands.Clear();
         Labels.clear();

         // Clear script-calls
         ScriptCalls.Clear();

         // Clear variables, keep arguments
         Variables.clear();
      }

      /// <summary>Finds and highlights the next match, if any</summary>
      /// <param name="start">starting offset</param>
      /// <param name="m">Match data</param>
      /// <returns>True if found, false otherwise</returns>
      bool  ScriptFile::FindNext(UINT start, MatchData& m)
      {
         int pos = GuiString::npos, len = 0;

         // Validate position 
         if (start >= OfflineBuffer.length())
            m.Clear();
         
         // RegEx: Find + determine match length
         else if (m.UseRegEx)
         {
            wsmatch matches;

            // Search remainder of text
            if (regex_search(OfflineBuffer.cbegin()+start, OfflineBuffer.cend(), matches, m.RegEx))  //, regex_constants::match_default))
            {
               pos = matches[0].first - OfflineBuffer.cbegin();
               len = matches[0].length();
            }
         }
         // Basic: Linear search
         else
         {
            pos = OfflineBuffer.Find(m.SearchTerm, start, m.MatchCase);
            len = m.SearchTerm.length();
         }
         
         // Found: Set location, length, line number, line text
         if (pos != GuiString::npos)
         {
            int line = count_if(OfflineBuffer.begin(), OfflineBuffer.begin()+pos, [](wchar ch) {return ch=='\n';} );
            m.SetMatch(pos, len, line+1, Commands.Input[line].Text);
         }
         else
            m.Clear();
         
         // Return whether matched
         return m.Matched;
      }

      /// <summary>Finds the name of the label that represents the scope of a line number</summary>
      /// <param name="line">The 1-based line number</param>
      /// <returns>Label name, or empty string if global</returns>
      wstring  ScriptFile::FindScope(UINT line)
      {
         // Sort labels into descending order
         vector<ScriptLabel> coll(Labels.begin(), Labels.end());
         sort(coll.begin(), coll.end(), [](ScriptLabel& a, ScriptLabel&b){ return a.LineNumber > b.LineNumber; });

         // Determine current scope
         auto scope = find_if(coll.begin(), coll.end(), [line](ScriptLabel& l) {return line >= l.LineNumber;} );
         return scope != coll.end() ? scope->Name : L"";
      }

      /// <summary>Gets all text in the offline buffer. Lines are separated by single '\n' chars</summary>
      /// <returns></returns>
      wstring  ScriptFile::GetAllText() const
      {
         return OfflineBuffer;
      }

      /// <summary>Gets the name of the object command.</summary>
      /// <returns>Name of command if any, otherwise empty string</returns>
      wstring  ScriptFile::GetCommandName() const
      {
         const ScriptObject* cmd;

         if (CommandID.Type == ValueType::String)
            return CommandID.String;

         else if (CommandID.Int != 0 && ScriptObjectLib.TryFind(ScriptObjectGroup::ObjectCommand, CommandID.Int, cmd))
            return cmd->Text;
         
         return L"";   
      }
      
      /// <summary>Replaces the current match</summary>
      /// <param name="m">Match data</param>
      /// <exception cref="Logic::AlgorithmException">Previous match no longer matches</exception>
      void  ScriptFile::Replace(MatchData& m)
      {
         wstring r;

         // Basic: Use replacement term
         if (!m.UseRegEx)
            r = m.ReplaceTerm;
         else
         {
            wsmatch matches;
         
            // RegEx: Format replacement expression
            if (regex_match(OfflineBuffer.cbegin()+m.Start, OfflineBuffer.cbegin()+m.End, matches, m.RegEx))
               r = matches.format(m.ReplaceTerm);
            else
               throw AlgorithmException(HERE, L"Previously matched text does not match regEx");
         }

         // Perform replacement
         OfflineBuffer.replace(m.Start, m.Length, r);

         // Refresh line text
         m.UpdateLineText( GetLineText(m.LineNumber-1) );
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Gets line text from the offline buffer</summary>
      /// <param name="line">Zero-based line number.</param>
      /// <returns></returns>
      wstring  ScriptFile::GetLineText(UINT line) const
      {
         wstring text;
         UINT ln = 0;

         // Manually extract updated line text
         for_each(OfflineBuffer.begin(), OfflineBuffer.end(), [&](wchar ch) {
            if (ch == '\n')
               ++ln;
            else if (ln == line)
               text += ch;
         });

         return text;
      }
   }
}
