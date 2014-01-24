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
         : ID(id), Title(title), Description(desc), Voiced(voice),
           Strings(id)

      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Gets the category/group.</summary>
      /// <returns></returns>
      PageGroup  LanguagePage::GetGroup() const
      {
         if (ID >= 6000)
            return PageGroup::USER;
         else if (ID >= 3800)
            return PageGroup::NPC;
         else if (ID >= 3200)
            return PageGroup::PLOT;
         else if (ID >= 2032)
            return PageGroup::NEWS;
         else if (ID >= 1999)
            return PageGroup::EDITOR;
         else if (ID >= 1701)
            return PageGroup::MENU;
         else if (ID >= 1500)
            return PageGroup::BONUS;
         else if (ID >= 1290)
            return PageGroup::QUEST;
         else if (ID >= 1000)
            return PageGroup::MISC;
         else if (ID >= 101)
            return PageGroup::DIALOGUE;
         else
            return PageGroup::DATA;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
   }

}