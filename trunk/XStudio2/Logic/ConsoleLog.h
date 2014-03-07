#pragma once

#include "Common.h"
#include "LogFileWriter.h"

namespace Logic
{
   /// <summary></summary>
   class ConsoleLog
   {
      // ------------------------ TYPES --------------------------
   protected:
	  
      // --------------------- CONSTRUCTION ----------------------
   private:
      ConsoleLog();
      virtual ~ConsoleLog();
		 
		NO_COPY(ConsoleLog);	// Default copy semantics
		NO_MOVE(ConsoleLog);	// Default move semantics

      // ------------------------ STATIC -------------------------
   public:
      void  Close();
      void  Open();
      void  Write(const wstring& txt, WORD attr);

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   public:
      static ConsoleLog  Instance;

   protected:
      LogFileWriter*  Writer;
   };

   /// <summary>Access to logfile Singleton</summary>
   #define  LogFile   ConsoleLog::Instance

}

using namespace Logic;
