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
         ReturnValue(int v)
         {
            ReturnType = (Scripts::ReturnType)(BYTE)((v & 0xff000000) >> 24);
            Destination = (v & 0x00ffff00) >> 8;
            Conditional = (Scripts::Conditional)(BYTE)(v & 0x000000ff);
         }
         virtual ~ReturnValue()
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         ReturnType    ReturnType;
         Conditional   Conditional;
         int           Destination;

      private:
      };

   }
}

using namespace Logic::Scripts;

