#include "stdafx.h"
#include "OleBitmap.h"

namespace GUI
{
   namespace Utils
   {
      
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an empty RichEdit image</summary>
      OleBitmap::OleBitmap() 
      {
         ZeroMemory(&m_oData, sizeof(STGMEDIUM));
         ZeroMemory(&m_oFormat, sizeof(FORMATETC));

         m_bDataOwner = TRUE;
         m_iRefCount  = 0;
         m_pEvents    = NULL;
      }

      /// <summary>Create a non-empty RichEdit image</summary>
      /// <param name="bitmap">The bitmap.</param>
      /// <exception cref="Logic::ArgumentNullException">Bitmap is null</exception>
      /// <exception cref="_com_error">Unable to set image data</exception>
      OleBitmap::OleBitmap(HBITMAP bitmap) : OleBitmap()
      {
         REQUIRED(bitmap);

         AddRef();
         SetBitmap(bitmap);
      }

      /// <summary>Release data</summary>
      OleBitmap::~OleBitmap()
      {
         // [CHECK] Destroy data if we're the owner
	      if (m_bDataOwner)
		      ::ReleaseStgMedium(&m_oData);
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Creates an OLE picture.</summary>
      /// <param name="pOleClientSite">The OLE client site.</param>
      /// <param name="pStorage">The storage.</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentNullException">Bitmap is null</exception>
      /// <exception cref="_com_error">Unable to set image data</exception>
      IOleObjectPtr  OleBitmap::CreateStatic(HBITMAP bitmap, IOleClientSitePtr clientSite, IStoragePtr storage)
      {
         REQUIRED(bitmap);
         REQUIRED(clientSite);
         REQUIRED(storage);

         // Create image source
	      auto img = new OleBitmap(bitmap);

         // Create static copy
         IOleObjectPtr object;
	      HRESULT hr = ::OleCreateStaticFromData(img, IID_IOleObject, OLERENDER_FORMAT, &img->m_oFormat, clientSite, storage, (void**)&object);
         if (FAILED(hr))
	         _com_issue_errorex(hr, img, __uuidof(IDataObject));

	      return object;
      }
         
      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Increment the reference count </summary>
      /// <returns></returns>
      STDMETHODIMP_(ULONG)  OleBitmap::AddRef()
      {
	      return ++m_iRefCount;
      }

      /// <summary>Register for a 'data changed' notification</summary>
      /// <param name="pFormat">format to monitor.</param>
      /// <param name="dwFlags">flags.</param>
      /// <param name="pOutput">The OLE object to call when changes occurr.</param>
      /// <param name="pdwConnectionID">Unique ID to identify the event</param>
      /// <returns>S_OK if succesful</returns>
      STDMETHODIMP OleBitmap::DAdvise(FORMATETC*  pFormat, DWORD  dwFlags, IAdviseSink*  pOutput, DWORD*  pdwConnectionID)
      {
         HRESULT  hResult;    // Operation result

         // Create events object if necessary
         if (!m_pEvents && FAILED(hResult = CreateDataAdviseHolder(&m_pEvents)))
            return hResult;

         // Register connect using events object
         return m_pEvents->Advise(this, pFormat, dwFlags, pOutput, pdwConnectionID);
      }

      /// <summary>Un-register a previously registered 'data changed' notification</summary>
      /// <param name="dwConnectionID">The connection identifier.</param>
      /// <returns>S_OK if succesful</returns>
      STDMETHODIMP  OleBitmap::DUnadvise(DWORD dwConnectionID)
      {
         // [CHECK] Events object exists
         if (!m_pEvents)
            return E_UNEXPECTED;

         // Un-register event using events object
         return m_pEvents->Unadvise(dwConnectionID);
      }

      /// <summary>Gets a copy of the data.</summary>
      /// <param name="pFormat">Requested format</param>
      /// <param name="pOutput">output.</param>
      /// <returns>S_OK if succesful or E_HANDLE otherwise</returns>
      STDMETHODIMP  OleBitmap::GetData(FORMATETC*  pFormat, STGMEDIUM*  pOutput)
      {
	      HBITMAP  hBitmapCopy;

         // Duplicate bitmap 
	      hBitmapCopy = (HBITMAP)::OleDuplicateData(m_oData.hBitmap, CF_BITMAP, NULL);
         // [CHECK] Bitmap handle was valid
	      if (hBitmapCopy == NULL)
		      return E_HANDLE;
	
         // Set result 
	      pOutput->tymed   = TYMED_GDI;
	      pOutput->hBitmap = hBitmapCopy;
	      pOutput->pUnkForRelease = NULL;

	      return S_OK;
      }

         
      /// <summary>Queries and AddRef's the interface.</summary>
      /// <param name="iID">The identifier.</param>
      /// <param name="pInterface">The interface.</param>
      /// <returns></returns>
      STDMETHODIMP  OleBitmap::QueryInterface(REFIID  iID, void**  pInterface)
      {
         // Set interface
	      if (iID == IID_IUnknown)
            *pInterface = static_cast<IUnknown*>(this);

         else if (iID == IID_IDataObject)
            *pInterface = static_cast<IDataObject*>(this);

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
      STDMETHODIMP_(ULONG)  OleBitmap::Release()
      {
	      if (--m_iRefCount == 0)
		      delete this;
	
	      return m_iRefCount;
      }

      /// <summary>Sets the image data.</summary>
      /// <param name="pFormat">The format.</param>
      /// <param name="pData">The data.</param>
      /// <param name="bDataOwner">Whether the object owns the new data and is responsible for destroying it</param>
      /// <returns>S_OK</returns>
      STDMETHODIMP  OleBitmap::SetData(FORMATETC*  pFormat, STGMEDIUM*  pData, BOOL  bDataOwner)
      {
         // [CHECK] Destroy existing data, if any
         if (m_bDataOwner && m_oData.hBitmap)
		      ::ReleaseStgMedium(&m_oData);

         // Save new data
	      m_oFormat    = *pFormat;
	      m_oData      = *pData;
         m_bDataOwner = bDataOwner;

	      return S_OK;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------
         
      /// <summary>Sets the image by Bitmap handle.</summary>
      /// <param name="hBitmap">The bitmap handle.</param>
      /// <exception cref="Logic::ArgumentNullException">Bitmap is null</exception>
      /// <exception cref="_com_error">Unable to set image data</exception>
      void  OleBitmap::SetBitmap(HBITMAP  hBitmap)
      {
         STGMEDIUM  oData;
         FORMATETC  oFormat;
         HRESULT    hr;

	      REQUIRED(hBitmap);

         /// Data
	      oData.tymed   = TYMED_GDI;	
	      oData.hBitmap = hBitmap;
	      oData.pUnkForRelease = NULL;

	      /// Data format
	      oFormat.cfFormat = CF_BITMAP;				// Clipboard format = CF_BITMAP
	      oFormat.ptd      = NULL;					// Target Device = Screen
	      oFormat.dwAspect = DVASPECT_CONTENT;	// Level of detail = Full content
	      oFormat.lindex   = -1;						// Index = Not applicaple
	      oFormat.tymed    = TYMED_GDI;				// Storage medium = HBITMAP handle

         // Save data
	      if (FAILED(hr=SetData(&oFormat, &oData, TRUE)))
            _com_issue_errorex(hr, this, __uuidof(IDataObject));
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
      
   }
}
