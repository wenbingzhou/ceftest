// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "stdafx.h"
#include <string>
#include "cefsimple/simple_handler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "simple_app.h"
#include "SimpleV8Handler.h"

SimpleApp::SimpleApp() {
}

void SimpleApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

//  // Information used when creating the native window.
//  CefWindowInfo window_info;
//
//#if defined(OS_WIN)
//  // On Windows we need to specify certain flags that will be passed to
//  // CreateWindowEx().
//  window_info.SetAsPopup(NULL, "cefsimple");
//#endif
//
//  // SimpleHandler implements browser-level callbacks.
//  CefRefPtr<SimpleHandler> handler(new SimpleHandler());
//
//  // Specify CEF browser settings here.
//  CefBrowserSettings browser_settings;
//
//  std::string url;
//
//  // Check if a "--url=" value was provided via the command-line. If so, use
//  // that instead of the default URL.
//  CefRefPtr<CefCommandLine> command_line =
//      CefCommandLine::GetGlobalCommandLine();
//  url = command_line->GetSwitchValue("url");
//  if (url.empty())
//    url = "http://www.google.com";
//
//  // Create the first browser window.
//  CefBrowserHost::CreateBrowser(window_info, handler.get(), url,
//                                browser_settings, NULL);
}

void SimpleApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
	/*throw std::logic_error("The method or operation is not implemented.");*/
	//�鿴����������� http://peter.sh/experiments/chromium-command-line-switches/#disable-gpu-process-prelaunch

	// cef3 ��Lantern ��ϵͳ����ʱ.pac �ļ�ʱ�����
	// �Ѿ�ȷ���Ǵ˰汾Cef��bug
	// Ŀǰ�ȼ���˲��������ߴ�����ȥִ��.pac�ļ���
	command_line->AppendSwitch("no-proxy-server");

	//����GPU����
	command_line->AppendSwitch("--disable-gpu");

	//�������е�ҳ�湲��һ�����̣��Խ�ʡ������
	command_line->AppendSwitchWithValue("--renderer-process-limit", "1");
}

void SimpleApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> pV8 = context->GetGlobal();

	CefRefPtr<CSimpleV8Handler> pJsHandler(new CSimpleV8Handler());

	CefRefPtr<CefV8Value> pFunc = CefV8Value::CreateFunction(CefString("ShowJsInfo"), pJsHandler);
	pV8->SetValue(CefString("ShowJsInfo"), pFunc, V8_PROPERTY_ATTRIBUTE_NONE);

	pV8->SetValue(CefString("external"), CefV8Value::CreateFunction(CefString("external"), pJsHandler), V8_PROPERTY_ATTRIBUTE_NONE);

	for (std::map<CefString, CallJsFunctionJs>::iterator it = m_pCCEFFrameDialog->m_pJsRegisteredFunction_.begin();
		it != m_pCCEFFrameDialog->m_pJsRegisteredFunction_.end(); ++it)
	{
		CefString strFunc = it->first;
		CefRefPtr<CefV8Value> external = CefV8Value::CreateObject(NULL);
		external->SetValue(strFunc, CefV8Value::CreateFunction(strFunc, pJsHandler), V8_PROPERTY_ATTRIBUTE_NONE);
		pV8->SetValue("external", external, V8_PROPERTY_ATTRIBUTE_NONE);
		//pV8->SetValue(strFunc, CefV8Value::CreateFunction(strFunc, pJsHandler), V8_PROPERTY_ATTRIBUTE_NONE);
	}

	//////////////////////////////ע��js�ص���ONJSBindIng��������һ����register������////////////////////////////////////////////
	pV8->SetValue("register", CefV8Value::CreateFunction("register", pJsHandler), V8_PROPERTY_ATTRIBUTE_NONE);
	if (!m_pCCEFFrameDialog->js_bridge_.get())
		m_pCCEFFrameDialog->js_bridge_.reset(new my_cef::CefJSBridge);
	pJsHandler->AttachJSBridge(m_pCCEFFrameDialog->js_bridge_);
}

void SimpleApp::OnWebKitInitialized()
{

	//CEF_REQUIRE_RENDERER_THREAD();

	// Define the extension contents.
	//std::string extensionCode =
	//	"var test;"
	//	"if (!test)"
	//	"  test = {};"
	//	"(function() {"
	//	"  test.addd = function(num1, num2) {"
	//	"    native function addd(num1, num2);"
	//	"    return addd(num1, num2);"
	//	"  };"
	//	"})();";
	/**
	* JavaScript ��չ���룬���ﶨ��һ�� NimCefWebFunction �����ṩ call �������� Web �˴��� native �� CefV8Handler �������
	* param[in] functionName Ҫ���õ� C++ ��������
	* param[in] params ���ø÷������ݵĲ�������ǰ��ָ������һ�� Object����ת�� Native ��ʱ��תΪ���ַ���
	* param[in] callback ִ�и÷�����Ļص�����
	* ǰ�˵���ʾ��
	* CefWebInstance.ShowJsInfo('showMessage', { message: 'Hello C++' }, (arguments) => {
	*    console.log(arguments)
	* })
	*/
	std::string extensionCode = R"(
		var NimCefWebInstance = {};
		(() => {
			NimCefWebInstance.call = (functionName, arg1, arg2) => {
				if (typeof arg1 === 'function') {
					native function call(functionName, arg1);
					return call(functionName, arg1);
				} else {
					const jsonString = JSON.stringify(arg1);
					native function call(functionName, jsonString, arg2);
					return call(functionName, jsonString, arg2);
				}
			};
			NimCefWebInstance.register = (functionName, callback) => {
				native function register(functionName, callback);
				return register(functionName, callback);
			};
		})();
	)";

	// Create an instance of my CefV8Handler object.
	CefRefPtr<CSimpleV8Handler> handler = new CSimpleV8Handler();

	if (!m_pCCEFFrameDialog->js_bridge_.get())
		m_pCCEFFrameDialog->js_bridge_.reset(new my_cef::CefJSBridge);
	handler->AttachJSBridge(m_pCCEFFrameDialog->js_bridge_);
	// Register the extension.
	CefRegisterExtension("v8/test", extensionCode, handler);
}
