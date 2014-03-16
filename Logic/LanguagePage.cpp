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
      ConsoleWnd& operator<<(ConsoleWnd& c, LanguageStringRef str)
      {
         return c << Cons::White << "{" 
                  << Cons::Yellow << "LanguageString" 
                  << Cons::White << " id=" << str.ID << " page=" << str.Page << " text=" << str.Text 
                  << "}";
      }

      /// <summary>Write page to console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, LanguagePageRef page)
      {
         return c << Cons::White << "{" 
                  << Cons::Yellow << "LanguagePage" 
                  << Cons::White << " id=" << page.ID << " title=" << page.Title << " desc=" << page.Description << " size=" << page.Strings.size()
                  << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Gets the category/group.</summary>
      /// <returns></returns>
      PageGroup  LanguagePage::GetGroup() const
      {
         return IdentifyGroup(ID);
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
   }

}