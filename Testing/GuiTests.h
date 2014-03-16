#pragma once

namespace Testing
{
   /// <summary></summary>
   class GuiTests
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      GuiTests() {};
      virtual ~GuiTests() {};

      // ------------------------ STATIC -------------------------

   public:
      static void  RunAll();

   private:
      static void  Test_DiffDocument();

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------			

		// ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

   private:
   };
}

using namespace Logic;
