#if !defined(TGRAPHICS_H)
#define TGRAPHICS_H

#pragma once
class TGraphics
{
public:
	TGraphics();
	~TGraphics();

	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void DrawPolyline(HDC hDC, POINT* points, int numberPoints, COLORREF color);
	void DrawPolygon(HDC hDC, POINT* points, int numberPoints, COLORREF color);
	void DrawEllipse(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void DrawArc(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	void DrawPie(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	void DrawChord(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	void DrawRoundRect(HDC hDC, int x1, int y1, int x2, int y2, int width, int height, COLORREF color);
	void DrawBezier(HDC hDC, POINT* points, int numPoints, COLORREF color);
};

#endif