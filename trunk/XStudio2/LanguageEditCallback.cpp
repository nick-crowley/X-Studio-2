#include "stdafx.h"
#include "LanguageEdit.h"

namespace GUI
{
   namespace Controls
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an empty RichEdit image</summary>
      LanguageEdit::EditCallback::EditCallback(LanguageEdit* edit) : Edit(edit), RefCount(0) 
      {
      }

      /// <summary></summary>
      LanguageEdit::EditCallback::~EditCallback()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Increment the reference count </summary>
      /// <returns></returns>
      STDMETHODIMP_(ULONG)  LanguageEdit::EditCallback::AddRef()
      {
	      return ++RefCount;
      }
      
      /// <summary>Notifies the language edit a button has been removed.</summary>
      /// <param name="pOleObject">The OLE object.</param>
      /// <returns></returns>
      STDMETHODIMP   LanguageEdit::EditCallback::DeleteObject(IOleObject*  pOleObject)
      {
         // Notify parent
         Edit->OnButtonRemoved(pOleObject);

         // Return OK
         return S_OK;
      }

      /// <summary>Gets the context menu.</summary>
      /// <param name="iSelectionType">Type of the selection.</param>
      /// <param name="piObject">The object.</param>
      /// <param name="pSelection">selection.</param>
      /// <param name="pOutput">Menu handle.</param>
      /// <returns></returns>
      STDMETHODIMP  LanguageEdit::EditCallback::GetContextMenu(WORD  iSelectionType, IOleObject*  piObject, 
                                                               CHARRANGE*  pSelection, HMENU*  pOutput)
      {
         CMenu menu;

         // Load RichEdit menu
	      menu.LoadMenu(IDM_EDIT_POPUP);
	      CMenu* popup = menu.GetSubMenu(0);
         
         // Return without destroying menu.
         *pOutput = popup->Detach();
         return S_OK;
      }

      /// <summary>Queries whether to accept data.</summary>
      /// <param name="pDataObject">The p data object.</param>
      /// <param name="lpcfFormat">The LPCF format.</param>
      /// <param name="reco">The reco.</param>
      /// <param name="fReally">The f really.</param>
      /// <param name="hMetaPict">The h meta pict.</param>
      /// <returns>Always S_OK</returns>
      STDMETHODIMP LanguageEdit::EditCallback::QueryAcceptData(IDataObject* pDataObject, CLIPFORMAT* lpcfFormat, 
                                                               DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
      {
         return S_OK; 
      }

      /// <summary>Queries whether to accept an object.</summary>
      /// <param name="pCLSID">CLSID.</param>
      /// <param name="pStorage">storage.</param>
      /// <param name="cp">location</param>
      /// <returns>Always S_OK</returns>
      STDMETHODIMP  LanguageEdit::EditCallback::QueryInsertObject(CLSID* pCLSID, IStorage* pStorage, LONG cp)
      { 
         return S_OK; 
      }
         
      /// <summary>Queries and AddRef's the interface.</summary>
      /// <param name="iID">The identifier.</param>
      /// <param name="pInterface">The interface.</param>
      /// <returns></returns>
      STDMETHODIMP  LanguageEdit::EditCallback::QueryInterface(REFIID  iID, void**  pInterface)
      {
         // Set interface
	      if (iID == IID_IUnknown)
            *pInterface = static_cast<IUnknown*>(this);

         else if (iID == IID_IRichEditOleCallback)
            *pInterface = static_cast<IRichEditOleCallback*>(this);

	      else
         {
            *pInterface = nullptr;
            return E_NOINTERFACE;
         }

         // Success: Addref
         AddRef();
         return S_OK;
      }

      /// <summary>Releases this instance.</summary>
      /// <returns></returns>
      STDMETHODIMP_(ULONG)  LanguageEdit::EditCallback::Release()
      {
	      if (--RefCount == 0)
		      delete this;
	
	      return RefCount;
      }


      // ------------------------------ PROTECTED METHODS -----------------------------
         
      // ------------------------------- PRIVATE METHODS ------------------------------
   }
}
