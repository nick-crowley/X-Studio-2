
// MFC Test 1Doc.h : interface of the ScriptDocument class
//
#pragma once

/// <summary>User interface</summary>
namespace GUI
{

   class ScriptDocument : public CDocument
   {
   protected: // create from serialization only
	   ScriptDocument();
	   DECLARE_DYNCREATE(ScriptDocument)

   // Attributes
   public:

   // Operations
   public:

   // Overrides
   public:
	   virtual BOOL OnNewDocument();
	   virtual void Serialize(CArchive& ar);
   #ifdef SHARED_HANDLERS
	   virtual void InitializeSearchContent();
	   virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
   #endif // SHARED_HANDLERS

   // Implementation
   public:
	   virtual ~ScriptDocument();
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif

   protected:

   // Generated message map functions
   protected:
	   DECLARE_MESSAGE_MAP()

   #ifdef SHARED_HANDLERS
	   // Helper function that sets search content for a Search Handler
	   void SetSearchContent(const CString& value);
   #endif // SHARED_HANDLERS
   };

}

using namespace GUI;
