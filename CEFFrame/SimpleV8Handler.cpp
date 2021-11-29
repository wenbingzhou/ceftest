#include "stdafx.h"
#include "SimpleV8Handler.h"
#include<iostream>
#include "singleton.h"
using namespace std;


CSimpleV8Handler::CSimpleV8Handler()
{
}


CSimpleV8Handler::~CSimpleV8Handler()
{
}

//void testFun(CefRefPtr<CefListValue> args)
//{
//
//}
bool CSimpleV8Handler::Execute(const CefString& name, 
	CefRefPtr<CefV8Value> object, 
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) 
{
	CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(name);
	// Retrieve the argument list object.
	CefRefPtr<CefListValue> args = msg->GetArgumentList();

	if (name == "call")
	{
		// ����û�в����б�ĵ��ã��ڶ�������Ϊ�ص�
		// ��������˲����б���ô�ص��ǵ���������
		CefString function_name = arguments[0]->GetStringValue();
		CefString params = "{}";
		CefRefPtr<CefV8Value> callback;
		if (arguments.size() > 1 && arguments[0]->IsString() &&arguments[1]->IsFunction())
		{
			callback = arguments[1];
		}
		else if (arguments.size() > 2 && arguments[0]->IsString() && arguments[1]->IsString() && arguments[2]->IsFunction())
		{
			params = arguments[1]->GetStringValue();
			callback = arguments[2];
		}
		//else
		//{
		//	exception = "Invalid arguments.";
		//	return false;
		//}

		// ִ�� C++ ����
		if (!js_bridge_->CallCppFunction(function_name, params, callback))
		{
			exception = function_name;
			return false;
		}

		return true;
	}
	else if (name == "register")
	{
		if (arguments[0]->IsString() && arguments[1]->IsFunction())
		{
			std::string function_name = arguments[0]->GetStringValue();
			CefRefPtr<CefV8Value> callback = arguments[1];
			if (!js_bridge_->RegisterJSFunc(function_name, callback))
			{
				exception = "Failed to register function.";
				return false;
			}
			return true;
		}
		else
		{
			exception = "Invalid arguments.";
			return false;
		}
	}
	else
	{
		int _size = arguments.size();

		args->SetSize(_size);

		for (int i = 0; i < _size; i++)
		{
			if (arguments[i]->IsString())
			{
				args->SetString(i, arguments[i]->GetStringValue());
			}
			else if (arguments[i]->IsInt())
			{
				args->SetInt(i, arguments[i]->GetIntValue());
			}
			else if (arguments[i]->IsDouble())
			{
				args->SetDouble(i, arguments[i]->GetDoubleValue());
			}
			else if (arguments[i]->IsBool())
			{
				args->SetBool(i, arguments[i]->GetBoolValue());
			}
			else
			{
				args->SetString(i, arguments[i]->GetStringValue());
			}
		}
		//testFun(args);
		context->GetBrowser()->SendProcessMessage(PID_BROWSER, msg);
	}
	return true;
}