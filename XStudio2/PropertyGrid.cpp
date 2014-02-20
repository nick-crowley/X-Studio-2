#include "stdafx.h"
#include "PropertyGrid.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(PropertyGrid, CMFCPropertyGridCtrl)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   PropertyGrid::PropertyGrid()
   {
   }

   PropertyGrid::~PropertyGrid()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /*BOOL  PropertyGrid::ValidateItemData(CMFCPropertyGridProperty* prop) 
   {
      if (auto p = dynamic_cast<ValidatingProperty*>(prop))
         return p->OnValidateValue() ? TRUE : FALSE;

      return TRUE;
   }*/

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)


