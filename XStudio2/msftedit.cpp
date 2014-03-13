// Created by Microsoft (R) C/C++ Compiler Version 12.00.21005.1 (180a4026).
//
// d:\my projects\xstudio2\xstudio2\release\msftedit.tli
//
// Wrapper implementations for Win32 type library Msftedit.dll
// compiler-generated file created 03/12/14 at 18:54:31 - DO NOT EDIT!

//#pragma once
#include "stdafx.h"
#include "msftedit.tlh"

namespace TOM {

   //
   // interface ITextFont wrapper method implementations
   //

   ITextFontPtr ITextFont::GetDuplicate ( ) {
       struct ITextFont * _result = 0;
       HRESULT _hr = get_Duplicate(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextFontPtr(_result, false);
   }

   void ITextFont::PutDuplicate ( struct ITextFont * ppFont ) {
       HRESULT _hr = put_Duplicate(ppFont);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::CanChange ( ) {
       long _result = 0;
       HRESULT _hr = raw_CanChange(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextFont::IsEqual ( struct ITextFont * pFont ) {
       long _result = 0;
       HRESULT _hr = raw_IsEqual(pFont, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextFont::Reset ( long Value ) {
       HRESULT _hr = raw_Reset(Value);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextFont::GetStyle ( ) {
       long _result = 0;
       HRESULT _hr = get_Style(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutStyle ( long pValue ) {
       HRESULT _hr = put_Style(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetAllCaps ( ) {
       long _result = 0;
       HRESULT _hr = get_AllCaps(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutAllCaps ( long pValue ) {
       HRESULT _hr = put_AllCaps(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetAnimation ( ) {
       long _result = 0;
       HRESULT _hr = get_Animation(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutAnimation ( long pValue ) {
       HRESULT _hr = put_Animation(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetBackColor ( ) {
       long _result = 0;
       HRESULT _hr = get_BackColor(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutBackColor ( long pValue ) {
       HRESULT _hr = put_BackColor(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetBold ( ) {
       long _result = 0;
       HRESULT _hr = get_Bold(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutBold ( long pValue ) {
       HRESULT _hr = put_Bold(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetEmboss ( ) {
       long _result = 0;
       HRESULT _hr = get_Emboss(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutEmboss ( long pValue ) {
       HRESULT _hr = put_Emboss(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetForeColor ( ) {
       long _result = 0;
       HRESULT _hr = get_ForeColor(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutForeColor ( long pValue ) {
       HRESULT _hr = put_ForeColor(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetHidden ( ) {
       long _result = 0;
       HRESULT _hr = get_Hidden(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutHidden ( long pValue ) {
       HRESULT _hr = put_Hidden(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetEngrave ( ) {
       long _result = 0;
       HRESULT _hr = get_Engrave(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutEngrave ( long pValue ) {
       HRESULT _hr = put_Engrave(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetItalic ( ) {
       long _result = 0;
       HRESULT _hr = get_Italic(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutItalic ( long pValue ) {
       HRESULT _hr = put_Italic(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextFont::GetKerning ( ) {
       float _result = 0;
       HRESULT _hr = get_Kerning(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutKerning ( float pValue ) {
       HRESULT _hr = put_Kerning(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetLanguageID ( ) {
       long _result = 0;
       HRESULT _hr = get_LanguageID(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutLanguageID ( long pValue ) {
       HRESULT _hr = put_LanguageID(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   _bstr_t ITextFont::GetName ( ) {
       BSTR _result = 0;
       HRESULT _hr = get_Name(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _bstr_t(_result, false);
   }

   void ITextFont::PutName ( _bstr_t pbstr ) {
       HRESULT _hr = put_Name(pbstr);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetOutline ( ) {
       long _result = 0;
       HRESULT _hr = get_Outline(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutOutline ( long pValue ) {
       HRESULT _hr = put_Outline(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextFont::GetPosition ( ) {
       float _result = 0;
       HRESULT _hr = get_Position(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutPosition ( float pValue ) {
       HRESULT _hr = put_Position(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetProtected ( ) {
       long _result = 0;
       HRESULT _hr = get_Protected(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutProtected ( long pValue ) {
       HRESULT _hr = put_Protected(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetShadow ( ) {
       long _result = 0;
       HRESULT _hr = get_Shadow(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutShadow ( long pValue ) {
       HRESULT _hr = put_Shadow(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextFont::GetSize ( ) {
       float _result = 0;
       HRESULT _hr = get_Size(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutSize ( float pValue ) {
       HRESULT _hr = put_Size(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetSmallCaps ( ) {
       long _result = 0;
       HRESULT _hr = get_SmallCaps(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutSmallCaps ( long pValue ) {
       HRESULT _hr = put_SmallCaps(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextFont::GetSpacing ( ) {
       float _result = 0;
       HRESULT _hr = get_Spacing(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutSpacing ( float pValue ) {
       HRESULT _hr = put_Spacing(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetStrikeThrough ( ) {
       long _result = 0;
       HRESULT _hr = get_StrikeThrough(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutStrikeThrough ( long pValue ) {
       HRESULT _hr = put_StrikeThrough(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetSubscript ( ) {
       long _result = 0;
       HRESULT _hr = get_Subscript(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutSubscript ( long pValue ) {
       HRESULT _hr = put_Subscript(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetSuperscript ( ) {
       long _result = 0;
       HRESULT _hr = get_Superscript(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutSuperscript ( long pValue ) {
       HRESULT _hr = put_Superscript(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetUnderline ( ) {
       long _result = 0;
       HRESULT _hr = get_Underline(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutUnderline ( long pValue ) {
       HRESULT _hr = put_Underline(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextFont::GetWeight ( ) {
       long _result = 0;
       HRESULT _hr = get_Weight(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextFont::PutWeight ( long pValue ) {
       HRESULT _hr = put_Weight(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   //
   // interface ITextPara wrapper method implementations
   //

   ITextParaPtr ITextPara::GetDuplicate ( ) {
       struct ITextPara * _result = 0;
       HRESULT _hr = get_Duplicate(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextParaPtr(_result, false);
   }

   void ITextPara::PutDuplicate ( struct ITextPara * ppPara ) {
       HRESULT _hr = put_Duplicate(ppPara);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::CanChange ( ) {
       long _result = 0;
       HRESULT _hr = raw_CanChange(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextPara::IsEqual ( struct ITextPara * pPara ) {
       long _result = 0;
       HRESULT _hr = raw_IsEqual(pPara, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextPara::Reset ( long Value ) {
       HRESULT _hr = raw_Reset(Value);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextPara::GetStyle ( ) {
       long _result = 0;
       HRESULT _hr = get_Style(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutStyle ( long pValue ) {
       HRESULT _hr = put_Style(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetAlignment ( ) {
       long _result = 0;
       HRESULT _hr = get_Alignment(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutAlignment ( long pValue ) {
       HRESULT _hr = put_Alignment(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetHyphenation ( ) {
       long _result = 0;
       HRESULT _hr = get_Hyphenation(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutHyphenation ( long pValue ) {
       HRESULT _hr = put_Hyphenation(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextPara::GetFirstLineIndent ( ) {
       float _result = 0;
       HRESULT _hr = get_FirstLineIndent(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextPara::GetKeepTogether ( ) {
       long _result = 0;
       HRESULT _hr = get_KeepTogether(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutKeepTogether ( long pValue ) {
       HRESULT _hr = put_KeepTogether(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetKeepWithNext ( ) {
       long _result = 0;
       HRESULT _hr = get_KeepWithNext(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutKeepWithNext ( long pValue ) {
       HRESULT _hr = put_KeepWithNext(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextPara::GetLeftIndent ( ) {
       float _result = 0;
       HRESULT _hr = get_LeftIndent(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   float ITextPara::GetLineSpacing ( ) {
       float _result = 0;
       HRESULT _hr = get_LineSpacing(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextPara::GetLineSpacingRule ( ) {
       long _result = 0;
       HRESULT _hr = get_LineSpacingRule(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextPara::GetListAlignment ( ) {
       long _result = 0;
       HRESULT _hr = get_ListAlignment(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutListAlignment ( long pValue ) {
       HRESULT _hr = put_ListAlignment(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetListLevelIndex ( ) {
       long _result = 0;
       HRESULT _hr = get_ListLevelIndex(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutListLevelIndex ( long pValue ) {
       HRESULT _hr = put_ListLevelIndex(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetListStart ( ) {
       long _result = 0;
       HRESULT _hr = get_ListStart(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutListStart ( long pValue ) {
       HRESULT _hr = put_ListStart(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextPara::GetListTab ( ) {
       float _result = 0;
       HRESULT _hr = get_ListTab(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutListTab ( float pValue ) {
       HRESULT _hr = put_ListTab(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetListType ( ) {
       long _result = 0;
       HRESULT _hr = get_ListType(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutListType ( long pValue ) {
       HRESULT _hr = put_ListType(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetNoLineNumber ( ) {
       long _result = 0;
       HRESULT _hr = get_NoLineNumber(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutNoLineNumber ( long pValue ) {
       HRESULT _hr = put_NoLineNumber(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetPageBreakBefore ( ) {
       long _result = 0;
       HRESULT _hr = get_PageBreakBefore(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutPageBreakBefore ( long pValue ) {
       HRESULT _hr = put_PageBreakBefore(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextPara::GetRightIndent ( ) {
       float _result = 0;
       HRESULT _hr = get_RightIndent(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutRightIndent ( float pValue ) {
       HRESULT _hr = put_RightIndent(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   HRESULT ITextPara::SetIndents ( float StartIndent, float LeftIndent, float RightIndent ) {
       HRESULT _hr = raw_SetIndents(StartIndent, LeftIndent, RightIndent);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextPara::SetLineSpacing ( long LineSpacingRule, float LineSpacing ) {
       HRESULT _hr = raw_SetLineSpacing(LineSpacingRule, LineSpacing);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   float ITextPara::GetSpaceAfter ( ) {
       float _result = 0;
       HRESULT _hr = get_SpaceAfter(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutSpaceAfter ( float pValue ) {
       HRESULT _hr = put_SpaceAfter(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextPara::GetSpaceBefore ( ) {
       float _result = 0;
       HRESULT _hr = get_SpaceBefore(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutSpaceBefore ( float pValue ) {
       HRESULT _hr = put_SpaceBefore(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetWidowControl ( ) {
       long _result = 0;
       HRESULT _hr = get_WidowControl(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextPara::PutWidowControl ( long pValue ) {
       HRESULT _hr = put_WidowControl(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextPara::GetTabCount ( ) {
       long _result = 0;
       HRESULT _hr = get_TabCount(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextPara::AddTab ( float tbPos, long tbAlign, long tbLeader ) {
       HRESULT _hr = raw_AddTab(tbPos, tbAlign, tbLeader);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextPara::ClearAllTabs ( ) {
       HRESULT _hr = raw_ClearAllTabs();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextPara::DeleteTab ( float tbPos ) {
       HRESULT _hr = raw_DeleteTab(tbPos);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextPara::GetTab ( long iTab, float * ptbPos, long * ptbAlign, long * ptbLeader ) {
       HRESULT _hr = raw_GetTab(iTab, ptbPos, ptbAlign, ptbLeader);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   //
   // interface ITextRange wrapper method implementations
   //

   _bstr_t ITextRange::GetText ( ) {
       BSTR _result = 0;
       HRESULT _hr = get_Text(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _bstr_t(_result, false);
   }

   void ITextRange::PutText ( _bstr_t pbstr ) {
       HRESULT _hr = put_Text(pbstr);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextRange::GetChar ( ) {
       long _result = 0;
       HRESULT _hr = get_Char(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextRange::PutChar ( long pch ) {
       HRESULT _hr = put_Char(pch);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   ITextRangePtr ITextRange::GetDuplicate ( ) {
       struct ITextRange * _result = 0;
       HRESULT _hr = get_Duplicate(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextRangePtr(_result, false);
   }

   ITextRangePtr ITextRange::GetFormattedText ( ) {
       struct ITextRange * _result = 0;
       HRESULT _hr = get_FormattedText(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextRangePtr(_result, false);
   }

   void ITextRange::PutFormattedText ( struct ITextRange * ppRange ) {
       HRESULT _hr = put_FormattedText(ppRange);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextRange::GetStart ( ) {
       long _result = 0;
       HRESULT _hr = get_Start(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextRange::PutStart ( long pcpFirst ) {
       HRESULT _hr = put_Start(pcpFirst);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextRange::GetEnd ( ) {
       long _result = 0;
       HRESULT _hr = get_End(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextRange::PutEnd ( long pcpLim ) {
       HRESULT _hr = put_End(pcpLim);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   ITextFontPtr ITextRange::GetFont ( ) {
       struct ITextFont * _result = 0;
       HRESULT _hr = get_Font(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextFontPtr(_result, false);
   }

   void ITextRange::PutFont ( struct ITextFont * pFont ) {
       HRESULT _hr = put_Font(pFont);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   ITextParaPtr ITextRange::GetPara ( ) {
       struct ITextPara * _result = 0;
       HRESULT _hr = get_Para(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextParaPtr(_result, false);
   }

   void ITextRange::PutPara ( struct ITextPara * pPara ) {
       HRESULT _hr = put_Para(pPara);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextRange::GetStoryLength ( ) {
       long _result = 0;
       HRESULT _hr = get_StoryLength(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::GetStoryType ( ) {
       long _result = 0;
       HRESULT _hr = get_StoryType(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextRange::Collapse ( long bStart ) {
       HRESULT _hr = raw_Collapse(bStart);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextRange::Expand ( long Unit ) {
       long _result = 0;
       HRESULT _hr = raw_Expand(Unit, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::GetIndex ( long Unit ) {
       long _result = 0;
       HRESULT _hr = raw_GetIndex(Unit, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextRange::SetIndex ( long Unit, long Index, long Extend ) {
       HRESULT _hr = raw_SetIndex(Unit, Index, Extend);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextRange::SetRange ( long cpActive, long cpOther ) {
       HRESULT _hr = raw_SetRange(cpActive, cpOther);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextRange::InRange ( struct ITextRange * pRange ) {
       long _result = 0;
       HRESULT _hr = raw_InRange(pRange, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::InStory ( struct ITextRange * pRange ) {
       long _result = 0;
       HRESULT _hr = raw_InStory(pRange, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::IsEqual ( struct ITextRange * pRange ) {
       long _result = 0;
       HRESULT _hr = raw_IsEqual(pRange, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextRange::Select ( ) {
       HRESULT _hr = raw_Select();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextRange::StartOf ( long Unit, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_StartOf(Unit, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::EndOf ( long Unit, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_EndOf(Unit, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::Move ( long Unit, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_Move(Unit, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveStart ( long Unit, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveStart(Unit, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveEnd ( long Unit, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveEnd(Unit, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveWhile ( VARIANT * Cset, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveWhile(Cset, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveStartWhile ( VARIANT * Cset, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveStartWhile(Cset, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveEndWhile ( VARIANT * Cset, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveEndWhile(Cset, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveUntil ( VARIANT * Cset, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveUntil(Cset, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveStartUntil ( VARIANT * Cset, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveStartUntil(Cset, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::MoveEndUntil ( VARIANT * Cset, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_MoveEndUntil(Cset, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::FindText ( _bstr_t bstr, long cch, long Flags ) {
       long _result = 0;
       HRESULT _hr = raw_FindText(bstr, cch, Flags, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::FindTextStart ( _bstr_t bstr, long cch, long Flags ) {
       long _result = 0;
       HRESULT _hr = raw_FindTextStart(bstr, cch, Flags, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::FindTextEnd ( _bstr_t bstr, long cch, long Flags ) {
       long _result = 0;
       HRESULT _hr = raw_FindTextEnd(bstr, cch, Flags, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::Delete ( long Unit, long Count ) {
       long _result = 0;
       HRESULT _hr = raw_Delete(Unit, Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextRange::Cut ( VARIANT * pVar ) {
       HRESULT _hr = raw_Cut(pVar);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextRange::Copy ( VARIANT * pVar ) {
       HRESULT _hr = raw_Copy(pVar);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextRange::Paste ( VARIANT * pVar, long Format ) {
       HRESULT _hr = raw_Paste(pVar, Format);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextRange::CanPaste ( VARIANT * pVar, long Format ) {
       long _result = 0;
       HRESULT _hr = raw_CanPaste(pVar, Format, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextRange::CanEdit ( ) {
       long _result = 0;
       HRESULT _hr = raw_CanEdit(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextRange::ChangeCase ( long Type ) {
       HRESULT _hr = raw_ChangeCase(Type);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextRange::GetPoint ( long Type, long * px, long * py ) {
       HRESULT _hr = raw_GetPoint(Type, px, py);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextRange::SetPoint ( long x, long y, long Type, long Extend ) {
       HRESULT _hr = raw_SetPoint(x, y, Type, Extend);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextRange::ScrollIntoView ( long Value ) {
       HRESULT _hr = raw_ScrollIntoView(Value);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   IUnknownPtr ITextRange::GetEmbeddedObject ( ) {
       IUnknown * _result = 0;
       HRESULT _hr = raw_GetEmbeddedObject(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return IUnknownPtr(_result, false);
   }

   //
   // interface ITextSelection wrapper method implementations
   //

   long ITextSelection::GetFlags ( ) {
       long _result = 0;
       HRESULT _hr = get_Flags(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextSelection::PutFlags ( long pFlags ) {
       HRESULT _hr = put_Flags(pFlags);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextSelection::GetType ( ) {
       long _result = 0;
       HRESULT _hr = get_Type(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextSelection::MoveLeft ( long Unit, long Count, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_MoveLeft(Unit, Count, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextSelection::MoveRight ( long Unit, long Count, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_MoveRight(Unit, Count, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextSelection::MoveUp ( long Unit, long Count, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_MoveUp(Unit, Count, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextSelection::MoveDown ( long Unit, long Count, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_MoveDown(Unit, Count, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextSelection::HomeKey ( long Unit, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_HomeKey(Unit, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextSelection::EndKey ( long Unit, long Extend ) {
       long _result = 0;
       HRESULT _hr = raw_EndKey(Unit, Extend, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextSelection::TypeText ( _bstr_t bstr ) {
       HRESULT _hr = raw_TypeText(bstr);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   //
   // interface ITextStoryRanges wrapper method implementations
   //

   IUnknownPtr ITextStoryRanges::_NewEnum ( ) {
       IUnknown * _result = 0;
       HRESULT _hr = raw__NewEnum(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return IUnknownPtr(_result, false);
   }

   ITextRangePtr ITextStoryRanges::Item ( long Index ) {
       struct ITextRange * _result = 0;
       HRESULT _hr = raw_Item(Index, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextRangePtr(_result, false);
   }

   long ITextStoryRanges::GetCount ( ) {
       long _result = 0;
       HRESULT _hr = get_Count(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   //
   // interface ITextDocument wrapper method implementations
   //

   _bstr_t ITextDocument::GetName ( ) {
       BSTR _result = 0;
       HRESULT _hr = get_Name(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _bstr_t(_result, false);
   }

   ITextSelectionPtr ITextDocument::GetSelection ( ) {
       struct ITextSelection * _result = 0;
       HRESULT _hr = get_Selection(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextSelectionPtr(_result, false);
   }

   long ITextDocument::GetStoryCount ( ) {
       long _result = 0;
       HRESULT _hr = get_StoryCount(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   ITextStoryRangesPtr ITextDocument::GetStoryRanges ( ) {
       struct ITextStoryRanges * _result = 0;
       HRESULT _hr = get_StoryRanges(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextStoryRangesPtr(_result, false);
   }

   long ITextDocument::GetSaved ( ) {
       long _result = 0;
       HRESULT _hr = get_Saved(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextDocument::PutSaved ( long pValue ) {
       HRESULT _hr = put_Saved(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   float ITextDocument::GetDefaultTabStop ( ) {
       float _result = 0;
       HRESULT _hr = get_DefaultTabStop(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextDocument::PutDefaultTabStop ( float pValue ) {
       HRESULT _hr = put_DefaultTabStop(pValue);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   HRESULT ITextDocument::New ( ) {
       HRESULT _hr = raw_New();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument::Open ( VARIANT * pVar, long Flags, long CodePage ) {
       HRESULT _hr = raw_Open(pVar, Flags, CodePage);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument::Save ( VARIANT * pVar, long Flags, long CodePage ) {
       HRESULT _hr = raw_Save(pVar, Flags, CodePage);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextDocument::Freeze ( ) {
       long _result = 0;
       HRESULT _hr = raw_Freeze(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextDocument::Unfreeze ( ) {
       long _result = 0;
       HRESULT _hr = raw_Unfreeze(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextDocument::BeginEditCollection ( ) {
       HRESULT _hr = raw_BeginEditCollection();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument::EndEditCollection ( ) {
       HRESULT _hr = raw_EndEditCollection();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextDocument::Undo ( long Count ) {
       long _result = 0;
       HRESULT _hr = raw_Undo(Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   long ITextDocument::Redo ( long Count ) {
       long _result = 0;
       HRESULT _hr = raw_Redo(Count, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   ITextRangePtr ITextDocument::Range ( long cp1, long cp2 ) {
       struct ITextRange * _result = 0;
       HRESULT _hr = raw_Range(cp1, cp2, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextRangePtr(_result, false);
   }

   ITextRangePtr ITextDocument::RangeFromPoint ( long x, long y ) {
       struct ITextRange * _result = 0;
       HRESULT _hr = raw_RangeFromPoint(x, y, &_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextRangePtr(_result, false);
   }

   //
   // interface ITextDocument2 wrapper method implementations
   //

   HRESULT ITextDocument2::AttachMsgFilter ( IUnknown * pFilter ) {
       HRESULT _hr = raw_AttachMsgFilter(pFilter);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::SetEffectColor ( long Index, unsigned long cr ) {
       HRESULT _hr = raw_SetEffectColor(Index, cr);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::GetEffectColor ( long Index, unsigned long * pcr ) {
       HRESULT _hr = raw_GetEffectColor(Index, pcr);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextDocument2::GetCaretType ( ) {
       long _result = 0;
       HRESULT _hr = get_CaretType(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextDocument2::PutCaretType ( long pCaretType ) {
       HRESULT _hr = put_CaretType(pCaretType);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   long ITextDocument2::GetImmContext ( ) {
       long _result = 0;
       HRESULT _hr = raw_GetImmContext(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   HRESULT ITextDocument2::ReleaseImmContext ( long Context ) {
       HRESULT _hr = raw_ReleaseImmContext(Context);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::GetPreferredFont ( long cp, long CodePage, long Option, long curCodepage, long curFontSize, BSTR * pbstr, long * pPitchAndFamily, long * pNewFontSize ) {
       HRESULT _hr = raw_GetPreferredFont(cp, CodePage, Option, curCodepage, curFontSize, pbstr, pPitchAndFamily, pNewFontSize);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   long ITextDocument2::GetNotificationMode ( ) {
       long _result = 0;
       HRESULT _hr = get_NotificationMode(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _result;
   }

   void ITextDocument2::PutNotificationMode ( long pMode ) {
       HRESULT _hr = put_NotificationMode(pMode);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
   }

   HRESULT ITextDocument2::GetClientRect ( long Type, long * pLeft, long * pTop, long * pRight, long * pBottom ) {
       HRESULT _hr = raw_GetClientRect(Type, pLeft, pTop, pRight, pBottom);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   ITextSelectionPtr ITextDocument2::GetSelectionEx ( ) {
       struct ITextSelection * _result = 0;
       HRESULT _hr = get_SelectionEx(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextSelectionPtr(_result, false);
   }

   HRESULT ITextDocument2::GetWindow ( long * phWnd ) {
       HRESULT _hr = raw_GetWindow(phWnd);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::GetFEFlags ( long * pFlags ) {
       HRESULT _hr = raw_GetFEFlags(pFlags);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::UpdateWindow ( ) {
       HRESULT _hr = raw_UpdateWindow();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::CheckTextLimit ( long cch, long * pcch ) {
       HRESULT _hr = raw_CheckTextLimit(cch, pcch);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::IMEInProgress ( long Mode ) {
       HRESULT _hr = raw_IMEInProgress(Mode);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::SysBeep ( ) {
       HRESULT _hr = raw_SysBeep();
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::Update ( long Mode ) {
       HRESULT _hr = raw_Update(Mode);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextDocument2::Notify ( long Notify ) {
       HRESULT _hr = raw_Notify(Notify);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   ITextFontPtr ITextDocument2::GetDocumentFont ( ) {
       struct ITextFont * _result = 0;
       HRESULT _hr = raw_GetDocumentFont(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextFontPtr(_result, false);
   }

   ITextParaPtr ITextDocument2::GetDocumentPara ( ) {
       struct ITextPara * _result = 0;
       HRESULT _hr = raw_GetDocumentPara(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return ITextParaPtr(_result, false);
   }

   IUnknownPtr ITextDocument2::GetCallManager ( ) {
       IUnknown * _result = 0;
       HRESULT _hr = raw_GetCallManager(&_result);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return IUnknownPtr(_result, false);
   }

   HRESULT ITextDocument2::ReleaseCallManager ( IUnknown * pVoid ) {
       HRESULT _hr = raw_ReleaseCallManager(pVoid);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   //
   // interface ITextMsgFilter wrapper method implementations
   //

   HRESULT ITextMsgFilter::AttachDocument ( wireHWND hwnd, struct ITextDocument2 * pTextDoc, IUnknown * punk ) {
       HRESULT _hr = raw_AttachDocument(hwnd, pTextDoc, punk);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextMsgFilter::HandleMessage ( unsigned int * pmsg, UINT_PTR * pwparam, LONG_PTR * plparam, LONG_PTR * plres ) {
       HRESULT _hr = raw_HandleMessage(pmsg, pwparam, plparam, plres);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

   HRESULT ITextMsgFilter::AttachMsgFilter ( struct ITextMsgFilter * pMsgFilter ) {
       HRESULT _hr = raw_AttachMsgFilter(pMsgFilter);
       if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
       return _hr;
   }

}