#include "StdAfx.h"

namespace ui
{
	void Watermark::SetUseWatermark(bool bUseWatermark)
	{
		if (m_bUseWatermark != bUseWatermark)
		{
			SetUseWatermark_i(bUseWatermark);
			OnWatermarkChange();
		}
	}

	bool Watermark::GetUseWatermark()
	{
		return m_bUseWatermark;
	}


	void Watermark::SetWatermarkFont(std::wstring sFontId)
	{
		if (m_sFontId != sFontId)
		{
			SetWatermarkFont_i(sFontId);
			OnWatermarkChange();
		}
	}

	std::wstring Watermark::GetWatermarkFont()
	{
		return m_sFontId;
	}


	void Watermark::SetWatermarkTextColor(const std::wstring& strColor)
	{
		if (m_strColor.compare(strColor) != 0)
		{
			SetWatermarkTextColor_i(strColor);
			OnWatermarkChange();
		}
	}

	void Watermark::SetWatermarkTextColor_i(const std::wstring& strColor)
	{
		m_strColor = strColor;
		m_dwTextColor = GlobalManager::GetTextColor(m_strColor);
		//使用GDI画不取阿尔法通道值 没有考虑大小端的情况
		//m_dwTextColor = RGB(((m_dwTextColor >> 16) & 0xFF), ((m_dwTextColor >> 8) & 0xFF), ((m_dwTextColor)& 0xFF));
		m_dwTextColor = RGB(GetRValue(m_dwTextColor), GetGValue(m_dwTextColor), GetBValue(m_dwTextColor));
	}

	std::wstring Watermark::GetWatermarkTextColor()
	{
		return m_strColor; 
	}


	void Watermark::SetWatermarkTextColor(DWORD dwColor)
	{
		if (m_dwTextColor != dwColor)
		{
			SetWatermarkTextColor_i(dwColor);
			OnWatermarkChange();
		}
	}

	DWORD Watermark::GetDWordWatermarkTextColor()
	{
		return m_dwTextColor; 
	}

	void Watermark::SetWatermarkText(const std::wstring& strText)
	{
		if (m_strText.compare(strText) != 0)
		{
			SetWatermarkText_i(strText);
			OnWatermarkChange();
		}
	}

	std::wstring Watermark::GetWatermarkText()
	{ 
		return m_strText;
	}


	void Watermark::SetWatermarkMargin(const ui::UiRect rcMargin)
	{
		if (!m_rcMargin_.Equal(rcMargin))
		{
			SetWatermarkMargin_i(rcMargin);
			OnWatermarkChange();
		}
	}

	ui::UiRect Watermark::GetWatermarkMargin()
	{
		return m_rcMargin_; 
	}

	void Watermark::SetWatermarkVStep(unsigned int nStep)
	{
		if (m_nVStep != nStep)
		{
			SetWatermarkVStep_i(nStep);
			OnWatermarkChange();
		}
	}

	unsigned int Watermark::GetWatermarkVStep()
	{ 
		return m_nVStep;
	}

	void Watermark::SetWatermarkType(int nType)
	{ 
		m_nType = nType;
	}

	int Watermark::GetWatermarkType()
	{ 
		return m_nType;
	}

	void Watermark::SetWatermarkInfo(const IWatermark& info)
	{
		bool update = false;
		if (info.m_sFontId != L"" && m_sFontId != info.m_sFontId)
		{
			update = true;
			SetWatermarkFont_i(info.m_sFontId);
		}
		if (info.m_nVStep != 0xFFFF && m_nVStep != info.m_nVStep)
		{
			update = true;
			SetWatermarkVStep_i(info.m_nVStep);
		}
		if (!info.m_rcMargin_.IsRectEmpty() && !m_rcMargin_.Equal(info.m_rcMargin_))
		{
			update = true;
			SetWatermarkMargin_i(info.m_rcMargin_);
		}
		if (!info.m_strColor.empty() && m_strColor.compare(info.m_strColor) != 0)
		{
			update = true;
			SetWatermarkTextColor_i(info.m_strColor);
		}
		if (!info.m_strText.empty() && m_strText.compare(info.m_strText) != 0)//如果清空，设置不展示水印，或调用单独接口
		{
			update = true;
			SetWatermarkText_i(info.m_strText);
		}
		if (info.m_bUseWatermark != m_bUseWatermark)
		{
			update = true;
			SetUseWatermark_i(info.m_bUseWatermark);
		}
		OnWatermarkChange();
	}

	void Watermark::PaintWatermark(IRenderContext* pRender, const ui::UiRect rcDraw)
	{
		std::wstring strText(m_strText);
		int i = m_strText.length();
		while (i > 0)
		{
			if (i < 6)
				strText = L" " + strText;
			else
				strText = strText + L" ";
			i--;
		}
		HDC hDC = pRender->GetDC();
		//使用GDI绘制，不调用RenderEngine::DrawText绘制
		ui::UiRect rcMem = { 0, 0, rcDraw.GetWidth(), rcDraw.GetHeight() };
		HDC hmemDC = CreateCompatibleDC(hDC);
		HBITMAP hmemBMP = CreateCompatibleBitmap(hDC, rcMem.GetWidth(), rcMem.GetHeight());
		HBITMAP oldBMP = (HBITMAP)SelectObject(hmemDC, hmemBMP);
		::BitBlt(hmemDC, 0, 0, rcMem.GetWidth(), rcMem.GetHeight(), hDC, rcDraw.left, rcDraw.top, SRCCOPY);

		HFONT hOldFont = (HFONT)SelectObject(hmemDC, GlobalManager::GetFont(m_sFontId));
		COLORREF oldColor = ::SetTextColor(hmemDC, m_dwTextColor);
		int oldBkMode = ::SetBkMode(hmemDC, TRANSPARENT);
		SIZE szText;
		::GetTextExtentExPoint(hmemDC, strText.c_str(), strText.length(), 0, NULL, NULL, &szText);
		LOGFONT logfont = { 0 };
		::GetObject(GlobalManager::GetFont(m_sFontId), sizeof(LOGFONT), &logfont);

		int nEscapement = 50 % 81;
		if (nEscapement < 10) nEscapement = 10;
		nEscapement *= 10;
		float angle = nEscapement / 10 * 3.14 / 180;
		int watermark_width = szText.cx*cos(angle) + szText.cy*(cos(angle) + sin(angle));
		if (watermark_width < szText.cy) watermark_width = szText.cy;//竖起来了???
		int watermark_height = watermark_width*tan(angle) + szText.cy*sin(angle) + szText.cx / strText.length()*cos(angle);
		if (watermark_height < szText.cy) watermark_height = szText.cy;
		int container_width = watermark_width + m_rcMargin_.left + m_rcMargin_.right;
		int container_height = watermark_height + m_rcMargin_.top + m_rcMargin_.bottom;
		int temp_left = 0, temp_top = 0;
		int draw_times = 1;
		int nVStep = GetWatermarkVStep();
		while (true)
		{
			temp_top = rcMem.top + nVStep*draw_times - container_height;
			while (temp_left <= rcMem.right)
			{
				RECT rc = { temp_left + m_rcMargin_.left, temp_top, 0, 0 };
				rc.right = rc.left + watermark_width;
				rc.bottom = rc.top + watermark_height;
				if (!rcMem.IsPointIn(ui::CPoint(rc.left, rc.bottom)) && rc.left > rcMem.right)
					break;
				if (draw_times % 2 == 0)
					::DrawText(hmemDC, strText.c_str(), strText.length(), &rc, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
				else
					::DrawText(hmemDC, L"DUILIB", 6, &rc, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
				temp_left += container_width;
				//temp_top -= container_height;
			}
			draw_times += 1;
			temp_left = rcMem.left;
			if (draw_times * nVStep > rcMem.GetHeight())
			{
				if ((int)((draw_times * nVStep - rcDraw.GetHeight()) / tan(angle)) > rcMem.GetWidth())
					break;
			}
		}
		BitBlt(hDC, rcDraw.left, rcDraw.top, rcMem.GetWidth(), rcMem.GetHeight(), hmemDC, 0, 0, SRCCOPY);
		SelectObject(hmemDC, hOldFont);
		::SetTextColor(hmemDC, oldColor);
		::SetBkMode(hmemDC, oldBkMode);
		SelectObject(hmemDC, oldBMP);
		DeleteObject(hmemBMP);
		DeleteObject(hmemDC);
	}

}