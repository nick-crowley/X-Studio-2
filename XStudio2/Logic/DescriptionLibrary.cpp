#include "stdafx.h"
#include "DescriptionLibrary.h"
#include "DescriptionFileReader.h"
#include "FileStream.h"

namespace Logic
{
   namespace Language
   {
      /// <summary>Description library singleton</summary>
      DescriptionLibrary  DescriptionLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      DescriptionLibrary::DescriptionLibrary()
      {
      }


      DescriptionLibrary::~DescriptionLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Clears the library</summary>
      void  DescriptionLibrary::Clear()
      {
         Commands.clear();
         Constants.clear();
         Macros.clear();
      }


      /// <summary>Populates the library from the descriptions file</summary>
      /// <param name="data">Background worker data</param>
      /// <returns>Descriptions loaded</returns>
      /// <exception cref="Logic::FileFormatException">??????????</exception>
      /// <exception cref="Logic::InvalidValueException">??????????</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      UINT  DescriptionLibrary::Enumerate(WorkerData* data)
      {
         const Path path(L"D:\\My Projects\\XStudio2\\XStudio2\\Descriptions.xml");

         // Clear previous contents
         Clear();

         // Feedback
         data->SendFeedback(ProgressType::Info, 1, L"Loading description tooltips...");

         // Feedback
         data->SendFeedback(ProgressType::Info, 2, GuiString(L"Loading legacy syntax file '%s'", path.FileName.c_str()));
         Console << Cons::Heading << L"Reading legacy syntax file: " << path << ENDL;
         
         // Load/Add descriptions file
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         Add( DescriptionFileReader(fs).ReadFile() );

         // Return descriptions read
         Console << Colour::Green << L"Description file loaded successfully" << ENDL;
         return Commands.size() + Constants.size();
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Copies contents of a description file into the library</summary>
      /// <param name="f">file</param>
      void  DescriptionLibrary::Add(DescriptionFile& f)
      {
         // Copy macros
         for (auto& pair : f.Macros)
            Macros.Add(pair.second);

         // Copy commands
         for (auto& pair : f.Commands)
            Commands.Add(pair.second);

         // Copy constants
         for (auto& pair : f.Constants)
            Constants.Add(pair.second);
      }
   }
}
