#pragma once



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
         JUMP_IF_TRUE  = 0xE0,      // Jump to <destination> if result is TRUE
         JUMP_IF_FALSE = 0xA0       // Jump to <destination> if result is FALSE
      };

      /// <summary>Get return type string</summary>
      LogicExport GuiString  GetString(ReturnType t);
          
      /// <summary>Identifies the branching statements used in the script language. The first 8 IDs are actually encoded in 
      /// ReturnObjects whereas the last four are defined by auxiliary commands and used for my coding clarity.</summary>
      enum class Conditional : BYTE 
      {   // Conditional IDs used by the game
         NONE      = 0,   DISCARD     = 1,
         START     = 2,
         IF        = 3,   IF_NOT      = 4, 
         ELSE_IF   = 5,   ELSE_IF_NOT = 6, 
         SKIP_IF   = 7,   SKIP_IF_NOT = 8,
         WHILE     = 9,   WHILE_NOT   = 10
      };

      /// <summary>Get conditional string</summary>
      LogicExport GuiString  GetString(Conditional c);

      /// <summary>Represents the encoded return value of a script command</summary>
      class LogicExport ReturnValue
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Decode a return value</summary>
         /// <param name="v">encoded value</param>
         ReturnValue(int v) 
         {
            ReturnType = (Scripts::ReturnType)(BYTE)((v & 0xff000000) >> 24);
            Destination = (v & 0x00ffff00) >> 8;
            Conditional = (Scripts::Conditional)(BYTE)(v & 0x000000ff);
         }

         /// <summary>Encode a conditional retVar</summary>
         /// <param name="c">conditional</param>
         /// <param name="jump">jump destination</param>
         ReturnValue(Conditional c, UINT jump) : Conditional(c), Destination(jump)
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
               throw ArgumentException(HERE, L"c", VString(L"Invalid conditional: %d", c));
            }
         }

         /// <summary>Encode a return variable</summary>
         /// <param name="r">return type</param>
         /// <param name="var">variable ID</param>
         ReturnValue(ReturnType r, UINT var) : ReturnType(r), Conditional((Scripts::Conditional)var), Destination(0)
         {
         }

         virtual ~ReturnValue()
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(int,EncodedValue,GetEncodedValue);

		   // ---------------------- ACCESSORS ------------------------			

         /// <summary>Gets the encoded value.</summary>
         /// <returns></returns>
         int  GetEncodedValue() const
         {
            return (INT)((BYTE)ReturnType << 24) | (INT)((WORD)Destination << 8) | (INT)(BYTE)Conditional;
         }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         ReturnType   ReturnType;
         Conditional  Conditional;
         int          Destination;
      };

      /// <summary>Write return value to the console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ReturnValue& val);

   }
}

using namespace Logic::Scripts;

