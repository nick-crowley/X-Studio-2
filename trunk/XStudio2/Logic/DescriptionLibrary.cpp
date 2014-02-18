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

      /// <summary>Private ctor</summary>
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
      /// <exception cref="Logic::ArgumentNullException">Missing xml node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid language ID -or- invalid command version</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      UINT  DescriptionLibrary::Enumerate(WorkerData* data)
      {
         const Path path(L"D:\\My Projects\\XStudio2\\XStudio2\\Descriptions.xml");

         // Clear previous contents
         Clear();

         // Feedback
         data->SendFeedback(Cons::Heading, ProgressType::Operation, 1, L"Loading commands and objects tooltips...");
         
         try
         {
            // Feedback
            Console << L"Reading descriptions file: " << Cons::Yellow << path << Cons::White << "...";

            // Load descriptions file
            StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
            DescriptionFile file( DescriptionFileReader(fs).ReadFile() );
            Add(file);

            // Feedback
            data->SendFeedback(ProgressType::Info, 2, GuiString(L"Loaded '%s'", file.Ident.c_str()));
            Console << Cons::Green << L"Success: " << Cons::White << file.Ident << ENDL;
         }
         catch (ExceptionBase& e) {
            Console << Cons::Red << L"Failed: " << e.Message << ENDL;
            throw;
         }

         // Return descriptions count
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
