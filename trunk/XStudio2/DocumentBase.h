#pragma once
#include "Logic/Path.h"
#include "Logic/MatchData.h"

/// <summary>User interface documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)
   
   /// <summary></summary>
   enum class DocumentType { Script, Language };

   /// <summary></summary>
   class DocumentBase : public CDocument
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      DocumentBase();    // Protected constructor used by dynamic creation
   public:
      virtual ~DocumentBase();
       
      // ------------------------ STATIC -------------------------
   public:
      static DocumentBase* GetActive();

      DECLARE_DYNCREATE(DocumentBase)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      IO::Path     GetFullPath() const;
      DocumentType GetType() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void          Activate();
      virtual bool  FindNext(MatchData& src);
      virtual bool  Replace(MatchData& src);
      virtual void  SetSelection(CHARRANGE rng);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      DocumentType  Type;
   };

   /// <summary>List of documents</summary>
   typedef list<DocumentBase*>  DocumentList;
   

/// <summary>User interface documents</summary>
NAMESPACE_END2(GUI,Documents)

