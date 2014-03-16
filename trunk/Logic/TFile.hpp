#pragma once
#include "TObject.h"

namespace Logic
{
   namespace Types
   {
      /// <summary>Non-generic TFile interface</summary>
      interface ITFile
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         virtual ~ITFile() {}

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(UINT,Count,GetCount);

         // ---------------------- ACCESSORS ------------------------			
      public:
         virtual UINT  GetCount() const PURE;

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Finds at object by index</summary>
         /// <typeparam name="OBJ">Object type</typeparam>
         /// <param name="index">The index</param>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">OBJ does not match object type</exception>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         template <typename OBJ>
         OBJ* FindAt(UINT index)
         {
            // Find object and upcast
            if (OBJ* obj = dynamic_cast<OBJ>(FindAt(index)))
               return obj;

            // Error: Wrong type
            throw ArgumentException(HERE, L"OBJ", L"Invalid object type");
         }

         /// <summary>Finds at object by index</summary>
         /// <param name="index">The index.</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         virtual const TObject*  FindAt(UINT index) const PURE;

         /// <summary>Removes an object by index</summary>
         /// <param name="index">The index.</param>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         virtual void  RemoveAt(UINT index) PURE;
      };

      /// <summary>Unique type definition file smart pointer</summary>
      typedef shared_ptr<ITFile>  TFilePtr;

      /// <summary>Object type definition file</summary>
      template <typename OBJ>
      class TFile : public ITFile
      {
         // ------------------------ TYPES --------------------------
      public:
         // --------------------- CONSTRUCTION ----------------------

      public:
         TFile(UINT count)
         {
            Objects.reserve(count);
         }
         virtual ~TFile()
         {}

         DEFAULT_COPY(TFile);	// Default copy semantics
         DEFAULT_MOVE(TFile);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets the object count.</summary>
         /// <returns></returns>
         UINT  GetCount() const
         {
            return Objects.size();
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Finds at object by index</summary>
         /// <param name="index">The index.</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         const TObject*  FindAt(UINT index) const
         {
            // Validate index
            if (index >= Objects.size())
               throw IndexOutOfRangeException(HERE, index, Count);

            return &Objects[index];
         }

         /// <summary>Removes an object by index</summary>
         /// <param name="index">The index.</param>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         void  RemoveAt(UINT index)
         {
            // Validate index
            if (index >= Objects.size())
               throw IndexOutOfRangeException(HERE, index, Count);

            Objects.erase(Objects.begin() + index);
         }
         // -------------------- REPRESENTATION ---------------------

      public:
         vector<OBJ>  Objects;
      };
   
   }
}

using namespace Logic::Types;
