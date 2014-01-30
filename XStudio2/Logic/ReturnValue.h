#pragma once

#include "Common.h"

namespace Logic
{
   namespace Scripts
   {
         

      //              ---- Return Value ----
      //     HI                                       LO
      //  ---------------------------------------------------
      // :             :                   :                 :
      // : RETURN TYPE : DESTINATION INDEX : CONDITIONAL ID  :
      // :             :                   :                 :
      //  -- 1 byte ---------- 2 bytes --------- 1 byte -----
      //

      /// <summary>Identifies how the ReturnObject is handled internally by the game.</summary>
      enum class ReturnType : BYTE
      {   
         ASSIGNMENT    = 0x00,      // Assign result to a variable
         DISCARD       = 0x80,      // Discard result
         JUMP_IF_TRUE  = 0xA0,      // Jump to <destination> if result is TRUE
         JUMP_IF_FALSE = 0xE0       // Jump to <destination> if result is FALSE
      };   
          
      /// <summary>Identifies the branching statements used in the script language. The first 8 IDs are actually encoded in 
      /// ReturnObjects whereas the last four are defined by auxiliary commands and used for my coding clarity.</summary>
      enum class Conditional : BYTE 
      {   // Conditional IDs used by the game
         NONE      = 0,   DISCARD     = 1,
         START     = 2,
         IF        = 3,   IF_NOT      = 4, 
         ELSE_IF   = 5,   ELSE_IF_NOT = 6, 
         SKIP_IF   = 7,   SKIP_IF_NOT = 8,
         WHILE     = 9,   WHILE_NOT   = 10,
         // My custom IDs to represent auxiliary branching commands, jumps, gotos and labels.
         BREAK     = 11,  CONTINUE    = 12,
         ELSE      = 13,  END         = 14,
         LABEL     = 15,  GOTO        = 16,
         GOSUB     = 17,  ENDSUB      = 18  
      };

      /// <summary>Represents the encoded return value of a script command</summary>
      class ReturnValue
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Decode a return value</summary>
         /// <param name="v">The v.</param>
         ReturnValue(int v) : Value(v)
         {
            ReturnType = (Scripts::ReturnType)(BYTE)((v & 0xff000000) >> 24);
            Destination = (v & 0x00ffff00) >> 8;
            Conditional = (Scripts::Conditional)(BYTE)(v & 0x000000ff);
         }

         /// <summary>Encode a return value from a conditional</summary>
         /// <param name="v">The v.</param>
         ReturnValue(Conditional c) : Conditional(c), Destination(0xffff)
         {
            switch (Conditional)
            {
            case Conditional::NONE:
               ReturnType = ReturnType::ASSIGNMENT;
               break;

            case Conditional::START:
            case Conditional::DISCARD:  
               ReturnType = ReturnType::DISCARD;         
               break;

            case Conditional::IF:
            case Conditional::WHILE:
            case Conditional::ELSE_IF:
            case Conditional::SKIP_IF_NOT:  
               ReturnType = ReturnType::JUMP_IF_FALSE;   
               break;
            case Conditional::IF_NOT:
            case Conditional::WHILE_NOT:
            case Conditional::ELSE_IF_NOT:
            case Conditional::SKIP_IF:      
               ReturnType = ReturnType::JUMP_IF_TRUE;    
               break;

            default:
               throw ArgumentException(HERE, L"c", GuiString(L"Invalid conditional: %d", c));
            }

            // Encode
            Value = (INT)((BYTE)ReturnType << 24) | (INT)((WORD)Destination << 8) | (INT)(BYTE)Conditional;
         }

         virtual ~ReturnValue()
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         ReturnType    ReturnType;
         Conditional   Conditional;
         int           Destination,
                       Value;          // Encoded value
      };

   }
}

using namespace Logic::Scripts;

