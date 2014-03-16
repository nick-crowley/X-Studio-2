#include "stdafx.h"
#include "ScriptFileReader.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates source value read from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      SourceValueReader::SourceValueReader(StreamPtr in) : XmlReader(in)
      {}

      /// <summary>empty</summary>
      SourceValueReader::~SourceValueReader()
      {}

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Reads the array size from an sval child node</summary>
      /// <param name="parent">Parent node</param>
      /// <param name="index">Zero-based child node index</param>
      /// <param name="help">Meaning of array</param>
      /// <returns>Size of the array</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not an array sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      int  SourceValueReader::ReadArray(XmlNodePtr& parent, UINT index, const WCHAR* help)
      {
         int size;

         // Verify child
         auto node = GetChild(parent, index, help);

         // Ensure node is script value
         ReadElement(node, L"sval");

         // Array: read size
         if (ReadAttribute(node, L"type") == L"array")
            size = _wtoi(ReadAttribute(node, L"size").c_str());

         // Int: Permit iff zero
         else if (ReadAttribute(node, L"type") == L"int" && ReadAttribute(node, L"val") == L"0")
            size = 0;
         
         else // Not an array
            throw FileFormatException(HERE, VString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));

         // Check size
         if (size != node->childNodes->length)
            throw FileFormatException(HERE, VString(L"Array <sval> node has %d children instead of %d", node->childNodes->length, size));

         return size;
      }


      /// <summary>Reads the integer value from an sval child node</summary>
      /// <param name="parent">Parent node</param>
      /// <param name="index">Zero-based child node index</param>
      /// <param name="help">Meaning of value</param>
      /// <returns>Integer value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not an integer sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      int  SourceValueReader::ReadInt(XmlNodePtr& parent, UINT index, const WCHAR* help)
      {
         // Get/Verify child
         auto node = GetChild(parent, index, help);

         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is int
         if (ReadAttribute(node, L"type") != L"int")
            throw FileFormatException(HERE, VString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));

         // Read value
         return _wtoi(ReadAttribute(node, L"val").c_str());
      }
      
      /// <summary>Reads the string value from an sval child node</summary>
      /// <param name="parent">Parent node</param>
      /// <param name="index">Zero-based child node index</param>
      /// <param name="help">Meaning of value</param>
      /// <returns>String value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not a string sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      wstring  SourceValueReader::ReadString(XmlNodePtr& parent, UINT index, const WCHAR* help)
      {
         // Get/Verify child
         auto node = GetChild(parent, index, help);

         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is string
         if (ReadAttribute(node, L"type") != L"string")
            throw FileFormatException(HERE, VString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));

         // Read value
         return ReadAttribute(node, L"val");
      }

      /// <summary>Reads string or integer value from an sval child node</summary>
      /// <param name="parent">Parent node</param>
      /// <param name="index">Zero-based child node index</param>
      /// <param name="help">Meaning of value</param>
      /// <returns>String/Int value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not a string/integer sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      ParameterValue  SourceValueReader::ReadValue(XmlNodePtr& parent, UINT index, const WCHAR* help)
      {
         // Get/Verify child
         auto node = GetChild(parent, index, help);

         // Ensure node is script value
         ReadElement(node, L"sval");
         wstring type = ReadAttribute(node, L"type");

         // String: return as string
         if (type == L"string")
            return ParameterValue( ReadAttribute(node, L"val") );

         // Int: return as int
         else if (type == L"int")
            return ParameterValue( _wtoi(ReadAttribute(node, L"val").c_str()) );

         // Unknown type
         throw FileFormatException(HERE, VString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));
      }


		// ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Reads the source file and locates the CodeArray node</summary>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">File format is corrupt</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  SourceValueReader::LoadDocument() 
      {
         // Parse document
         XmlReader::LoadDocument();

         // Get codearray node
         CodeArray = Document->documentElement->selectSingleNode(L"codearray");
         if (CodeArray == nullptr)
            throw FileFormatException(HERE, L"Missing codearray node");

         // Verify size
         else if (ReadArray(CodeArray, 0, L"codearray branch") != 10)
            throw FileFormatException(HERE, L"Invalid codearray node");
            
         // Set codearray data array
         CodeArray = GetChild(CodeArray, 0, L"codearray Branch");
      }

		// ------------------------------- PRIVATE METHODS ------------------------------
      

   }
}