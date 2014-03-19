#include "stdafx.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /*LanguagePage::LanguagePage() : ID(0), Voiced(false)
      {
      }*/

      LanguagePage::LanguagePage(UINT id, wstring title, wstring desc, bool voice) 
         : ID(id), Title(title), Description(desc), Voiced(voice), Strings(id)
      {
      }

      LanguagePage::LanguagePage(LanguagePage&& r) 
         : ID(r.ID), Title(move(r.Title)), Description(move(r.Description)), Voiced(r.Voiced), Strings(move(r.Strings))
      {
      }

      LanguagePage::~LanguagePage()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Gets the group for a page id</summary>
      /// <param name="page">The page ID</param>
      /// <returns></returns>
      PageGroup  LanguagePage::IdentifyGroup(UINT page) 
      {
         if (page >= 6000)
            return PageGroup::USER;
         else if (page >= 3800)
            return PageGroup::NPC;
         else if (page >= 3200)
            return PageGroup::PLOT;
         else if (page >= 2032)
            return PageGroup::NEWS;
         else if (page >= 1999)
            return PageGroup::EDITOR;
         else if (page >= 1701)
            return PageGroup::MENU;
         else if (page >= 1500)
            return PageGroup::BONUS;
         else if (page >= 1290)
            return PageGroup::QUEST;
         else if (page >= 1000)
            return PageGroup::MISC;
         else if (page >= 101)
            return PageGroup::DIALOGUE;
         else
            return PageGroup::DATA;
      }

      /// <summary>Write string to console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, LanguageStringRef str)
      {
         return c << Cons::White << "{" 
                  << Cons::Yellow << "LanguageString" 
                  << Cons::White << " id=" << str.ID << " page=" << str.Page << " text=" << str.Text 
                  << "}";
      }

      /// <summary>Write page to console</summary>
      LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, LanguagePageRef page)
      {
         return c << Cons::White << "{" 
                  << Cons::Yellow << "LanguagePage" 
                  << Cons::White << " id=" << page.ID << " title=" << page.Title << " desc=" << page.Description << " size=" << page.Strings.size()
                  << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Finds an available ID for a new string</summary>
      /// <param name="start">ID of insertion point, or -1 to append after last string.</param>
      /// <returns>ID preceeding input, if available, otherwise the first ID following the input.</returns>
      int  LanguagePage::GetAvailableID(int start) const
      {
         // Empty/Append: Return LastID+1
         if (start == -1 || Strings.empty())
            return !Strings.empty() ? (--Strings.end())->first+1 : 1;

         // Prev Available: Use previous ID
         if (Strings.find(start-1) == Strings.end())
            return start-1;

         // Find next ID
         auto pos = Strings.find(start);
         for (int id = start; pos != Strings.end(); id++)
         {
            // Gap Detected: Use first available ID
            if ((UINT)id < (pos++)->first)
               return id;
         }

         // Contiguous: Use LastID+1
         return (--Strings.end())->first+1;
      }

      /// <summary>Gets the category/group.</summary>
      /// <returns></returns>
      PageGroup  LanguagePage::GetGroup() const
      {
         return IdentifyGroup(ID);
      }

      /// <summary>Convert to XML [Used for copying to clipboard]</summary>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
      GuiString  LanguagePage::ToXML() const
      {
         VString xml(L"<page id='%d' title='%s' descr='%s' voice='%s'>", ID, Title.c_str(), Description.c_str(), Voiced ? L"yes" : L"no");
         
         // Append strings
         for (auto& str : Strings)
         {
            xml += L"\r\n";
            xml += str.second.ToXML();
         }

         // Close page
         xml += L"\r\n</page>";
         return xml;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
   }

}