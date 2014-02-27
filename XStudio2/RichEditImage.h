#pragma once

#include "Logic/Common.h"

namespace GUI
{
   namespace Controls
   {
      namespace OLE
      {
         /// <summary></summary>
         class RichEditImage : public IDataObject
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------   
         public:
	         RichEditImage();
	         ~RichEditImage();

            NO_COPY(RichEditImage);	// Default copy semantics
            NO_MOVE(RichEditImage);	// Default move semantics

            
            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // --------------------- INTERFACES ------------------------
            
         // IUnknown
            STDMETHOD(QueryInterface)(REFIID  iID, void** ppInterface);
	         STDMETHOD_(ULONG,AddRef)();
	         STDMETHOD_(ULONG,Release)();
   
         // IDataObject
            STDMETHOD(GetData)(FORMATETC*  pFormat, STGMEDIUM*  pOutput);
            STDMETHOD(SetData)(FORMATETC*  pFormat, STGMEDIUM*  pData, BOOL  bDataOwner);
            STDMETHOD(DAdvise)(FORMATETC*  pFormat, DWORD  dwFlags, IAdviseSink*  pOutput, DWORD*  pdwConnectionID);
            STDMETHOD(DUnadvise)(DWORD dwConnectionID);

	      // Not Implemented
	         STDMETHOD(EnumFormatEtc)(DWORD  dwDirection , IEnumFORMATETC**  ppenumFormatEtc )      { return E_NOTIMPL; };
            STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise)                                   { return E_NOTIMPL; };
            STDMETHOD(GetCanonicalFormatEtc)(FORMATETC*  pformatectIn ,FORMATETC* pformatetcOut )  { return E_NOTIMPL; };
            STDMETHOD(GetDataHere)(FORMATETC* pformatetc, STGMEDIUM*  pmedium)                     { return E_NOTIMPL; };
	         STDMETHOD(QueryGetData)(FORMATETC*  pformatetc )                                       { return E_NOTIMPL; };

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
	         IOleObject*   Create(IOleClientSite*  pOleClientSite, IStorage*  pStorage);
            void          SetBitmap(HBITMAP  hBitmap);

            // -------------------- REPRESENTATION ---------------------

         private:
            // Data
	         INT	        m_iRefCount;
	         BOOL	        m_bDataOwner;
	         STGMEDIUM     m_oData;
            FORMATETC     m_oFormat;

            // Events
            IDataAdviseHolder*  m_pEvents;
         };
      }
   }
}

using namespace GUI::Controls::OLE;
