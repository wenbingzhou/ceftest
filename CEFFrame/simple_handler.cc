// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include "stdafx.h"
#include "simple_handler.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "CEFFrameDialog.h"
#include "ipc_string_define.h"

namespace {

SimpleHandler* g_instance = NULL;

}  // namespace

SimpleHandler::SimpleHandler()
    : is_closing_(false) {
  DCHECK(!g_instance);
  g_instance = this;
}

SimpleHandler::~SimpleHandler() {
  g_instance = NULL;
}

// static
SimpleHandler* SimpleHandler::GetInstance() {
  return g_instance;
}


void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{

}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Add to the list of existing browsers.
  m_cefBrowser1 = browser;
  browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Remove from the list of existing browsers.
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }

  if (browser_list_.empty()) {
    // All browser windows have closed. Quit the application message loop.
    CefQuitMessageLoop();
  }
}

void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;

  // Display a load error message.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
  frame->LoadString(ss.str(), failedUrl);
}

void SimpleHandler::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&SimpleHandler::CloseAllBrowsers, this, force_close));
    return;
  }

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}

bool SimpleHandler::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message)
{
	// 收到 browser 的消息回复
	const CefString & message_name = message->GetName();
	CefRefPtr<CefProcessMessage> msg = message->Copy();

	if (message_name == my_cef::kFocusedNodeChangedMessage)
	{
		bool is_focus_oneditable_field_ = message->GetArgumentList()->GetBool(0);
		return true;
	}
	else if (message_name == my_cef::kCallCppFunctionMessage)
	{
		CefString fun_name = message->GetArgumentList()->GetString(0);
		CefString param = message->GetArgumentList()->GetString(1);
		int js_callback_id = message->GetArgumentList()->GetInt(2);

		if (m_pCCEFFrameDialog->js_bridge_)
			m_pCCEFFrameDialog->js_bridge_->ExecuteCppFunc(fun_name, param, js_callback_id, browser);

		return true;
	}
	else if (message_name == my_cef::kExecuteCppCallbackMessage)
	{
		CefString param = message->GetArgumentList()->GetString(0);
		int callback_id = message->GetArgumentList()->GetInt(1);

		if (m_pCCEFFrameDialog->js_bridge_)
			m_pCCEFFrameDialog->js_bridge_->ExecuteCppCallbackFunc(callback_id, param);
	}
	else
	{
		//std::map<CefString/* function_name*/, int/* function addr*/>::const_iterator it = m_pCCEFFrameDialog->m_pJsFuntion.find(message_name);
		//if(it != m_pCCEFFrameDialog->m_pJsFuntion.end())
		//{
		//	if (msg->GetArgumentList().get()->GetSize() == 1)
		//	{
		//		//int aa = message->GetArgumentList()->GetInt(0);
		//		//(AFX_PMSG)(void (CCEFFrameDialog::*)(void))&it->second;
		//		m_pCCEFFrameDialog->___CALL(it->second, message->GetArgumentList());
		//	}
		//	return true;
		//}
		std::map<CefString, CallJsFunctionJs>::iterator it2 = m_pCCEFFrameDialog->m_pJsRegisteredFunction_.find(message_name);
		if (it2 != m_pCCEFFrameDialog->m_pJsRegisteredFunction_.end())
		{
			if (msg->GetArgumentList().get()->GetSize() == 1)
			{
				string str = message->GetArgumentList()->GetString(0);
				it2->second(str);
				return true;
			}
			return false;
		}
	}
	return false;
}
