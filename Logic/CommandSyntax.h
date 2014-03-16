#pragma once

#include "ParameterSyntax.h"

namespace Logic
{
   namespace Scripts
   {
      class CommandSyntax;

      /// <summary>Defines the display group of a script command</summary>
      enum class CommandGroup : UINT;

      /// <summary>Defines how script commands are stored</summary>
      enum class CommandType { Standard, Auxiliary, Macro };
      
      
      /// <summary>Constant command syntax reference</summary>
      typedef const CommandSyntax&  CommandSyntaxRef;

      /// <summary>Command syntax array</summary>
      typedef vector<const CommandSyntax*>  CmdSyntaxArray;

      /// <summary>Available execution types</summary>
      enum class ExecutionType  { Serial, Either, Concurrent };

      /// <summary>Parameter syntax lamda predicates</summary>
      typedef function<bool (const ParameterSyntax&)>  ParamSyntaxPredicate;

      /// <summary>Defines the syntax of a script command</summary>
      class LogicExport CommandSyntax
      {
      public:
         /// <summary>Declares command syntax within an external file</summary>
         class Declaration
         {
         public:
            Declaration() { clear(); }

            /// <summary>Clears contents</summary>
            void clear() { URL.clear(); Syntax.clear(); Params.clear(); Versions = ID = 0; }

            CommandGroup     Group;
            CommandType      Type;
            UINT             Versions, 
                             ID,
                             VarArgCount;
            wstring          URL,
                             Syntax;
            ParamSyntaxArray Params;
            ExecutionType    Execution;
            bool             VarArg;
         };

         // --------------------- CONSTRUCTION ----------------------
      private:
         CommandSyntax();
      public:
         CommandSyntax(const Declaration& d);
         virtual ~CommandSyntax();

         // ------------------------ STATIC -------------------------
      public:
         static const CommandSyntax  Unrecognised;

         static const ParamSyntaxPredicate  IsScriptNameParam;
         static const ParamSyntaxPredicate  IsStringRefParam;

      private:
         wstring  GenerateHash(const wstring& syntax);

         // --------------------- PROPERTIES ------------------------
		
      public:
         PROPERTY_GET(bool,Keyword,IsKeyword);
         PROPERTY_GET(wstring,DisplayText,GetDisplayText);
         PROPERTY_GET(ParamSyntaxArray,ParametersByDisplay,GetParametersByDisplay);

         bool  operator==(CommandSyntaxRef r) const   { return ID == r.ID && Versions == r.Versions; }
         bool  operator!=(CommandSyntaxRef r) const   { return ID != r.ID || Versions != r.Versions; }

		   // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Get populated display text</summary>
         wstring GetDisplayText() const;

         /// <summary>Get populated display richtext</summary>
         wstring GetRichText(bool boldParams) const;

         /// <summary>Get parameter syntax in display order</summary>
         ParamSyntaxArray  GetParametersByDisplay() const;

         /// <summary>Query command ID</summary>
         bool  Is(UINT ID) const;

         /// <summary>Query command type</summary>
         bool  Is(CommandType t) const;

         /// <summary>Query whether command is compatible with a version</summary>
         bool  IsCompatible(GameVersion v) const;

         /// <summary>Query whether command is a keyword like break, continue etc.</summary>
         bool  IsKeyword() const;

         /// <summary>Query whether command is a script call</summary>
         bool  IsScriptCall() const;

         /// <summary>Query whether command references a string/page ID</summary>
         bool  IsStringReference() const;

         /// <summary>Query whether command has variable arguments</summary>
         bool  IsVariableArgument() const;

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         const CommandGroup      Group;
         const CommandType       Type;
         const ParamSyntaxArray  Parameters;
         const ExecutionType     Execution;
         const UINT              Versions,
                                 ID,
                                 VarArgCount;
         const wstring           Hash,
                                 Text,
                                 URL;
         const bool              VarArg;
      };

      /// <summary>Defines the display group of a script command</summary>
      enum class CommandGroup : UINT
      {  
         ARRAY             = 0,
         CUSTOM            = 1,
         FLEET             = 2,
         FLOW_CONTROL      = 3,
         GAME_ENGINE       = 4,
         GRAPH             = 5,
         MACRO             = 6,
         MARINE            = 7,
         MATHS             = 8,
         MERCHANT          = 9,
         NON_PLAYER        = 10,
         OBJECT_ACTION     = 11,
         OBJECT_PROPERTY   = 12,
         PASSENGER         = 13,
         PILOT             = 14,
         PLAYER            = 15,
         SCRIPT_PROPERTY   = 16,
         SHIP_ACTION       = 17,
         SHIP_PROPERTY     = 18,
         SHIP_TRADE        = 19,
         SHIP_WING         = 20,
         STATION_PROPERTY  = 21,
         STATION_TRADE     = 22,
         STOCK_EXCHANGE    = 23,
         STRING            = 24,
         SYSTEM_PROPERTY   = 25,
         UNIVERSE_DATA     = 26,
         UNIVERSE_PROPERTY = 27,
         USER_INTERFACE    = 28,
         GC_WAR               = 29,
         WARE_PROPERTY     = 30,
         WEAPON_PROPERTY   = 31,

         HIDDEN            = 32 
      };

      /// <summary>Get command group name</summary>
      GuiString  GetString(CommandGroup g);

      /// <summary>Convenience command group global functions</summary>
      CommandGroup operator++(CommandGroup& g, int);
      CommandGroup& operator++(CommandGroup& g);
      UINT operator-(const CommandGroup& a, const CommandGroup& b);




      /// <summary>Define IDs of some important commands, for clarity</summary>
      #define    CMD_UNRECOGNISED                 0
      #define    CMD_COMMENT                      1
      #define    CMD_NOP                          2
      #define    CMD_COMMAND_COMMENT              3
      #define    CMD_END                          4
      #define    CMD_ELSE                         5
      #define    CMD_BREAK                        7
      #define    CMD_CONTINUE                     6
      #define    CMD_GOTO_LABEL                   100
      #define    CMD_DEFINE_LABEL                 101
      #define    CMD_CALL_SCRIPT                  102
      #define    CMD_RETURN                       103
      #define    CMD_EXPRESSION                   104
      #define    CMD_RANDOM_VALUE_ZERO            106
      #define    CMD_RANDOM_VALUE                 110
      #define    CMD_INTERRUPT_SCRIPT             111
      #define    CMD_HIDDEN_JUMP                  112
      #define    CMD_CONNECT_SHIP_SCRIPT          113 
      #define    CMD_SET_GLOBAL_SCRIPT_MAP        118
      #define    CMD_IS_DATATYPE                  125
      #define    CMD_READ_TEXT                    126
      #define    CMD_ARRAY_ALLOC                  128
      #define    CMD_ARRAY_ACCESS                 129
      #define    CMD_ARRAY_ASSIGNMENT             130
      #define    CMD_SIZE_OF_ARRAY                131
      #define    CMD_GET_NEWS_ARTICLE_STATE       138
      #define    CMD_SET_NEWS_ARTICLE_STATE       139
      #define    CMD_INCREMENT                    142
      #define    CMD_DECREMENT                    143
      #define    CMD_STRING_PRINTF                150
      #define    CMD_SET_SHIP_UPGRADE_SCRIPT      154
      #define    CMD_SET_LOCAL_VARIABLE           155
      #define    CMD_GET_LOCAL_VARIABLE           156
      #define    CMD_SET_GLOBAL_VARIABLE          157
      #define    CMD_GET_GLOBAL_VARIABLE          158
      #define    CMD_INTERRUPT_SCRIPT_ARGS        159
      #define    CMD_SEND_INCOMING_QUESTION       212
      #define    CMD_SEND_INCOMING_QUESTION_EX    213
      #define    CMD_DEFINE_ARRAY                 250
      #define    CMD_FOR_EACH                     251
      #define    CMD_FOR_EACH_COUNTER             252
      #define    CMD_FOR_LOOP                     253
      #define    CMD_ADD_MENU_ITEM_BYREF          254
      #define    CMD_ADD_MENU_INFO_BYREF          255
      #define    CMD_ADD_MENU_HEADING_BYREF       256
      #define    CMD_WRITE_LOGBOOK                403
      #define    CMD_WRITE_LOG                    406
      #define    CMD_START_COMMAND                514
      #define    CMD_AVERAGE_PRICE_BY_REF         628
      #define    CMD_AVERAGE_PRICE                629
      #define    CMD_REGISTER_AL_SCRIPT           1000
      #define    CMD_UNREGISTER_AL_SCRIPT         1001
      #define    CMD_INTERRUPT_TASK_ARGS          1016
      #define    CMD_BEGIN_TASK_ARGS              1017
      #define    CMD_SPEAK_TEXT                   1042
      #define    CMD_IS_SCRIPT_ON_STACK           1072
      #define    CMD_GET_INDEX_OF_ARRAY           1083
      #define    CMD_REGISTER_QUEST_SCRIPT        1084
      #define    CMD_ADD_EVENT_LISTENER           1088
      #define    CMD_DISPLAY_NEWS_ARTICLE         1090
      #define    CMD_REGISTER_HOTKEY_1            1100
      #define    CMD_REGISTER_GOD_EVENT           1141
      #define    CMD_GOTO_SUB                     1167
      #define    CMD_END_SUB                      1168
      #define    CMD_START_DELAYED_COMMAND        1185
      #define    CMD_CONNECT_WING_SCRIPT          1186
      #define    CMD_READ_TEXT_ARRAY              1337
      #define    CMD_READ_TEXT_EXISTS             1338
      #define    CMD_ADD_SECONDARY_SIGNAL         1339
      #define    CMD_SET_WING_UPGRADE_SCRIPT      1381
      #define    CMD_SET_GLOBAL_WING_SCRIPT_MAP   1382
      #define    CMD_SET_WING_COMMAND             1384
      #define    CMD_ADD_WING_SCRIPT              1393
      #define    CMD_ADD_WING_SCRIPT_CHECK        1398
      #define    CMD_START_WING_COMMAND           1404
      #define    CMD_SET_GLOBAL_SECONDARY_MAP     1407
      #define    CMD_GET_LOCAL_VARIABLE_KEYS      1414
      #define    CMD_CALL_NAMED_SCRIPT            1425
      #define    CMD_LAUNCH_SCRIPT_ARGS           1426
      #define    CMD_GET_GLOBAL_VARIABLE_KEYS     1436
      #define    CMD_ADD_MENU_INFO                1446
      #define    CMD_ADD_MENU_ITEM                1447
      #define    CMD_ADD_MENU_HEADING             1448
      #define    CMD_GET_LOCAL_VARIABLE_REGEX     1450
      #define    CMD_GET_GLOBAL_VARIABLE_REGEX    1451
      #define    CMD_SET_SHIP_PRELOAD_SCRIPT      1466
      #define    CMD_SET_WING_PRELOAD_SCRIPT      1467
      #define    CMD_GET_OBJECT_NAME_ARRAY        1496
      #define    CMD_SPEAK_ARRAY                  1510
      #define    CMD_REGISTER_HOTKEY_2            1511
      #define    CMD_ADD_NEWS_ARTICLE             1531
      #define    CMD_ADD_INFO_ARTICLE             1532
      #define    CMD_ADD_HISTORY_ARTICLE          1533
      #define    CMD_ADD_STORIES_ARTICLE          1534
      #define    CMD_ADD_SCRIPT_OPTIONS           1598
      #define    CMD_REMOVE_SCRIPT_OPTIONS        1599
      #define    CMD_CHECK_SCRIPT_EXISTS_1        1600
      #define    CMD_REGISTER_HOTKEY_1_BYREF      1601
      #define    CMD_GET_HOTKEY_INDEX_1           1602
      #define    CMD_REGISTER_HOTKEY_2_BYREF      1603
      #define    CMD_GET_HOTKEY_INDEX_2           1604
      #define    CMD_CHECK_SCRIPT_EXISTS_2        1605
      #define    CMD_ADD_PLAYER_GRAPH             1608
      #define    CMD_REMOVE_PLAYER_GRAPH          1609
      #define    CMD_ADD_CLASS_GRAPH              1610
      #define    CMD_REMOVE_CLASS_GRAPH           1611
      #define    CMD_ADD_OBJECT_GRAPH             1612
      #define    CMD_REMOVE_OBJECT_GRAPH          1613

   }
}

using namespace Logic::Scripts;
