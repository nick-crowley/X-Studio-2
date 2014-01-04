
// MFC Test 1View.h : interface of the CMFCTest1View class
//

#pragma once

#include "resource.h"


class CMFCTest1View : public CFormView
{
protected: // create from serialization only
	CMFCTest1View();
	DECLARE_DYNCREATE(CMFCTest1View)

public:
	enum{ IDD = IDD_MFCTEST1_FORM };

// Attributes
public:
	CMFCTest1Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CMFCTest1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MFC Test 1View.cpp
inline CMFCTest1Doc* CMFCTest1View::GetDocument() const
   { return reinterpret_cast<CMFCTest1Doc*>(m_pDocument); }
#endif

