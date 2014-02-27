#include "stdafx.h"
#include "RichEditImage.h"

namespace GUI
{
   namespace Controls
   {
      namespace OLE
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Creates an empty RichEdit image</summary>
         RichEditImage::RichEditImage() 
         {
            ZeroMemory(&m_oData, sizeof(STGMEDIUM));
            ZeroMemory(&m_oFormat, sizeof(FORMATETC));

            m_bDataOwner = TRUE;
            m_iRefCount  = 0;
            m_pEvents    = NULL;
         }

         /// <summary>Release data</summary>
         RichEditImage::~RichEditImage()
         {
            // [CHECK] Destroy data if we're the owner
	         if (m_bDataOwner)
		         ::ReleaseStgMedium(&m_oData);
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Increment the reference count </summary>
         /// <returns></returns>
         STDMETHODIMP_(ULONG)  RichEditImage::AddRef()
         {
	         return ++m_iRefCount;
         }

         /// <summary>Creates an OLE picture.</summary>
         /// <param name="pOleClientSite">The OLE client site.</param>
         /// <param name="pStorage">The storage.</param>
         /// <returns></returns>
         IOleObject*  RichEditImage::Create(IOleClientSite*  pOleClientSite, IStorage*  pStorage)
         {
            IOleObject* pOleObject = NULL;
            HRESULT     hResult;

            // [CHECK] Ensure we have a bitmap
	         ASSERT(m_oData.hBitmap);

            // Create OleObject
	         hResult = ::OleCreateStaticFromData(this, IID_IOleObject, OLERENDER_FORMAT, &m_oFormat, pOleClientSite, pStorage, (void**)&pOleObject);
	         ASSERT(hResult == S_OK);

	         return pOleObject;
         }
         
         /// <summary>Register for a 'data changed' notification</summary>
         /// <param name="pFormat">format to monitor.</param>
         /// <param name="dwFlags">flags.</param>
         /// <param name="pOutput">The OLE object to call when changes occurr.</param>
         /// <param name="pdwConnectionID">Unique ID to identify the event</param>
         /// <returns>S_OK if succesful</returns>
         STDMETHODIMP RichEditImage::DAdvise(FORMATETC*  pFormat, DWORD  dwFlags, IAdviseSink*  pOutput, DWORD*  pdwConnectionID)
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
         STDMETHODIMP  RichEditImage::DUnadvise(DWORD dwConnectionID)
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
         STDMETHODIMP  RichEditImage::GetData(FORMATETC*  pFormat, STGMEDIUM*  pOutput)
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
         STDMETHODIMP  RichEditImage::QueryInterface(REFIID  iID, void**  pInterface)
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
         STDMETHODIMP_(ULONG)  RichEditImage::Release()
         {
	         if (--m_iRefCount == 0)
		         delete this;
	
	         return m_iRefCount;
         }

         /// <summary>Sets the image by Bitmap handle.</summary>
         /// <param name="hBitmap">The bitmap handle.</param>
         VOID  RichEditImage::SetBitmap(HBITMAP  hBitmap)
         {
            STGMEDIUM  oData;
            FORMATETC  oFormat;

	         ASSERT(hBitmap);

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
	         SetData(&oFormat, &oData, TRUE);		
         }

         /// <summary>Sets the image data.</summary>
         /// <param name="pFormat">The format.</param>
         /// <param name="pData">The data.</param>
         /// <param name="bDataOwner">Whether the object owns the new data and is responsible for destroying it</param>
         /// <returns>S_OK</returns>
         STDMETHODIMP  RichEditImage::SetData(FORMATETC*  pFormat, STGMEDIUM*  pData, BOOL  bDataOwner)
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

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
