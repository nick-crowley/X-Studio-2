
#include "stdafx.h"
#include "LegacySyntaxFileReader.h"


namespace Logic
{
   namespace IO
   {
      
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create reader using an input stream</summary>
      /// <param name="src">The input</param>
      LegacySyntaxFileReader::LegacySyntaxFileReader(StreamPtr src) : StringReader(src)
      {
      }

      /// <summary>Move reader</summary>
      /// <param name="r">The source</param>
      LegacySyntaxFileReader::LegacySyntaxFileReader(LegacySyntaxFileReader&& r) : StringReader(std::move(r))
      {
      }

      LegacySyntaxFileReader::~LegacySyntaxFileReader()
      {
      }

      // -------------------------------- STATIC METHODS -------------------------------

      /// <summary>Generates a parameter lists from a legacy parameter collection</summary>
      /// <param name="id">Command ID</param>
      /// <param name="syntax">The legacy format command syntax string</param>
      /// <param name="params">Parameter types in physical index order</param>
      /// <returns>New format Parameter array</returns>
      ParamSyntaxArray  LegacySyntaxFileReader::GenerateParams(UINT id, const wstring& syntax, const list<ParameterType>& params)
      {
         ParamSyntaxArray  output;
         vector<int>       displayIndicies(params.size()),
                           ordinals(params.size());
         UINT              displayIndex = 0,
                           index = 0;
         
         // Iterate thru '$n' syntax parameter markers
         for (auto it = find(syntax.begin(), syntax.end(), L'$'); it != syntax.end(); it = find(++it, syntax.end(), L'$'))
         {
            // Convert number character -> integer
            index = *(it+1)-48;

            // Store display index
            displayIndicies[index] = displayIndex++;  

            // Check for superscript ordinal indicator
            if (it+2 < syntax.end())
               switch (*(it+2))
               {
               case L'º': case 'o':  ordinals[index] = 0;  break;
               case L'¹': case 'x':  ordinals[index] = 1;  break;
               case L'²': case 'y':  ordinals[index] = 2;  break;
               case L'³': case 'z':  ordinals[index] = 3;  break;
               case L'ª': case 'a':  ordinals[index] = 4;  break;
               }
            else
               ordinals[index] = 0;
         }

         // Generate parameters
         index = 0;
         for (auto type : params)
         {
            ParameterSyntax::Declaration d(type, index, displayIndicies[index], ordinals[index], IdentifyUsage(id, index));

            // Store new syntax
            output.push_back( ParameterSyntax(d) );
            index++;
         }

         return output;
      }

      /// <summary>Generates the new format syntax string from the old</summary>
      /// <param name="syntax">The old syntax text</param>
      /// <returns></returns>
      wstring  LegacySyntaxFileReader::GenerateSyntax(const wstring& syntax)
      {
         wstring output(syntax);

         // Iterate thru '$n' syntax parameter markers
         for (auto it = find(output.begin(), output.end(), L'$'); it != output.end(); it = find(++it, output.end(), L'$'))
         {
            // Strip [1-2 character] superscript ordinal indicator, if any
            for (int pass = 0; pass<2 && it+2<output.end(); ++pass)
               switch (it[2])
               {
               case L'º': case 'o':  
               case L'¹': case 'x':  
               case L'²': case 'y':  
               case L'³': case 'z':  
               case L'ª': case 'a':  
                  output.erase(it+2);
                  break;
               }
         }

         return output;
      }

      /// <summary>Manually Identifies the execution type (hard-coded)</summary>
      /// <param name="id">The command ID</param>
      /// <param name="index">The physical parameter index</param>
      /// <returns>Usage</returns>
      ExecutionType  LegacySyntaxFileReader::IdentifyExecution(UINT id)
      {
         switch (id)
         {
         default:
            return ExecutionType::Serial;

         case CMD_START_COMMAND:             // START $0 command $1 : arg1=$2, arg2=$3, arg3=$4, arg4=$5
         case CMD_START_DELAYED_COMMAND:     // START $0 delayed command $1 : arg1=$2, arg2=$3, arg3=$4, arg4=$5
         case CMD_START_WING_COMMAND:        // START $0 wing command $1 : arg1=$2, arg2=$3, arg3=$4, arg4=$5
            return ExecutionType::Concurrent;

         case CMD_SPEAK_ARRAY:
         case CMD_SPEAK_TEXT:
         case CMD_CALL_SCRIPT:
            return ExecutionType::Either;
         }
      }

      /// <summary>Manually Identifies the parameter usage (hard-coded)</summary>
      /// <param name="id">The command ID</param>
      /// <param name="index">The physical parameter index</param>
      /// <returns>Usage</returns>
      ParameterUsage  LegacySyntaxFileReader::IdentifyUsage(UINT id, UINT index)
      {
         // Examine command ID
         switch (id)
         {
         // {ScriptName} @ {0}
         case CMD_CALL_SCRIPT:               // "$1 $2 call script $0 :"
         case CMD_REGISTER_AL_SCRIPT:                 // "al engine: register script=$0"
         case CMD_REGISTER_QUEST_SCRIPT:              // "register quest script $0 instance multiple=$1"
         case CMD_REGISTER_GOD_EVENT:                 // "register god event: script=$0 mask=$1"
         case CMD_UNREGISTER_AL_SCRIPT:               // "al engine: unregister script $0"
         case CMD_REMOVE_SCRIPT_OPTIONS:              // "remove script options: script=$0"  
            return index == 0 ? ParameterUsage::ScriptName : ParameterUsage::None;

         // {ScriptName} @ {1}
         case CMD_BEGIN_TASK_ARGS:                    // "$0 begin task $2 with script $1 and priority $3: arg1=$4o arg2=$5x arg3=$6y arg4=$7z arg5=$8a"
         case CMD_INTERRUPT_SCRIPT:                   // "$0 interrupt with script $1 and prio $2"
         case CMD_INTERRUPT_SCRIPT_ARGS:              // "$0 interrupt with script $1 and prio $2: arg1=$3 arg2=$4 arg3=$5 arg4=$6"
         case CMD_INTERRUPT_TASK_ARGS:                // "$0 interrupt task $2 with script $1 and prio $3: arg1=$4 arg2=$5 arg3=$6 arg4=$7"
   
         case CMD_SET_GLOBAL_SCRIPT_MAP:              // "global script map: set: key=$0, class=$3, race=$4, script=$1, prio=$2"
         case CMD_SET_SHIP_PRELOAD_SCRIPT:            // "set ship command preload script: command=$0 script=$1"
         case CMD_SET_WING_PRELOAD_SCRIPT:            // "set wing command preload script: wing command=$0 script=$1"

         case CMD_GET_HOTKEY_INDEX_1:                 // "$0 get hotkey index, script: $1"
         case CMD_GET_HOTKEY_INDEX_2:                 // "$0 get hotkey index, script name: $1"
         case CMD_CHECK_SCRIPT_EXISTS_1:              // "$0 does script exist: $1"
         case CMD_CHECK_SCRIPT_EXISTS_2:              // "$0 does script name exist: $1"

         case CMD_SEND_INCOMING_QUESTION:             // "send incoming question $0 to player: callback=$1"
         case CMD_SEND_INCOMING_QUESTION_EX:          // "send incoming question $0 to player: callback=$1 flags=$2"
            return index == 1 ? ParameterUsage::ScriptName : ParameterUsage::None;

         // {ScriptName} @ {2}
         case CMD_CALL_NAMED_SCRIPT:                   // "$1 $0 call named script: script=$2, $3, $4, $5, $6, $7"
         case CMD_LAUNCH_SCRIPT_ARGS:                 // "$0 launch named script: task=$1 scriptname=$2 prio=$3, $4, $5, $6, $7, $8"
         case CMD_IS_SCRIPT_ON_STACK:                 // "$1 $0 is script $2 on stack of task=$3"

         case CMD_CONNECT_SHIP_SCRIPT:                // "$0 connect ship command/signal $1 to script $2 with prio $3"
         case CMD_CONNECT_WING_SCRIPT:                // "$0 connect wing command/signal $1 to script $2 with prio $3"
         case CMD_ADD_SECONDARY_SIGNAL:               // "$0 add secondary signal: signal=$1, script=$2, prio=$3, name=$4"

         case CMD_SET_SHIP_UPGRADE_SCRIPT:            // "set script command upgrade: command=$0  upgrade=$1  script=$2"
         case CMD_SET_WING_UPGRADE_SCRIPT:            // "set script command upgrade: wing command=$0 upgrade=$1 script=$2"
         case CMD_SET_GLOBAL_WING_SCRIPT_MAP:         // "global script map for wings: key=$0 race=$1 script=$2 prio=$3"

         case CMD_REGISTER_HOTKEY_1:                  // "$0 register hotkey $1 to call script $2"
         case CMD_REGISTER_HOTKEY_2:                  // "$0 register hotkey $1 to call script name $2" 
            return index == 2 ? ParameterUsage::ScriptName : ParameterUsage::None;

         // {ScriptName} @ {3}
         case CMD_ADD_EVENT_LISTENER:                 // "$0 add event listener: quest/event=$1 objevent=$2 script=$3"
         case CMD_SET_GLOBAL_SECONDARY_MAP:           // "global secondary signal map: add signal=$0 race=$1 class=$2 script=$3 prio=$4 name=$5"
            return index == 2 ? ParameterUsage::ScriptName : ParameterUsage::None;


         // {Page,String} @ {0, 1}
         case CMD_SET_NEWS_ARTICLE_STATE:             // "set state of news article page=$0 id=$1 to $2"
         case CMD_WRITE_LOGBOOK:                      // "write to player logbook: printf: pageid=$0 textid=$1, $2, $3, $4, $5, $6"
         case CMD_WRITE_LOG:                          // "write to log file $0  append=$1  printf: pageid=$2 textid=$3, $4, $5, $6, $7, $8"
            return index == 0 ? ParameterUsage::PageID
                 : index == 1 ? ParameterUsage::StringID : ParameterUsage::None;

         // {Page,String} @ {1, 2}
         case CMD_READ_TEXT:                          // "$0 read text: page=$1 id=$2"
         case CMD_READ_TEXT_EXISTS:                   // "$0 read text: page id=$1, id=$2 exists"
         case CMD_SPEAK_TEXT:                         // "$0 speak text: page=$1 id=$2 priority=$3"
         case CMD_SPRINTF_BYREF:                      // "$0 sprintf: pageid=$1 textid=$2, $3, $4, $5, $6, $7"
         case CMD_DISPLAY_NEWS_ARTICLE:               // "$0 display news article: page=$1 newsid=$2 occurrences=$3 maxoffertime=$4 maxtime=$5 placeholder:race1=$6 race2=$7 customarray=$8"
         case CMD_GET_NEWS_ARTICLE_STATE:             // "$0 state of news article: page=$1 id=$2"
         case CMD_ADD_MENU_ITEM_BYREF:                // "add custom menu item to array $0: page=$1x id=$2y returnvalue=$3"
         case CMD_ADD_MENU_INFO_BYREF:                // "add custom menu info line to array $0: page=$1x id=$2y"
         case CMD_ADD_MENU_HEADING_BYREF:             // "add custom menu heading to array $0: page=$1x id=$2y"
            return index == 1 ? ParameterUsage::PageID
                 : index == 2 ? ParameterUsage::StringID : ParameterUsage::None;

         // {Script,Page,String} @ {0, 1, 2}
         case CMD_ADD_SCRIPT_OPTIONS:                 // "add script options: script=$0 pageid=$1 textid=$2"
         case CMD_REMOVE_PLAYER_GRAPH:                // "remove player graph: script=$0 group pageid=$1 group textid=$2"
            return index == 0 ? ParameterUsage::ScriptName
                 : index == 1 ? ParameterUsage::PageID
                 : index == 2 ? ParameterUsage::StringID : ParameterUsage::None;

         // {Page,String,Script} @ {1, 2, 3}
         case CMD_REGISTER_HOTKEY_1_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script $3"
         case CMD_REGISTER_HOTKEY_2_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script name $3"
            return index == 1 ? ParameterUsage::PageID 
                 : index == 2 ? ParameterUsage::StringID 
                 : index == 3 ? ParameterUsage::ScriptName : ParameterUsage::None;

         
         // {Script,Page,String} @ {1, 2, 3}
         case CMD_REMOVE_CLASS_GRAPH:                 // "remove class LogicExport graph: class=$0 script=$1 group pageid=$2 group textid=$3"
         case CMD_REMOVE_OBJECT_GRAPH:                // "$0 remove object graph: script=$1 group pageid=$2 group textid=$3"
            return index == 1 ? ParameterUsage::ScriptName
                 : index == 2 ? ParameterUsage::PageID
                 : index == 3 ? ParameterUsage::StringID : ParameterUsage::None;


         // {Page,String} @ {1+3, 2+4}
         case CMD_ADD_NEWS_ARTICLE:                   // "add encyclopedia custom article: News, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
         case CMD_ADD_INFO_ARTICLE:                   // "add encyclopedia custom article: Information, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
         case CMD_ADD_HISTORY_ARTICLE:                // "add encyclopedia custom article: History, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
         case CMD_ADD_STORIES_ARTICLE:                // "add encyclopedia custom article: Stories, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
            switch (index) {
            case 1: case 3:   return ParameterUsage::PageID;
            case 2: case 4:   return ParameterUsage::StringID;
            default:          return ParameterUsage::None; }

         // {Script,Page,String} @ {0, 1+3, 2+4}
         case CMD_ADD_PLAYER_GRAPH:                   // "add player graph: script=$0 pageid=$1 textid=$2 group pageid=$3 group textid=$4 subgroup=$5"
            switch (index) {
            case 0:           return ParameterUsage::ScriptName;
            case 1: case 3:   return ParameterUsage::PageID;
            case 2: case 4:   return ParameterUsage::StringID;
            default:          return ParameterUsage::None; }

         // {Script,Page,String} @ {2,3,4}
         case CMD_ADD_WING_SCRIPT:                    // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4"
            return index == 2 ? ParameterUsage::ScriptName
                 : index == 3 ? ParameterUsage::PageID 
                 : index == 4 ? ParameterUsage::StringID  : ParameterUsage::None;

         // {Script,Page,String} @ {2+5,3,4}
         case CMD_ADD_WING_SCRIPT_CHECK:              // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4 checkscript=$5"
            return index == 2 || index == 5 ? ParameterUsage::ScriptName
                 : index == 3               ? ParameterUsage::PageID 
                 : index == 4               ? ParameterUsage::StringID  : ParameterUsage::None;

         // {Script,Page,String} @ {1,2+4,3+5}
         case CMD_ADD_CLASS_GRAPH:                    // "add class LogicExport graph: class=$0 script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
         case CMD_ADD_OBJECT_GRAPH:                   // "$0 add object graph: script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
            switch (index) {
            case 1:           return ParameterUsage::ScriptName;
            case 2: case 4:   return ParameterUsage::PageID;
            case 3: case 5:   return ParameterUsage::StringID;
            default:          return ParameterUsage::None; }

         // {Page,String} @ {1,2+3}
         case CMD_READ_TEXT_ARRAY:                    // "$0 read text: page id=$1, from $2 to $3 to array, include empty=$4"
            return index == 1               ? ParameterUsage::PageID
                 : index == 2 || index == 3 ? ParameterUsage::StringID : ParameterUsage::None;

         // None:
         default:
            return ParameterUsage::None;
         }
      }

      /// <summary>Identifies command type from the ID</summary>
      /// <param name="id">Command ID</param>
      /// <param name="g">Command group</param>
      /// <returns>Standard/Auxiliary</returns>
      CommandType   LegacySyntaxFileReader::IdentifyType(UINT id, CommandGroup  g)
      {
         if (g == CommandGroup::MACRO)
            return CommandType::Macro;

         switch (id)
         {
         case CMD_COMMENT:
         case CMD_COMMAND_COMMENT:
         case CMD_NOP:
         case CMD_END:
         case CMD_ELSE:
         case CMD_BREAK:
         case CMD_CONTINUE:
            return CommandType::Auxiliary;

         default:
            return CommandType::Standard;
         }

      }
      
      /// <summary>Identifies variable argument commands</summary>
      /// <param name="id">Command ID</param>
      /// <returns></returns>
      VArgSyntax   LegacySyntaxFileReader::IsVariableArgument(UINT id)
      {
         switch (id)
         {
         case CMD_CALL_SCRIPT:               // "$1 $2 call script $0 :"
         case CMD_CALL_NAMED_SCRIPT:         // "$1 $0 call named script: script=$2, $3, $4, $5, $6, $7"
         case CMD_LAUNCH_SCRIPT_ARGS:        // "$0 launch named script: task=$1 scriptname=$2 prio=$3, $4, $5, $6, $7, $8"
         case CMD_BEGIN_TASK_ARGS:           // "$0 begin task $2 with script $1 and priority $3: arg1=$4o arg2=$5x arg3=$6y arg4=$7z arg5=$8a"
         case CMD_INTERRUPT_SCRIPT_ARGS:     // "$0 interrupt with script $1 and prio $2: arg1=$3 arg2=$4 arg3=$5 arg4=$6"
         case CMD_INTERRUPT_TASK_ARGS:       // "$0 interrupt task $2 with script $1 and prio $3: arg1=$4 arg2=$5 arg3=$6 arg4=$7"

         /*case CMD_START_COMMAND:
         case CMD_START_DELAYED_COMMAND:
         case CMD_START_WING_COMMAND:*/
            return VArgSyntax::NamedPairs;

         case CMD_SPRINTF:                   // "$0 sprintf: fmt=$1, $2o, $3x, $4y, $5z, $6a"
         case CMD_SPRINTF_BYREF:             // "$0 sprintf: pageid=$1x textid=$2y, $3o, $4x, $5y, $6z, $7a"
         case CMD_CREATE_NEW_ARRAY:          // "$0 create new array, arguments=$1o, $2x, $3y, $4z, $5a"
         case MACRO_DIM_ARRAY:               // "dim $0 = $1"
            return VArgSyntax::CommaDelimited;

         default:
            return VArgSyntax::None;
         }
      }

      /// <summary>Identifies variable argument commands</summary>
      /// <param name="id">Command ID</param>
      /// <returns></returns>
      UINT   LegacySyntaxFileReader::GetVariableArgumentCount(UINT id)
      {
         switch (id)
         {
         case CMD_BEGIN_TASK_ARGS:          // "$0 begin task $2 with script $1 and priority $3: arg1=$4o arg2=$5x arg3=$6y arg4=$7z arg5=$8a"
         case CMD_CALL_NAMED_SCRIPT:        // "$1 $0 call named script: script=$2, $3, $4, $5, $6, $7"
         case CMD_LAUNCH_SCRIPT_ARGS:       // "$0 launch named script: task=$1 scriptname=$2 prio=$3, $4, $5, $6, $7, $8"
            return 5;

         case CMD_INTERRUPT_SCRIPT_ARGS:    // "$0 interrupt with script $1 and prio $2: arg1=$3 arg2=$4 arg3=$5 arg4=$6"
         case CMD_INTERRUPT_TASK_ARGS:      // "$0 interrupt task $2 with script $1 and prio $3: arg1=$4 arg2=$5 arg3=$6 arg4=$7"
            return 4;

         case CMD_CALL_SCRIPT:              // "$1 $2 call script $0 :"
            return 10;

         case MACRO_DIM_ARRAY:               // "dim $0 :"
            return 50;

         /*case CMD_START_COMMAND:
         case CMD_START_DELAYED_COMMAND:
         case CMD_START_WING_COMMAND:
            return 4;*/

         case CMD_SPRINTF:                  // "$0 sprintf: fmt=$1, $2o, $3x, $4y, $5z, $6a"
         case CMD_SPRINTF_BYREF:            // "$0 sprintf: pageid=$1x textid=$2y, $3o, $4x, $5y, $6z, $7a"
         case CMD_CREATE_NEW_ARRAY:         // "$0 create new array, arguments=$1o, $2x, $3y, $4z, $5a"
            return 5;

         default:
            return 0;
         }
      }

      /// <summary>Identifies how missing varg parameters are handled</summary>
      /// <param name="id">Command ID</param>
      /// <returns></returns>
      VArgMethod  LegacySyntaxFileReader::GetVariableArgumentMethod(UINT id)
      {
         switch (id)
         {
         case CMD_BEGIN_TASK_ARGS:          // "$0 begin task $2 with script $1 and priority $3: arg1=$4o arg2=$5x arg3=$6y arg4=$7z arg5=$8a"
         case CMD_CALL_NAMED_SCRIPT:        // "$1 $0 call named script: script=$2, $3, $4, $5, $6, $7"
         case CMD_LAUNCH_SCRIPT_ARGS:       // "$0 launch named script: task=$1 scriptname=$2 prio=$3, $4, $5, $6, $7, $8"
         
         case CMD_SPRINTF:                  // "$0 sprintf: fmt=$1, $2o, $3x, $4y, $5z, $6a"
         case CMD_SPRINTF_BYREF:            // "$0 sprintf: pageid=$1x textid=$2y, $3o, $4x, $5y, $6z, $7a"
         case CMD_CREATE_NEW_ARRAY:         // "$0 create new array, arguments=$1o, $2x, $3y, $4z, $5a"

         /*case CMD_START_COMMAND:
         case CMD_START_DELAYED_COMMAND:
         case CMD_START_WING_COMMAND:*/

         case CMD_INTERRUPT_SCRIPT_ARGS:    // "$0 interrupt with script $1 and prio $2: arg1=$3 arg2=$4 arg3=$5 arg4=$6"
         case CMD_INTERRUPT_TASK_ARGS:      // "$0 interrupt task $2 with script $1 and prio $3: arg1=$4 arg2=$5 arg3=$6 arg4=$7"
            return VArgMethod::PadNull;

         case CMD_CALL_SCRIPT:              // "$1 $2 call script $0 :"
         case MACRO_DIM_ARRAY:              // "dim $0 :"
            return VArgMethod::Drop;

         default:
            return VArgMethod::None;
         }
      }
      
      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire syntax file</summary>
      /// <returns></returns>
      SyntaxFile   LegacySyntaxFileReader::ReadFile()
      {
         SyntaxFile   file;
         
         // Populate types (hard-coded)
         for (int i = 0; i < PARAMETER_SYNTAX_COUNT; ++i)
            if (StrCmpI(szParameterSyntax[i], L"--NONE--") != 0)
               file.Types[szParameterSyntax[i]] = (ParameterType)i;

            // Populate groups (hard-coded)
         for (int i = 0; i < COMMAND_GROUP_COUNT; ++i)
            file.Groups[szCommandGroups[i]] = (CommandGroup)i;

         // Read syntax blocks
         CommandSyntax::Declaration  d;
         while (ReadSyntax(file, d))
            file.Commands.Add( CommandSyntax(d) );

         return file;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Resolve a command group from the name</summary>
      /// <param name="name">The group name</param>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidValueException">Unknown group</exception>
      CommandGroup  LegacySyntaxFileReader::LookupCommandGroup(const SyntaxFile& f, const wstring& name)
      {
         // Lookup group
         auto pair = f.Groups.find(name);
         if (pair != f.Groups.end())
            return pair->second;

         // Unrecognised group
         throw InvalidValueException(HERE, VString(L"Unrecognised command group '%s' on line %d", name.c_str(), LineNumber));
      }

      /// <summary>Resolve a parameter type from the name</summary>
      /// <param name="name">The parameter type name</param>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidValueException">Unknown parameter type</exception>
      ParameterType  LegacySyntaxFileReader::LookupParameterType(const SyntaxFile& f, const wstring& name)
      {
         // Lookup type
         auto pair = f.Types.find(name);
         if (pair != f.Types.end())
            return pair->second;

         // Unrecognised group
         throw InvalidValueException(HERE, VString(L"Unrecognised parameter type '%s' on line %d", name.c_str(), LineNumber) );
      }

      /// <summary>Reads the next syntax block</summary>
      /// <param name="dec">Command declaration</param>
      /// <returns>true if successful, false on EOF</returns>
      /// <exception cref="Logic::FileFormatException">Missing syntax component</exception>
      /// <exception cref="Logic::InvalidValueException">Unknown command group / parameter type</exception>
      bool  LegacySyntaxFileReader::ReadSyntax(const SyntaxFile& f, CommandSyntax::Declaration& dec)
      {
         list<ParameterType> params;
         wstring line;

         // Init
         dec.clear();

         // Group
         if (!ReadLine(line))
            return false;  // EOF Reached
         dec.Group = LookupCommandGroup(f, line);

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

         // ID / Type
         RequireLine(line, L"ID");
         dec.ID = _wtoi(line.c_str());
         dec.Type = IdentifyType(dec.ID, dec.Group);

         // URL
         RequireLine(dec.URL, L"help URL");
         if (dec.URL == L"NONE")
            dec.URL.clear();

         // Syntax
         wstring oldSyntax;
         RequireLine(oldSyntax, L"syntax text");
         dec.Syntax = GenerateSyntax(oldSyntax);

         // Read Parameter types
         while (RequireLine(line, L"parameter") && line != END_BLOCK)
            params.push_back( LookupParameterType(f, line) );
         
         // Convert old parameter types into new format
         dec.Params = GenerateParams(dec.ID, oldSyntax, params);

         // Execution type
         dec.Execution = IdentifyExecution(dec.ID);

         // VarArgs
         dec.VArgument  = IsVariableArgument(dec.ID);
         dec.VArgCount  = GetVariableArgumentCount(dec.ID);
         dec.VArgParams = GetVariableArgumentMethod(dec.ID);
         return true;
      }


      /// <summary>Reads the next line</summary>
      /// <param name="line">The line text</param>
      /// <param name="help">Meaning of line</param>
      /// <returns>true</returns>
      /// <exception cref="Logic::FileFormatException">Line Missing</exception>
      bool  LegacySyntaxFileReader::RequireLine(wstring& line, const WCHAR* help)
      {
         // Ensure line exists
         if (!ReadLine(line))
            throw FileFormatException(HERE, VString(L"Missing %s on line %d", help, LineNumber));

         return true;
      }

      
      // Hard-coded Command group names, used only for resolving parameter syntax from the syntax definitions file
      const WCHAR*  LegacySyntaxFileReader::szCommandGroups[COMMAND_GROUP_COUNT] =  
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
      const WCHAR*  LegacySyntaxFileReader::szParameterSyntax[PARAMETER_SYNTAX_COUNT] = 
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
