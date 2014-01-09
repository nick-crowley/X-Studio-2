
#include "stdafx.h"
#include "LegacySyntaxReader.h"


namespace Logic
{
   namespace Scripts
   {
      namespace IO
      {

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create reader using an input stream</summary>
      /// <param name="src">The input</param>
      LegacySyntaxReader::LegacySyntaxReader(StreamPtr src) : StringReader(src)
      {
      }

      /// <summary>Move reader</summary>
      /// <param name="r">The source</param>
      LegacySyntaxReader::LegacySyntaxReader(LegacySyntaxReader&& r) : StringReader(std::move(r))
      {
      }

      LegacySyntaxReader::~LegacySyntaxReader()
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire syntax file</summary>
      /// <returns></returns>
      SyntaxFile   LegacySyntaxReader::ReadFile()
      {
         CommandSyntax::Declaration  d;
         SyntaxFile   file;

         // Read syntax blocks
         while (ReadSyntax(d))
            file.Commands.Add( CommandSyntax(d) );

         return file;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Resolve a command group from the name</summary>
      /// <param name="name">The group name</param>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidValueException">Unknown group</exception>
      CommandGroup  LegacySyntaxReader::LookupCommandGroup(const WCHAR* name)
      {
         for (int i = 0; i < COMMAND_GROUP_COUNT; i++) {
            if (StrCmp(szCommandGroups[i], name) == 0)
               return (CommandGroup)i;
         }

         // Unrecognised group
         throw InvalidValueException(HERE, GuiString(L"Unrecognised command group '%s' on line %d", name, LineNumber));
      }

      /// <summary>Resolve a parameter type from the name</summary>
      /// <param name="name">The parameter type name</param>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidValueException">Unknown parameter type</exception>
      ParameterType  LegacySyntaxReader::LookupParameterType(const WCHAR* name)
      {
         for (int i = 0; i < PARAMETER_SYNTAX_COUNT; i++) {
            if (StrCmp(szParameterSyntax[i], name) == 0)
               return (ParameterType)i;
         }

         // Unrecognised group
         throw InvalidValueException(HERE, GuiString(L"Unrecognised parameter type '%s' on line %d", name, LineNumber) );
      }

      /// <summary>Reads the next syntax block</summary>
      /// <param name="dec">Command declaration</param>
      /// <returns>true if successful, false on EOF</returns>
      /// <exception cref="Logic::FileFormatException">Missing syntax component</exception>
      /// <exception cref="Logic::InvalidValueException">Unknown command group / parameter type</exception>
      bool  LegacySyntaxReader::ReadSyntax(CommandSyntax::Declaration& dec)
      {
         wstring line;

         // Init
         dec = CommandSyntax::Declaration();

         // Group
         if (!TryReadLine(line))
            return false;  // EOF Reached
         dec.Group = LookupCommandGroup(line.c_str());

         // Game versions
         RequireLine(line, L"compatibility flags");
         
         if (line.find(L"X2") != wstring::npos)
            dec.Versions |= (UINT)GameVersion::Threat;
         if (line.find(L"X3R") != wstring::npos)
            dec.Versions |= (UINT)GameVersion::Reunion;
         if (line.find(L"X3TC") != wstring::npos)
            dec.Versions |= (UINT)GameVersion::TerranConflict;
         if (line.find(L"X3AP") != wstring::npos)
            dec.Versions |= (UINT)GameVersion::AlbionPrelude;

         // ID
         RequireLine(line, L"ID");
         dec.ID = _wtoi(line.c_str());

         // URL
         RequireLine(dec.URL, L"help URL");
         // Syntax
         RequireLine(dec.Syntax, L"syntax text");

         // Params
         while (RequireLine(line, L"parameter") && line != END_BLOCK)
            dec.Params.push_back( LookupParameterType(line.c_str()) );

         return true;
      }


      /// <summary>Reads the next line</summary>
      /// <param name="line">The line text</param>
      /// <param name="content">Meaning of line</param>
      /// <returns>true</returns>
      /// <exception cref="Logic::FileFormatException">Line Missing</exception>
      bool  LegacySyntaxReader::RequireLine(wstring& line, const WCHAR* content)
      {
         string ansi;

         // Ensure line exists
         if (!ReadLine(ansi))
            throw FileFormatException(HERE, GuiString(L"Missing %s on line %d", content, LineNumber));

         // Convert ansi->wide
         line = wstring(ansi.begin(), ansi.end());
         return true;
      }

      /// <summary>Tries to read the next line</summary>
      /// <param name="line">The line text</param>
      /// <returns>true if found, false otherwise</returns>
      bool  LegacySyntaxReader::TryReadLine(wstring& line)
      {
         string ansi;

         // Ensure line exists
         if (!ReadLine(ansi))
            return false;

         // Convert ansi->wide
         line = wstring(ansi.begin(), ansi.end());
         return true;
      }

      
      // Hard-coded Command group names, used only for resolving parameter syntax from the syntax definitions file
      const WCHAR*  LegacySyntaxReader::szCommandGroups[COMMAND_GROUP_COUNT] =  
      {
         TEXT("ARRAY"),		         // CG_ARRAY
         TEXT("CUSTOM"),		      // CG_CUSTOM
         TEXT("FLEET"),		         // CG_FLEET
         TEXT("FLOW_CONTROL"),		// CG_FLOW_CONTROL
         TEXT("GAME_ENGINE"),		   // CG_GAME_ENGINE
         TEXT("GRAPH"),		         // CG_GRAPH
         TEXT("MACRO"),		         // CG_MACRO
         TEXT("MARINE"),		      // CG_MARINE
         TEXT("MATHS"),		         // CG_MATHS
         TEXT("MERCHANT"),		      // CG_MERCHANT
         TEXT("NON_PLAYER"),		   // CG_NON_PLAYER
         TEXT("OBJECT_ACTION"),		// CG_OBJECT_ACTION
         TEXT("OBJECT_PROPERTY"),	// CG_OBJECT_PROPERTY
         TEXT("PASSENGER"),		   // CG_PASSENGER
         TEXT("PILOT"),		         // CG_PILOT
         TEXT("PLAYER"),		      // CG_PLAYER
         TEXT("SCRIPT_PROPERTY"),	// CG_SCRIPT_PROPERTY
         TEXT("SHIP_ACTION"),		   // CG_SHIP_ACTION
         TEXT("SHIP_PROPERTY"),		// CG_SHIP_PROPERTY
         TEXT("SHIP_TRADE"),		   // CG_SHIP_TRADE
         TEXT("SHIP_WING"),		   // CG_SHIP_WING
         TEXT("STATION_PROPERTY"),	// CG_STATION_PROPERTY
         TEXT("STATION_TRADE"),		// CG_STATION_TRADE
         TEXT("STOCK_EXCHANGE"),		// CG_STOCK_EXCHANGE
         TEXT("STRING"),		      // CG_STRING
         TEXT("SYSTEM_PROPERTY"),	// CG_SYSTEM_PROPERTY
         TEXT("UNIVERSE_DATA"),		// CG_UNIVERSE_DATA
         TEXT("UNIVERSE_PROPERTY"),	// CG_UNIVERSE_PROPERTY
         TEXT("USER_INTERFACE"),		// CG_USER_INTERFACE
         TEXT("WAR"),               // GC_WAR
         TEXT("WARE_PROPERTY"),		// CG_WARE_PROPERTY
         TEXT("WEAPON_PROPERTY"),	// CG_WEAPON_PROPERTY
         TEXT("HIDDEN")             // CG_HIDDEN
      };

      // Hard-coded parameter syntax, used only for resolving parameter syntax from the syntax definitions file
      const WCHAR*  LegacySyntaxReader::szParameterSyntax[PARAMETER_SYNTAX_COUNT] = 
      {
         TEXT("Var"),
         TEXT("Label Name"),
         TEXT("String"),
         TEXT("Number"),
         TEXT("Condition"),
         TEXT("Parameter"),
         TEXT("Label Number"),
         TEXT("Script Name"),
         TEXT("Comment"),
         TEXT("Value"),
         TEXT("Var/Number"),
         TEXT("Var/String"),
         TEXT("Var/Station"),
         TEXT("Var/Ware"),
         TEXT("Var/Sector"),
         TEXT("RetVar"),
         TEXT("Var/Race"),
         TEXT("Var/Station Type"),
         TEXT("Var/Stations Serial"),
         TEXT("Var/Ship Type"),
         TEXT("RefObj"),
         TEXT("Var/Ship"),
         TEXT("Var/Ship/Station"),
         TEXT("Var/Class"),
         TEXT("Var/Transport Class"),
         TEXT("@RetVar/IF"),
         TEXT("RetVar/IF"),
         TEXT("Var/Ship Type/Station Type"),
         TEXT("RetVar/IF/START"),
         TEXT("Relation"),
         TEXT("Expression"),
         TEXT("Object Command"),
         TEXT("Object Signal"),
         TEXT("Object Command/Signal"),
         TEXT("Flight Retcode"),
         TEXT("Var/Ship owned by Player"),
         TEXT("Var/Station owned by Player"),
         TEXT("Var/Station/Carrier owned by Player"),
         TEXT("Var/Ship/Station owned by Player"),
         TEXT("Var/Warpgate"),
         TEXT("Var/Station/Carrier"),
         TEXT("Var/Station/Carrier to dock at"),
         TEXT("Var/All Wares"),
         TEXT("Var/Environment"),
         TEXT("Var/Script Data Type"),
         TEXT("Var/Array"),
         TEXT("Var/Station and Resource"),
         TEXT("Var/Station and Product"),
         TEXT("Var/Station and Ware"),
         TEXT("Var/Homebase and Resource"),
         TEXT("Var/Homebase and Product"),
         TEXT("Var/Homebase and Ware"),
         TEXT("Var/Sector Position"),
         TEXT("Var/Constant"),
         TEXT("Var/Asteroid"),
         TEXT("Var/Flying Ware"),
         TEXT("Var/Jumpdrive Gate"),
         TEXT("Var/Ship and Ware"),
         TEXT("Var/Ware of Ship"),
         TEXT("Var/Quest"),
         TEXT("--NONE--"),
         TEXT("Var/Wing"),
         TEXT("Script Reference Type"),
         TEXT("Var/Boolean"),
         TEXT("--NONE--"),
         TEXT("Var/Wing Command"),
         TEXT("Var/Passenger"),
         TEXT("Var/Ship and Passenger"),
         TEXT("Var/Passenger Of Ship"),
         TEXT("Var/Sector for Jumpdrive"),
         TEXT("Var/Fleet Commander"),
         TEXT("Var/Global Parameter")
      };      

      }
   }
}
