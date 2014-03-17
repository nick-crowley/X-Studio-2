#include "stdafx.h"
#include "ArgumentDialog.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   //IMPLEMENT_DYNCREATE(ArgumentDialog, CDialog)

   BEGIN_MESSAGE_MAP(ArgumentDialog, CDialog)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Create modify-argument dialog</summary>
   /// <param name="script">script containing the argument.</param>
   /// <param name="arg">argument.</param>
   /// <param name="parent">parent wnd.</param>
   ArgumentDialog::ArgumentDialog(ScriptFile& script, const ScriptVariable& arg, CWnd* parent) 
       : CDialog(IDD, parent), 
         Script(script),
         Argument(arg),
         Name(arg.Name),
         Description(arg.Description)
   {
   }

   /// <summary>Create new-argument dialog</summary>
   /// <param name="script">script.</param>
   /// <param name="parent">parent wnd.</param>
   ArgumentDialog::ArgumentDialog(ScriptFile& script, CWnd* parent) 
       : CDialog(IDD, parent), 
         Script(script)
   {
   }

   ArgumentDialog::~ArgumentDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Populates the argument properties.</summary>
   /// <returns></returns>
   BOOL ArgumentDialog::OnInitDialog()
   {
      // Init
      __super::OnInitDialog();

      try
      {
         // Populate parameter types 
         for (const ScriptObject& obj : ScriptObjectLib)
            if (obj.Group == ScriptObjectGroup::ParameterType && !obj.IsHidden())
            {
               int item = Types.InsertString(-1, obj.Text.c_str());
               Types.SetItemData(item, obj.ID);
            }

         // Select current type  [New Arg has no type]
         if (Argument.ValueType != ParameterType::UNRECOGNISED)
            Types.SelectString(-1, ScriptObjectLib.Find(ScriptObjectGroup::ParameterType, (UINT)Argument.ValueType).Text.c_str());
         else
            Types.SetCurSel(0);
         
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, L"Unable to initialise the argument dialog");
      }

      return TRUE;
   }
   
   /// <summary>Verifies argument name.</summary>
   void ArgumentDialog::OnOK()
   { 
      UpdateData(TRUE);

      // Require unique argument name
      if (Script.Variables.Contains(Name) || Types.GetCurSel() == CB_ERR)
         return;

      // Update argument
      Argument.Name        = Name;
      Argument.Description = Description;
      Argument.ValueType   = static_cast<ParameterType>(Types.GetItemData(Types.GetCurSel()));

      // Close
      __super::OnOK();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  ArgumentDialog::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect wnd(this);

      // TODO: Layout code
   }
   
   /// <summary>Does the data exchange.</summary>
   /// <param name="pDX">The p dx.</param>
   void ArgumentDialog::DoDataExchange(CDataExchange* pDX)
   {
      __super::DoDataExchange(pDX);

      DDX_Control(pDX, IDC_TYPE_COMBO, Types);
      DDX_Text(pDX, IDC_NAME_EDIT, Name);
      DDX_Text(pDX, IDC_DESCRIPTION_EDIT, Description);
   }
   
   /// <summary>Adjusts the layout on resize</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void ArgumentDialog::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)


