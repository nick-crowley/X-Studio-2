#pragma once
#include "Logic/Common.h"

namespace GUI
{
   namespace Utils
   {
      class GuiCommand;

      /// <summary>Shared pointer to a GUI command</summary>
      typedef shared_ptr<GuiCommand>  GuiCommandPtr;

      /// <summary>Action/Command pattern for GUI commands</summary>
      class GuiCommand
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         GuiCommand() {}
         virtual ~GuiCommand() {}

         DEFAULT_COPY(GuiCommand);	// Default copy semantics
         DEFAULT_MOVE(GuiCommand);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(wstring,Name,GetName);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Query whether command can be undone</summary>
         virtual bool  CanUndo() const
         {
            return true;
         }

         /// <summary>Get command name</summary>
         virtual wstring GetName() const PURE;

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Execute the command</summary>
         virtual void Execute() PURE;

         /// <summary>Undo the command</summary>
         virtual void Undo() {}

         // -------------------- REPRESENTATION ---------------------
      private:
      };

      /// <summary>Queue of GUI commands</summary>
      class CommandQueue 
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         CommandQueue()
         {}

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(wstring,UndoName,GetUndoName);
         PROPERTY_GET(wstring,RedoName,GetRedoName);

         // ---------------------- ACCESSORS ------------------------		
      public:
         /// <summary>Query whether redo is available.</summary>
         /// <returns></returns>
         bool CanRedo() const
         {
            return !RedoQueue.empty();
         }

         /// <summary>Query whether undo is available.</summary>
         /// <returns></returns>
         bool CanUndo() const
         {
            return !UndoQueue.empty();
         }
         
         /// <summary>Get 'Redo' command name</summary>
         /// <exception cref="Logic::InvalidOperationException">Redo queue is empty</exception>
         wstring GetRedoName() const
         {
            if (RedoQueue.empty())
               throw InvalidOperationException(HERE, L"Redo queue is empty");

            return RedoQueue.front()->Name;
         }
         
         /// <summary>Gets the 'redo' menu item text</summary>
         /// <returns></returns>
         wstring  GetRedoMenuItem() const
         {
            if (!CanRedo())
               return L"Redo";

            return GuiString(L"Redo '%s'", GetRedoName().c_str());
         }

         /// <summary>Get 'Undo' command name</summary>
         /// <exception cref="Logic::InvalidOperationException">Undo queue is empty</exception>
         wstring GetUndoName() const
         {
            if (UndoQueue.empty())
               throw InvalidOperationException(HERE, L"Undo queue is empty");

            return UndoQueue.front()->Name;
         }
         
         /// <summary>Gets the 'Undo' menu item text</summary>
         /// <returns></returns>
         wstring  GetUndoMenuItem() const
         {
            if (!CanUndo())
               return L"Undo";

            return GuiString(L"Undo '%s'", GetUndoName().c_str());
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Clears the queue of all commands.</summary>
         void ClearUndo()
         {
            UndoQueue.clear();
            RedoQueue.clear();
         }

         /// <summary>Executes a command, adds it to the queue, and clears the 'Redo' queue</summary>
         /// <param name="cmd">The command.</param>
         /// <remarks>If the command throws, the queue is not affected</remarks>
         /// <exception cref="Logic::ArgumentNullException">Command is nullptr</exception>
         void Execute(GuiCommand* cmd)
         {
            Execute(GuiCommandPtr(cmd));
         }

         /// <summary>Executes a command, adds it to the queue, and clears the 'Redo' queue</summary>
         /// <param name="cmd">The command.</param>
         /// <remarks>If the command throws, the queue is not affected</remarks>
         /// <exception cref="Logic::ArgumentNullException">Command is nullptr</exception>
         void Execute(GuiCommandPtr cmd)
         {
            REQUIRED(cmd);

            // Execute
            cmd->Execute();

            // Is-Undoable: Add to 'Undo' queue
            if (cmd->CanUndo())
            {
               UndoQueue.push_front(cmd);
               
               // Clear 'Redo' queue
               RedoQueue.clear();
            }
         }

         /// <summary>Redo the last undone command.</summary>
         /// <remarks>If the command throws, the queue is not affected</remarks>
         /// <exception cref="Logic::InvalidOperationException">No commands to redo</exception>
         void Redo()
         {
            // Validate
            if (!CanRedo())
               throw InvalidOperationException(HERE, L"Redo queue is empty");

            // Re-Execute
            RedoQueue.front()->Execute();

            // Return command to the 'Undo' queue
            UndoQueue.push_front(RedoQueue.front());
            RedoQueue.pop_front();
         }

         /// <summary>Undo the last command.</summary>
         /// <remarks>If the command throws, the queue is not affected</remarks>
         /// <exception cref="Logic::InvalidOperationException">No commands to undo</exception>
         void Undo()
         {
            // Validate
            if (!CanUndo())
               throw InvalidOperationException(HERE, L"Undo queue is empty");

            // Undo
            UndoQueue.front()->Undo();

            // Move command into the 'Redo' queue
            RedoQueue.push_front(UndoQueue.front());
            UndoQueue.pop_front();
         }

         // -------------------- REPRESENTATION ---------------------
      private:
         list<GuiCommandPtr>  RedoQueue,
                              UndoQueue;
      };
   }

}

using namespace GUI::Utils;
