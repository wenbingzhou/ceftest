// CEFFrame.h : CEFFrame DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCEFFrameApp
// �йش���ʵ�ֵ���Ϣ������� CEFFrame.cpp
//

class CCEFFrameApp : public CWinApp
{
public:
	CCEFFrameApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
