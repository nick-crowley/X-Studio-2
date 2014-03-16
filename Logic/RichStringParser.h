#pragma once


#include "RichString.h"
#include <regex>

namespace Logic
{
   namespace Language
   {
      // ------------------------ TYPES --------------------------

      /// <summary>Defines available rich edit tags</summary>
      enum class TagType { Bold, Underline, Italic, 
                           Left, Right, Centre, Justify, 
                           Text, Select, Author, Title, Rank,
                           Black, Blue, Cyan, Green, Grey, Magenta, Orange, Red, Silver, Yellow, White, 
                           Default, Unrecognised };

      wstring   GetString(TagType t);

      // ------------------------ CLASSES ------------------------

      /// <summary>Rich-text parser</summary>
      class LogicExport RichStringParser
      {
         // ------------------------ TYPES --------------------------
      protected:
         /// <summary>Constant character iterator</summary>
         typedef wstring::const_iterator CharIterator;

         /// <summary>Rich-text tag {name,value} property pair</summary>
         class Property 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            Property(const wstring& name, const wstring& val) : Name(name), Value(val)
            {}

            // -------------------- REPRESENTATION ---------------------
         public:
            wstring  Name, 
                     Value;
         };

         /// <summary>List of {name,value} property pairs for a rich-text tag</summary>
         typedef list<Property>  PropertyList;

         /// <summary>Groups tags by category</summary>
         enum class TagClass { Character, Paragraph, Colour, Special };

         /// <summary>Any valid tag in a rich-text string</summary>
         class RichTag
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            RichTag(TagType t, bool open) : Type(t), Class(GetClass(t)), Opening(open), Closing(!open), SelfClosing(false)
            {}

            RichTag(TagType t, const wstring& txt) : Type(t), Class(TagClass::Special), Text(txt), Opening(false), Closing(false), SelfClosing(false)
            {}

            RichTag(TagType t, const PropertyList& props) : Type(t), Properties(props), Class(GetClass(t)), Opening(true), Closing(false), SelfClosing(false)
            {}

            // -------------------- REPRESENTATION ---------------------
         public:
            bool         Opening,         // Opening tag
                         Closing,         // Closing tag
                         SelfClosing;     // Self-closing tag
            
            wstring      Text;         // Title/Author/Button text
            TagType      Type;         // Tag type
            TagClass     Class;        // Class of tag
            PropertyList Properties;   // {Name,Value} property pairs
         };

         /// <summary>Stack of rich formatting tags</summary>
         class TagStack : protected deque<TagType>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create tag stack</summary>
            /// <param name="cl">The class LogicExport of tags to hold</param>
            TagStack(TagClass cl) : Class(cl)
            {}

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Push or pop a tag from the stack</summary>
            /// <exception cref="Logic::ArgumentException">Tag is of the incorrect class</exception>
            /// <exception cref="Logic::Language::RichTextException">Closing tag doesn't match currently open tag</exception>
            void  PushPop(const RichTag& tag)
            {
               // Ensure tag is of correct class
               if (tag.Class != Class)
                  throw ArgumentException(HERE, L"tag", VString(L"Cannot add %s tag to a %s stack", ::GetString(tag.Type).c_str(), GetString(tag.Class).c_str()) );

               // Open: Push stack
               if (tag.Opening)
                  push_back(tag.Type);

               // Close: Pop stack
               else if (!empty() && back() == tag.Type)
                  pop_back();

               else
                  // Mismatch: Error
                  throw RichTextException(HERE, VString(L"Unexpected closing %s tag", ::GetString(tag.Type).c_str()) );
            }

            // -------------------- REPRESENTATION ---------------------
         private:
            TagClass Class;
         };

         /// <summary>Specialised character tag stack</summary>
         class CharTagStack : public TagStack
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CharTagStack() : TagStack(TagClass::Character)
            {}

            // --------------------- PROPERTIES ------------------------
         public:
            /// <summary>Generate character formatting flags from tags in stack</summary>
            PROPERTY_GET(UINT,Current,GetCurrent);

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Generate character formatting flags from tags in stack</summary>
            UINT  GetCurrent() const
            {
               UINT format = NULL;
               
               // Convert current tags into formating flags
               for (auto& t : *this)
                  switch (t)
                  {
                  case TagType::Bold:       format |= CFE_BOLD;       break;
                  case TagType::Italic:     format |= CFE_ITALIC;     break;
                  case TagType::Underline:  format |= CFE_UNDERLINE;  break;
                  }

               return format;
            }
         };

         /// <summary>Specialised colour tag stack</summary>
         class ColourTagStack : public TagStack
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            ColourTagStack() : TagStack(TagClass::Colour)
            {}
            
            // --------------------- PROPERTIES ------------------------
         public:
            /// <summary>Get colour on top of the stack, or default colour if empty</summary>
            PROPERTY_GET(Colour,Current,GetCurrent);

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Get colour on top of the stack, or default colour if empty</summary>
            /// <exception cref="Logic::AlgorithmException">Current colour tag is unrecognised</exception>
            Colour GetCurrent() const
            {
               // Default if empty
               if (empty())
                  return Colour::Default;

               // Convert tag to colour
               switch (back())
               {
               case TagType::Black:	   return Colour::Black;
               case TagType::Blue:	   return Colour::Blue;     
               case TagType::Cyan:	   return Colour::Cyan;
               case TagType::Green:	   return Colour::Green;
               case TagType::Magenta:	return Colour::Purple;
               case TagType::Orange:	return Colour::Orange;
               case TagType::Red:	   return Colour::Red;
               case TagType::Silver:	return Colour::Silver;
               case TagType::Yellow:	return Colour::Yellow;
               case TagType::White:	   return Colour::White;
               }

               // Error
               throw AlgorithmException(HERE, L"Unrecognised colour tag on top of stack");
            }
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         RichStringParser(const wstring& str, Alignment default = Alignment::Left);
         virtual ~RichStringParser();

         DEFAULT_COPY(RichStringParser);	// Default copy semantics
         DEFAULT_MOVE(RichStringParser);	// Default move semantics

         // ------------------------ STATIC -------------------------
      protected:
         static const wregex IsOpeningTag;
         static const wregex IsClosingTag;
         static const wregex IsBasicTag;
         static const wregex IsTagProperty;
         static const wregex IsAuthorDefinition;
         static const wregex IsTitleDefinition;
         static const wregex IsButtonDefinition;
         static const wregex IsButtonText;

         static Alignment GetAlignment(TagType t);
         static TagClass  GetClass(TagType t);
         static wstring   GetString(TagClass t);
         static TagType   IdentifyTag(const wstring& name);

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(RichParagraph&,FirstParagraph,GetFirstParagraph);

         // ---------------------- ACCESSORS ------------------------			
      public:
         bool  MatchColourCode(CharIterator pos) const;
         bool  MatchTag(CharIterator pos) const;

      protected:
         RichButton*  CreateButton(const RichTag& tag) const;

         // ----------------------- MUTATORS ------------------------
      public:
         
      protected:
         void    Parse();

      private:
         RichParagraph& GetFirstParagraph();
         Colour  ReadColourCode(CharIterator& pos);
         RichTag ReadTag(CharIterator& pos);
         void    SetColumnInfo(const RichTag& tag);
         
         // -------------------- REPRESENTATION ---------------------
      public:
         RichString      Output;

      protected:
         wstring         Input;
         ColourTagStack  Colours;
         CharTagStack    Formatting;
         TagStack        Alignments;
         Alignment       Default;
      };

   }
}

using namespace Logic::Language;