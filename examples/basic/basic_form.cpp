#include "stdafx.h"
#include "basic_form.h"

const std::wstring BasicForm::kClassName = L"Basic";

BasicForm::BasicForm()
{
}


BasicForm::~BasicForm()
{
}

std::wstring BasicForm::GetSkinFolder()
{
	return L"basic";
}

std::wstring BasicForm::GetSkinFile()
{
	return L"basic.xml";
}

std::wstring BasicForm::GetWindowClassName() const
{
	return kClassName;
}

void BasicForm::InitWindow()
{
	ui::Box* control_box = static_cast<ui::Box*>(FindControl(L"watermark"));
	control_box->SetWatermarkType(1);

	ui::IWatermark info;
	info.m_strText = L"111111111";
	info.m_strColor = L"watermark_color";
	info.m_bUseWatermark = true;
	info.m_sFontId = L"system_water_12";
	info.m_nVStep = 75;
	control_box->SetWatermarkInfo(info);
}

LRESULT BasicForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}
