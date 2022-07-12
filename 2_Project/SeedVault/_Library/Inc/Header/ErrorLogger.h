#pragma once
//#define NOMINMAX
#include <Windows.h>
#include <string>

class COMException;

class ErrorLogger
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(COMException& exception);
};