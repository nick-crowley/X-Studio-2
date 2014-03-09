
///////////////////////////////// ID //////////////////////////////////

#define IDR_MAINFRAME                   100
#define IDR_PROPERTIES                  101
#define IDR_OUTPUT                      102
#define IDR_COMMANDS                    103
#define IDR_GAME_OBJECTS                104
#define IDR_SCRIPT_OBJECTS              105
#define IDR_EDITOR                      106

#define IDR_LANGUAGEVIEW                107
#define IDR_SCRIPTVIEW                  108
#define IDR_PROJECTVIEW                 109
#define IDR_STRINGVIEW                  150

///////////////////////////////// TOOLBARS /////////////////////////////

#define IDT_FILE                        110
#define IDT_EDIT                        111
#define IDT_VIEW                        112
#define IDT_MENU_IMAGES                 113

////////////////////////////////// MENUS ///////////////////////////////

#define IDM_EDIT_POPUP                  114
#define IDM_PROJECT_POPUP               115
#define IDM_OUTPUT_POPUP                116
#define IDM_STRINGVIEW_POPUP            117

///////////////////////////////// BITMAPS //////////////////////////////

#define IDB_EDITOR_GREY                 118
#define IDB_PROJECT_ICONS               119
#define IDB_OUTPUT_ICONS                120
#define IDB_GAMEDATA_ICONS              121
#define IDB_LANGUAGE_ICONS              122
#define IDB_RICH_BUTTON                 123
#define IDB_USER_IMAGES                 124
#define IDB_NEWDOCUMENT_ICONS           125
#define IDB_SPLASH                      126

////////////////////////////////// ICONS ///////////////////////////////


///////////////////////////////// COMMANDS /////////////////////////////

// File Menu
#define ID_FILE_BROWSE                  200
#define ID_FILE_SAVE_ALL                201
#define ID_FILE_EXPORT                  202

// Edit Menu
#define ID_EDIT_INSERT                  203
#define ID_EDIT_COMMENT                 204
#define ID_EDIT_LOCALIZE                205
#define ID_EDIT_REFACTOR                206
#define ID_EDIT_INDENT                  207
#define ID_EDIT_OUTDENT                 208
#define ID_EDIT_PREFERENCES             209

// Test menu
#define ID_TEST_RUN_ALL                 210
#define ID_TEST_COMPILE                 211

// View menu
#define ID_VIEW_PROJECT                 212
#define ID_VIEW_SCRIPT_OBJECTS          213
#define ID_VIEW_GAME_OBJECTS            214
#define ID_VIEW_COMMANDS                215
#define ID_VIEW_OUTPUT                  216
#define ID_VIEW_PROPERTIES              217
#define ID_VIEW_STRING_LIBRARY          218
#define ID_VIEW_MEDIA_LIBRARY           219
#define ID_VIEW_CUSTOMIZE               220

// Window menu
#define ID_WINDOW_MANAGER               221

// Help
#define ID_HELP_FORUMS                  222
#define ID_VIEW_CONSOLE                 223

////////////////////////// CONTEXT MENU COMMANDS ////////////////////////

// PropertiesWnd Context
#define ID_EXPAND_ALL                   300
#define ID_SORTPROPERTIES               301
#define ID_INSERT_ARGUMENT              302
#define ID_REORDER_ARGUMENT_UP          303
#define ID_EDIT_ARGUMENT                304
#define ID_REORDER_ARGUMENT_DOWN        305
#define ID_REMOVE_ARGUMENT              306

// ProjectWnd Context menu
#define ID_PROJECT_OPEN                 307
#define ID_PROJECT_ADD_FILE             308
#define ID_PROJECT_ADD_FOLDER           309
#define ID_PROJECT_RENAME               310
#define ID_PROJECT_REMOVE               311
#define ID_PROJECT_DELETE               312
#define ID_PROJECT_PROPERTIES           313

///////////////////////////// TOOLBAR COMMANDS //////////////////////////

// LanguageEditor
#define ID_EDIT_BOLD                    400
#define ID_EDIT_ITALIC                  401
#define ID_EDIT_UNDERLINE               402
#define ID_EDIT_COLOUR                  403
#define ID_EDIT_LEFT                    404
#define ID_EDIT_CENTRE                  405
#define ID_EDIT_RIGHT                   406
#define ID_EDIT_JUSTIFY                 407
#define ID_EDIT_ADD_BUTTON              408
#define ID_VIEW_SOURCE                  409
#define ID_VIEW_EDITOR                  410
#define ID_VIEW_DISPLAY                 411

///////////////////////////////// DIALOGS //////////////////////////////

#define IDD_ABOUTBOX                    700
#define IDD_FINDREPLACE                 701
#define IDD_FINDPROGRESS                702
#define IDD_GENERAL_PAGE                703
#define IDD_COMPILER_PAGE               704
#define IDD_EDITOR_PAGE                 705
#define IDD_NEW_DOCUMENT                706
#define IDD_SPLASH                      707
#define IDD_REFACTOR                    708
#define IDD_SYMBOL                      709

///////////////////////////////// CONTROLS //////////////////////////////

// Script View
#define IDC_SCOPE_COMBO                 800
#define IDC_VARIABLES_COMBO             801
#define IDC_SCRIPT_EDIT                 802

// Language View
#define IDC_PAGE_LIST                   800
#define IDC_STRING_LIST                 801
#define IDC_STRING_EDIT                 802

// Find Dialog
#define IDC_FIND                        800
#define IDC_REPLACE                     801
#define IDC_FIND_ALL                    802
#define IDC_REPLACE_ALL                 803
#define IDC_FIND_COMBO                  804
#define IDC_REPLACE_COMBO               805
#define IDC_TARGET_COMBO                806
#define IDC_CASE_CHECK                  807
#define IDC_WORD_CHECK                  808
#define IDC_REGEX_CHECK                 809
#define IDC_OPTIONS                     810
#define IDC_RESULTS1_RADIO              811
#define IDC_RESULTS2_RADIO              812
#define IDC_FIND_GROUPBOX               813
#define IDC_FIND_RESIZE                 814

// Find Progress Dialog
#define IDC_PROGRESS                    800

// Project Dialog
#define IDC_PROJECT_TREE                800

// Preferences dialog
#define IDC_PROPERTY_GRID               800

// New document dialog
#define IDC_TEMPLATE_LIST               800
#define IDC_FILENAME_EDIT               801
#define IDC_FOLDER_EDIT                 802
#define IDC_DESCRIPTION_EDIT            803
#define IDC_ADD_PROJECT_CHECK           804

// Refactor dialog
#define IDC_SYMBOL_LIST                 800
//#define IDC_SYMBOL_EDIT                 801

// Symbol dialog
#define IDC_SYMBOL_EDIT                 802
#define IDC_RENAME_EDIT                 803
#define IDC_PREVIEW_CHECK               804

///////////////////////////////// STRINGS ///////////////////////////////

// Customization
#define IDS_EDIT_MENU                   1014

// Status bar
#define IDS_CARET_STATUS                1015
#define IDS_STATUS_PANE2                1016

// Toolbar
#define IDS_TOOLBAR_CUSTOMIZE           1018


// Command Group Names
#define IDS_COMMAND_GROUP_ARRAY			          1100
#define IDS_COMMAND_GROUP_CUSTOM                 1101
#define IDS_COMMAND_GROUP_FLEET						 1102
#define IDS_COMMAND_GROUP_FLOW_CONTROL				 1103
#define IDS_COMMAND_GROUP_GAME_ENGINE				 1104
#define IDS_COMMAND_GROUP_GRAPH						 1105 
#define IDS_COMMAND_GROUP_MACRO                  1106
#define IDS_COMMAND_GROUP_MARINE						 1107
#define IDS_COMMAND_GROUP_MATHS						 1108
#define IDS_COMMAND_GROUP_MERCHANT					 1109
#define IDS_COMMAND_GROUP_NON_PLAYER				 1110
#define IDS_COMMAND_GROUP_OBJECT_ACTION	 		 1111
#define IDS_COMMAND_GROUP_OBJECT_PROPERTY	 		 1112
#define IDS_COMMAND_GROUP_PASSENGER			 		 1113
#define IDS_COMMAND_GROUP_PILOT					 	 1114
#define IDS_COMMAND_GROUP_PLAYER						 1115
#define IDS_COMMAND_GROUP_SCRIPT_PROPERTY			 1116
#define IDS_COMMAND_GROUP_SHIP_ACTION				 1117
#define IDS_COMMAND_GROUP_SHIP_PROPERTY			 1118
#define IDS_COMMAND_GROUP_SHIP_TRADE				 1119
#define IDS_COMMAND_GROUP_SHIP_WING					 1120
#define IDS_COMMAND_GROUP_STATION_PROPERTY 		 1121
#define IDS_COMMAND_GROUP_STATION_TRADE			 1122
#define IDS_COMMAND_GROUP_STOCK_EXCHANGE		 	 1123
#define IDS_COMMAND_GROUP_STRING						 1124
#define IDS_COMMAND_GROUP_SYSTEM_PROPERTY			 1125
#define IDS_COMMAND_GROUP_UNIVERSE_DATA			 1126
#define IDS_COMMAND_GROUP_UNIVERSE_PROPERTY		 1127
#define IDS_COMMAND_GROUP_USER_INTERFACE			 1128
#define IDS_COMMAND_GROUP_WAR					       1129
#define IDS_COMMAND_GROUP_WARE_PROPERTY			 1130
#define IDS_COMMAND_GROUP_WEAPON_PROPERTY			 1131
#define IDS_COMMAND_GROUP_HIDDEN		             1132
#define IDS_FIRST_COMMAND_GROUP                  IDS_COMMAND_GROUP_ARRAY

// Maintype names
#define IDS_MAIN_TYPE_BULLET                     1140         
#define IDS_MAIN_TYPE_SECTOR                     1141         
#define IDS_MAIN_TYPE_BACKGROUND                 1142         
#define IDS_MAIN_TYPE_SUN                        1143         
#define IDS_MAIN_TYPE_PLANET                     1144         
#define IDS_MAIN_TYPE_DOCK                       1145         
#define IDS_MAIN_TYPE_FACTORY                    1146         // Factory
#define IDS_MAIN_TYPE_SHIP                       1147         // Ship
#define IDS_MAIN_TYPE_LASER                      1148         // Laser
#define IDS_MAIN_TYPE_SHIELD                     1149         // Shield
#define IDS_MAIN_TYPE_MISSILE                    1150         // Missile
#define IDS_MAIN_TYPE_ENERGY                     1151         // Ware
#define IDS_MAIN_TYPE_NATURAL                    1152         // Ware
#define IDS_MAIN_TYPE_BIO                        1153         // Ware
#define IDS_MAIN_TYPE_FOOD                       1154         // Ware
#define IDS_MAIN_TYPE_MINERAL                    1155         // Ware
#define IDS_MAIN_TYPE_TECH                       1156         // Ware
#define IDS_MAIN_TYPE_ASTEROID                   1157
#define IDS_MAIN_TYPE_GATE                       1158 
#define IDS_MAIN_TYPE_CAMERA                     1159 
#define IDS_MAIN_TYPE_SPECIAL                    1160 
#define IDS_MAIN_TYPE_NEBULA                     1161 
#define IDS_MAIN_TYPE_INSTATION                  1162 
#define IDS_MAIN_TYPE_DUMMY                      1163 
#define IDS_MAIN_TYPE_COMMAND                    1164 
#define IDS_MAIN_TYPE_COCKPIT                    1165 
#define IDS_MAIN_TYPE_DATATYPE                   1166 
#define IDS_MAIN_TYPE_UNKNOWN                    1167 
#define IDS_MAIN_TYPE_DEBRIS                     1168 
#define IDS_MAIN_TYPE_DOCK_WRECK                 1169 
#define IDS_MAIN_TYPE_FACTORY_WRECK              1170 
#define IDS_MAIN_TYPE_SHIP_WRECK                 1171 
#define IDS_FIRST_MAIN_TYPE                      IDS_MAIN_TYPE_BULLET


// ScriptObject Group names
#define IDS_SCRIPT_OBJECT_GROUP_CONSTANT         1180      // Script Constant
#define IDS_SCRIPT_OBJECT_GROUP_DATA_TYPE        1181      // Variable Data Type
#define IDS_SCRIPT_OBJECT_GROUP_FLIGHT_RETURN    1182      // Flight Return Code
#define IDS_SCRIPT_OBJECT_GROUP_OBJECT_CLASS     1183      // Object Class
#define IDS_SCRIPT_OBJECT_GROUP_RACE             1184      // Race
#define IDS_SCRIPT_OBJECT_GROUP_RELATION         1185      // Relation
#define IDS_SCRIPT_OBJECT_GROUP_PARAMETER_TYPE   1186      // Parameter Syntax
#define IDS_SCRIPT_OBJECT_GROUP_SECTOR           1187      // Sector
#define IDS_SCRIPT_OBJECT_GROUP_STATION_SERIAL   1188      // Station Serial
#define IDS_SCRIPT_OBJECT_GROUP_OBJECT_COMMAND   1189      // Object Command
#define IDS_SCRIPT_OBJECT_GROUP_WING_COMMAND     1190      // Wing Command
#define IDS_SCRIPT_OBJECT_GROUP_SIGNAL           1191      // Signal
#define IDS_SCRIPT_OBJECT_GROUP_TRANSPORT_CLASS  1192      // Transport Class
#define IDS_SCRIPT_OBJECT_GROUP_OPERATOR         1193      // Operator
#define IDS_FIRST_SCRIPT_OBJECT_GROUP            IDS_SCRIPT_OBJECT_GROUP_CONSTANT



// Language page group names
#define IDS_LANGUAGE_GROUP_DATA                  1200
#define IDS_LANGUAGE_GROUP_DIALOGUE              1201
#define IDS_LANGUAGE_GROUP_MISC                  1202
#define IDS_LANGUAGE_GROUP_QUEST                 1203
#define IDS_LANGUAGE_GROUP_BONUS                 1204
#define IDS_LANGUAGE_GROUP_MENU                  1205
#define IDS_LANGUAGE_GROUP_EDITOR                1206
#define IDS_LANGUAGE_GROUP_NEWS                  1207
#define IDS_LANGUAGE_GROUP_PLOT                  1208
#define IDS_LANGUAGE_GROUP_NPC                   1209
#define IDS_LANGUAGE_GROUP_USER                  1210
#define IDS_FIRST_LANGUAGE_GROUP                 IDS_LANGUAGE_GROUP_DATA


// Errors
#define ERR_XML_MISSING_ELEMENT         1000
#define ERR_XML_UNEXPECTED_ELEMENT      1001
#define ERR_XML_MISSING_ATTRIBUTE       1002
#define ERR_XML_PARSE_FAILED            1003

#define ERR_LANGUAGE_ID_INVALID         1004
#define ERR_ARGUMENT_NULL               1005
#define ERR_NO_READ_ACCESS              1006
#define ERR_NO_WRITE_ACCESS             1007
#define ERR_NO_SEEK_ACCESS              1008

