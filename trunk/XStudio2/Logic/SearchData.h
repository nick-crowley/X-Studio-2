#pragma once

#include "Common.h"

namespace Logic
{
   
      

      /// <summary></summary>
      class SearchData
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         SearchData();
         SearchData(const wstring& txt);
         virtual ~SearchData();

         DEFAULT_COPY(SearchData);	// Default copy semantics
         DEFAULT_MOVE(SearchData);	// Default move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const CHARRANGE  NO_MATCH;

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(wstring,ReplaceTerm,GetReplaceTerm);
         PROPERTY_GET(wstring,SearchTerm,GetSearchTerm);
         PROPERTY_GET_SET(Path,FullPath,GetFullPath,SetFullPath);
         PROPERTY_GET_SET(CHARRANGE,Location,GetLocation,SetLocation);
         PROPERTY_GET_SET(bool,Matched,GetMatched,SetMatched);

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool  GetMatched() const
         {
            return Successful;
         }

         Path  GetFullPath() const
         {
            return FullPath;
         }

         CHARRANGE  GetLocation() const
         {
            return Location;
         }

         wstring    GetReplaceTerm() const
         {
            return Replace;
         }

         wstring    GetSearchTerm() const
         {
            return Search;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         void  Reset()
         {
            FullPath = L"";
            Successful = false;
            Location = NO_MATCH;
         }

         void  SetFullPath(Path p) 
         {
            FullPath = p;
         }

         void  SetLocation(CHARRANGE cr)
         {
            Location = cr;
         }

         void  SetMatched(bool b) 
         {
            Successful = b;
         }

         void  SetLastMatch(int pos = -1);

         // -------------------- REPRESENTATION ---------------------
      private:
         bool        Successful;
         CHARRANGE   Location;
         wstring     Search,
                     Replace;
         IO::Path    FullPath;
      };

   
}

using namespace Logic;