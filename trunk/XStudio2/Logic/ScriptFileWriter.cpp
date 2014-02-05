#include "stdafx.h"
#include "ScriptFileWriter.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a script file writer</summary>
      /// <param name="out">Output stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not writeable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      ScriptFileWriter::ScriptFileWriter(StreamPtr out) : XmlWriter(out)
      {
      }


      ScriptFileWriter::~ScriptFileWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Closes and flushes the output stream</summary>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  ScriptFileWriter::Close()
      {
         XmlWriter::Close();
      }

      /// <summary>Writes a script file to the output stream</summary>
      /// <param name="sf">script</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::Write(const ScriptFile& sf)
      {
         UINT index = 1;

         // Header
         WriteInstruction(L"version='1.0' encoding='utf-8'");
         //WriteInstruction(L"-stylesheet href='x2script.xsl' type='text/xsl'");
         WriteComment(L"Written by X-Studio II");
         auto root = WriteRoot(L"script");

         // Properties
         WriteElement(root, L"name", sf.Name);
         WriteElement(root, L"version", GuiString(L"%d", sf.Version));
         WriteElement(root, L"engineversion", L"44");
         WriteElement(root, L"description", sf.Description);

         // Arguments
         auto arguments = WriteElement(root, L"arguments");
         for (const ScriptVariable& var : sf.Variables.Arguments.SortByID)
            WriteArgument(arguments, index++, var);

         // TODO: SourceText

         // CodeArray
         auto codearray = WriteElement(root, L"codearray");
         codearray = WriteArray(codearray, 10);

         // Properties 
         WriteValue(codearray, sf.Name);
         WriteValue(codearray, 44);
         WriteValue(codearray, sf.Description);
         WriteValue(codearray, sf.Version);
         WriteValue(codearray, sf.LiveData ? 1 : 0);

         // Variables
         auto variables = WriteArray(codearray, sf.Variables.Count);
         for (const ScriptVariable& var : sf.Variables.All.SortByID)
            WriteValue(variables, var.Name);

         // Commands (std)
         auto commands = WriteArray(codearray, sf.Commands.StdOutput.size());
         for (const ScriptCommand& c : sf.Commands.StdOutput)
            WriteCommand(commands, c);

         // Arguments
         arguments = WriteArray(codearray, sf.Variables.Arguments.size());
         for (const ScriptVariable& var : sf.Variables.Arguments.SortByID)
            WriteArgument(arguments, var);

         // Commands (aux)
         commands = WriteArray(codearray, sf.Commands.AuxOutput.size());
         for (const ScriptCommand& c : sf.Commands.AuxOutput)
            WriteCommand(commands, c);

         // Command ID
         if (sf.HasCommandName())
            WriteValue(codearray, sf.CommandName);
         else
            WriteValue(codearray, 0);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Calculates number of elements needed to specify a command</summary>
      /// <returns></returns>
      UINT ScriptFileWriter::CalculateSize(const ScriptCommand& cmd)
      {
         // ID [+Index]
         UINT count = cmd.Is(CommandType::Standard) ? 2 : 1;

         // Parameters
         for (const ScriptParameter& p : cmd.Parameters)
            switch (p.Syntax.Type)
            {
            // {Type,Value} pair
            default: count += 2; 
               continue;

            // Single string node
            case ParameterType::COMMENT:        
            case ParameterType::SCRIPT_NAME:    
            case ParameterType::LABEL_NAME:     
            // Single int node
            case ParameterType::VARIABLE:
            case ParameterType::RETURN_VALUE:
            case ParameterType::RETURN_VALUE_IF:
            case ParameterType::RETURN_VALUE_IF_START:
            case ParameterType::INTERRUPT_RETURN_VALUE_IF: 
            case ParameterType::LABEL_NUMBER:  
            case ParameterType::STRUCTURAL_COUNT:
            case ParameterType::STRUCTURAL_INDEX:
               ++count;
               continue;
            }

         return count;
      }

      /// <summary>Writes an array node</summary>
      /// <param name="parent">parent node</param>
      /// <param name="size">array size</param>
      /// <returns></returns>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlElementPtr  ScriptFileWriter::WriteArray(XmlElementPtr parent, UINT size)
      {
         auto node = WriteElement(parent, L"sval");
         WriteAttribute(node, L"type", L"array");
         WriteAttribute(node, L"size", size);
         return node;
      }
      
      /// <summary>Writes a codearray argument node</summary>
      /// <param name="parent">parent (codearray node)</param>
      /// <param name="var">argument</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::WriteArgument(XmlElementPtr parent, const ScriptVariable& var)
      {
         auto arg = WriteArray(parent, 2);
         WriteValue(arg, (int)var.Type);
         WriteValue(arg, var.Description);
      }

      /// <summary>Writes an argument node</summary>
      /// <param name="parent">parent (root node)</param>
      /// <param name="index">1-based index</param>
      /// <param name="var">argument</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::WriteArgument(XmlElementPtr parent, UINT index, const ScriptVariable& var)
      {
         auto arg = WriteElement(parent, L"argument");
         WriteAttribute(arg, L"index", index);
         WriteAttribute(arg, L"name", var.Name);
         WriteAttribute(arg, L"type", GetString(var.ValueType));
         WriteAttribute(arg, L"desc", var.Description);
      }

      /// <summary>Writes a standard or auxiliary command</summary>
      /// <param name="parent">parent node (std or auxiliary command branch)</param>
      /// <param name="cmd">command</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::WriteCommand(XmlElementPtr parent, const ScriptCommand& cmd)
      {
         // array
         auto node = WriteArray(parent, CalculateSize(cmd));

         // Index+ID
         if (cmd.Is(CommandType::Auxiliary))
            WriteValue(node, cmd.RefIndex);
         WriteValue(node, cmd.Syntax.ID);

         // Parameters
         for (const ScriptParameter& p : cmd.Parameters)
            WriteParameter(node, p);
      }

      /// <summary>Writes a command parameter</summary>
      /// <param name="parent">parent node (command node)</param>
      /// <param name="p">parameter</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void ScriptFileWriter::WriteParameter(XmlElementPtr parent, const ScriptParameter& p)
      {
         // Check syntax
         switch (p.Syntax.Type)
         {
         // Single int node
         case ParameterType::VARIABLE:
         case ParameterType::RETURN_VALUE:
         case ParameterType::RETURN_VALUE_IF:
         case ParameterType::RETURN_VALUE_IF_START:
         case ParameterType::INTERRUPT_RETURN_VALUE_IF: 
         case ParameterType::LABEL_NUMBER:  
         case ParameterType::STRUCTURAL_COUNT:
         case ParameterType::STRUCTURAL_INDEX:
            WriteValue(parent, p.Value.Int);
            break;

         // Single string node
         case ParameterType::COMMENT:        
         case ParameterType::SCRIPT_NAME:    
         case ParameterType::LABEL_NAME:     
            WriteValue(parent, p.Value.String);
            break;

         // {Type,Value} pair
         default:
            WriteValue(parent, (int)p.Type);
            WriteValue(parent, p.Value);
            break;
         }
      }
      
      /// <summary>Writes an integer value</summary>
      /// <param name="parent">parent node</param>
      /// <param name="val">value.</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::WriteValue(XmlElementPtr parent, int val)
      {
         auto node = WriteElement(parent, L"sval");
         WriteAttribute(node, L"type", L"int");
         WriteAttribute(node, L"val", val);
      }

      /// <summary>Writes a string value</summary>
      /// <param name="parent">parent node</param>
      /// <param name="val">value.</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::WriteValue(XmlElementPtr parent, const wstring& val)
      {
         auto node = WriteElement(parent, L"sval");
         WriteAttribute(node, L"type", L"string");
         WriteAttribute(node, L"val", val);
      }
      
      /// <summary>Writes a parameter value</summary>
      /// <param name="parent">parent node</param>
      /// <param name="val">value.</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileWriter::WriteValue(XmlElementPtr parent, const ParameterValue& val)
      {
         if (val.Type == ValueType::String)
            WriteValue(parent, val.String);
         else
            WriteValue(parent, val.Int);
      }

   }
}

