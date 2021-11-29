#pragma once
#include "stdafx.h"
#include "simple_handler.h"
//#include <future>
#include "stdio.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <set>
#include "comm.h"
#include "cef_js_bridge.h"

using namespace std;

typedef std::function<void(CefV8ValueList& V8ValueList)> CallJsFunctionV8;
typedef std::map<CefString/* function_name*/, CallJsFunctionV8/* function*/> V8RegisteredFunction;
typedef std::function<void(const std::string& params)> CallJsFunctionJs;
typedef std::map<CefString/* function_name*/, CallJsFunctionJs/* function*/> JsRegisteredFunction;

// CCEFFrameDialog 对话框

//递归终止函数
template<class F>
void expand(F& f)
{
}
template<class F, class T ,class... Args>void expand(F& f, T first, Args&&...args)
{
	f(first);
	expand(f, args...);
}

struct OutPut1
{
	OutPut1(std::stringstream &sstr):_sstr(sstr) {}

	template<class T>
	void operator()(T val)
	{
		print(val);
	}
	template<class T= std::string>
	void print(std::string val)
	{
		_sstr << "'" << val << "',";
	}
	template<class T>
	void print(T val)
	{
		_sstr << val<<",";
	}
	std::stringstream &_sstr;
};

template <class T, class R, typename... Args>
class  MyDelegate
{
public:
	MyDelegate(T* t, R(T::*f)(Args...)) :m_t(t), m_f(f) {}

	R operator()(Args&&... args)
	{
		return (m_t->*m_f)(std::forward<Args>(args) ...);
	}

private:
	T* m_t;
	R(T::*m_f)(Args...);
};
template <class T, class R, typename... Args>
MyDelegate<T, R, Args...>* CreateDelegate(T* t, R(T::*f)(Args...))
{
	return new MyDelegate<T, R, Args...>(t, f);
}

class EXPORT_API CCEFFrameDialog : public CDialogEx, public virtual mynbase::SupportWeakCallback
{
	DECLARE_DYNAMIC(CCEFFrameDialog)

public:
	CCEFFrameDialog(UINT Itemp,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCEFFrameDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CEFFRAMEDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CefRefPtr<SimpleHandler> m_simpleClient;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetUrl(std::string url) { m_url = url; }
	bool Navigate(CString url);

	//char* strFuntionName方法名， "abcd", 5.6, 6
	template<class... Args>
	void CCEFFrameDialog::executeJS(char* strFuntionName, Args&&... args) {
		if (!strFuntionName)
			return;
		std::stringstream stream;
		stream << strFuntionName << "(";
		expand(OutPut1(stream), args...);
		std::string strCode = stream.str(); 
		strCode = strCode.substr(0, strCode.length() - 1);
		strCode += ")";

		if (m_simpleClient.get() != NULL) {
			CefRefPtr<CefBrowser> browser = m_simpleClient->getBrowser();
			if (browser)
			{
				CefRefPtr<CefFrame> frame = browser->GetMainFrame();
				if (frame)
					frame->ExecuteJavaScript(CefString(strCode), frame->GetURL(), 0);
			}
		}
	}

	// js调用c++
	//V8RegisteredFunction m_pV8RegisteredFunction_;
	//bool addJsFuntion(std::string strFunName, CallJsFunctionV8 function)
	//{
	//	m_pV8RegisteredFunction_.emplace(strFunName.c_str(), function);
	//	return true;
	//}

	/**
	* @brief 注册一个 C++ 方法提供前端调用
	* @param[in] strFunName 方法名称
	* @param[in] function 方法函数体
	* @return 返回 true 表示注册成功，false 可能已经注册
	*/
	JsRegisteredFunction m_pJsRegisteredFunction_;
	bool addJsFuntion(std::string strFunName, CallJsFunctionJs function)
	{
		m_pJsRegisteredFunction_.emplace(strFunName.c_str(), function);
		return true;
	}

	//typedef std::function<void(void)> CallJsFunctionT;
	//typedef std::map<CefString/* function_name*/, CallJsFunctionT/* function*/> RegisteredFunctionT;
	//RegisteredFunctionT m_pRegisteredFunctionT_;
	//std::map<CefString/* function_name*/, int/* function addr*/> m_pJsFuntion;
	//template <class T, class R, typename... Args>
	//void addJsFuntion(T* t,std::string strFunName, R(T::*f)(Args...))
	//{
	//	auto func = CreateDelegate(t, f); //创建委托
	//	//int a1 = 1;
	//	//(*func)(&a1);
	//	int p1 = (int)func;
	//	m_pJsFuntion[strFunName] = p1;
	//	//___CALL(p1,&a1);
	//}

	std::shared_ptr<my_cef::CefJSBridge>	js_bridge_;
	/**
	* @brief 注册一个 C++ 方法提供前端调用
	* @param[in] function_name 方法名称
	* @param[in] function 方法函数体
	* @param[in] global_function 是否是一个全局方法
	* @return 返回 true 表示注册成功，false 可能已经注册
	*/
	bool RegisterCppFunc(const std::string& function_name, my_cef::CppFunction function, bool global_function = false);
private:
	//std::string url = "http://cam.pcbpp.com:8088/ipcb/sight/getSightDataToStandardTemplate?BOID=6bc7b4236c0e476d9dbc26cdf3009ae6&softcode=lianhe";// "http://www.baidu.com";
	std::string m_url = "E:\\home\\libcef\\MFCApplication1\\HtmlPage.html";//"www.huya.com";
};
