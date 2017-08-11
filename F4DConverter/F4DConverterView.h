
// F4DConverterView.h : CF4DConverterView Ŭ������ �������̽�
//

#pragma once


class CF4DConverterView : public CView
{
protected: // serialization������ ��������ϴ�.
	CF4DConverterView();
	DECLARE_DYNCREATE(CF4DConverterView)

// Ư���Դϴ�.
public:
	CF4DConverterDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	void activateConverter();

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CF4DConverterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // F4DConverterView.cpp�� ����� ����
inline CF4DConverterDoc* CF4DConverterView::GetDocument() const
   { return reinterpret_cast<CF4DConverterDoc*>(m_pDocument); }
#endif

