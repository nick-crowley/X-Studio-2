#pragma once
#include "Logic/ScriptFile.h"
#include "Logic/ScriptObjectLibrary.h"
#include "DocumentBase.h"
#include "PropertySource.h"

/// <summary>Forward declarations</summary>
FORWARD_DECLARATION2(GUI,Controls,class ScriptEdit)
FORWARD_DECLARATION2(GUI,Views,class ScriptView)

/// <summary>Documents</summary>
NAMESPACE_BEGIN2(GUI,Documents)

   /// <summary>MSCI Script document template</summary>
   class ScriptDocTemplate : public CMultiDocTemplate
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ScriptDocTemplate();

      // ------------------------ STATIC -------------------------

      DECLARE_DYNAMIC(ScriptDocTemplate)

      // ---------------------- ACCESSORS ------------------------	
      virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
   };

   /// <summary>MSCI Script document</summary>
   class ScriptDocument : public DocumentBase, public PropertySource
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Script property base</summary>
      class ScriptProperty : public ValidatingProperty
      {
      public:
         ScriptProperty(ScriptDocument& d, wstring name, _variant_t val, wstring desc)
            : Document(d), Script(d.Script), ValidatingProperty(name.c_str(), val, desc.c_str(), NULL, nullptr, nullptr, nullptr)
         {}

      protected:
         /// <summary>Modify document</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Document.SetModifiedFlag(TRUE);
         }

      protected:
         ScriptDocument&  Document;
         ScriptFile&      Script;
      };

      /// <summary>Script argument property grid item</summary>
      class ArgumentProperty : public ScriptProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create argument property.</summary>
         /// <param name="arg">Argument.</param>
         ArgumentProperty(ScriptDocument& doc, ScriptVariable& arg)
            : Argument(arg), ScriptProperty(doc, arg.Name, GetString(arg.ValueType).c_str(), arg.Description)
         {
            // Populate parameter types 
            for (const ScriptObject& obj : ScriptObjectLib)
               if (obj.Group == ScriptObjectGroup::ParameterType && !obj.IsHidden())
                  AddOption(obj.Text.c_str(), FALSE);

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update parameter type</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Argument.ValueType = (ParameterType)ScriptObjectLib.Find(value).ID;
            
            ScriptProperty::OnValueChanged(value);       // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
         ScriptVariable& Argument;
      };

      /// <summary>Command ID property grid item</summary>
      class CommandIDProperty : public ScriptProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create command ID property.</summary>
         /// <param name="s">Script.</param>
         CommandIDProperty(ScriptDocument& doc)
            : ScriptProperty(doc, L"Command", doc.Script.CommandName.c_str(), L"ID of ship/station command implemented by the script")
         {
            // Populate command IDs 
            for (const ScriptObject& obj : ScriptObjectLib)
               if (obj.Group == ScriptObjectGroup::ObjectCommand)
                  AddOption(obj.Text.c_str(), FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Validates and optionally modifies the value being input</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            const ScriptObject* obj = nullptr;
            int index = 0;
            
            // Command ID: Allow manually input numeric IDs
            if (value.IsNumeric())
            {
               // Replace with name if possible
               if (ScriptObjectLib.TryFind(ScriptObjectGroup::ObjectCommand, value.ToInt(), obj))
                  value = obj->Text;
            }
            // Command Name: Replace with exact match if present
            else if (TryFind(value, index))
               value = GetOption(index);

            // Always accept values
            return true;
         }

         /// <summary>Update command ID</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            if (value.IsNumeric())
               Script.CommandID = value.ToInt();
            else
               Script.CommandID = value;

            ScriptProperty::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Script description property grid item</summary>
      class DescriptionProperty : public ScriptProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create description property.</summary>
         /// <param name="s">Script.</param>
         DescriptionProperty(ScriptDocument& doc)
            : ScriptProperty(doc, L"Description", doc.Script.Description.c_str(), L"Short description of functionality")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update description</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Description = value;
            
            ScriptProperty::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Game version property grid item</summary>
      class GameVersionProperty : public ScriptProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create game version property.</summary>
         /// <param name="s">Script.</param>
         GameVersionProperty(ScriptDocument& doc)
            : ScriptProperty(doc, L"Game Required", VersionString(doc.Script.Game).c_str(),  L"Minimum version of game required")
              
         {
            // Populate game versions
            AddOption(VersionString(GameVersion::Threat).c_str(), FALSE);
            AddOption(VersionString(GameVersion::Reunion).c_str(), FALSE);
            AddOption(VersionString(GameVersion::TerranConflict).c_str(), FALSE);
            AddOption(VersionString(GameVersion::AlbionPrelude).c_str(), FALSE);

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update game version</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Game = GameVersionIndex(Find(value.c_str())).Version;      // Convert zero-based index into GameVersion
            
            ScriptProperty::OnValueChanged(value);    // Modify document
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Script name property grid item</summary>
      class NameProperty : public ScriptProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create name property.</summary>
         /// <param name="s">Script.</param>
         NameProperty(ScriptDocument& doc)
            : ScriptProperty(doc, L"Name", doc.Script.Name.c_str(), L"How script is referenced throughout the game")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Update name</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Name = value;

            // Modify document
            ScriptProperty::OnValueChanged(value);    
            
            // Change path [+title]
            Document.Rename(value + Document.FullPath.Extension);
         }
         
         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Egosoft signature grid item</summary>
      class SignedProperty : public CMFCPropertyGridProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create read-only Egosoft signature property.</summary>
         /// <param name="val">Signed property.</param>
         SignedProperty(bool val)
            : CMFCPropertyGridProperty(L"Signed", (_variant_t)val, L"Whether script has been signed by Egosoft")
         {
            Enable(FALSE);
         }
      };

      /// <summary>Script name property grid item</summary>
      class VersionProperty : public ScriptProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create version property.</summary>
         /// <param name="s">Script.</param>
         VersionProperty(ScriptDocument& doc)
            : ScriptProperty(doc, L"Version", doc.Script.Version, L"Current version number")
         {
         }

         // ---------------------- ACCESSORS ------------------------	

         // ----------------------- MUTATORS ------------------------
      protected:
         /// <summary>Ensures version is numeric</summary>
         /// <param name="value">The value.</param>
         /// <returns>True to accept, false to reject</returns>
         bool OnValidateValue(GuiString& value) override
         {
            return !value.length() || (value.IsNumeric() && value.ToInt() > 0);
         }

         /// <summary>Update version</summary>
         /// <param name="value">value text</param>
         void OnValueChanged(GuiString value) override
         {
            Script.Version = value.length() ? value.ToInt() : 0;

            // Modify document
            ScriptProperty::OnValueChanged(value);
         }

         // -------------------- REPRESENTATION ---------------------
      protected:
      };
      
      // --------------------- CONSTRUCTION ----------------------

   protected: // create from serialization only
	   ScriptDocument();
   public:
	   virtual ~ScriptDocument();

      // ------------------------ STATIC -------------------------
   protected:
      DECLARE_DYNCREATE(ScriptDocument)
	   DECLARE_MESSAGE_MAP()

   public:
      static ScriptDocument*  GetActive();

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      CHARRANGE               GetSelection() const override;
      GUI::Views::ScriptView* GetView() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  AttachEdit(GUI::Controls::ScriptEdit& edit);
      bool  FindNext(UINT start, MatchData& src) const override;
      bool  Replace(MatchData& m) override;
      void  Replace(const wstring& txt);
      void  SetSelection(CHARRANGE rng) override;

      handler void OnDisplayProperties(CMFCPropertyGridCtrl& grid) override;
      afx_msg void OnInsertArgument();
	   handler BOOL OnNewDocument() override;
      handler BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
      handler void OnQueryCustomCommand(CCmdUI* pCmd) override;
      virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
   
      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptFile  Script;

   protected:
      GUI::Controls::ScriptEdit* Edit;
      list<ArgumentProperty*>    ArgumentProperties;
   };


NAMESPACE_END2(GUI,Documents)

