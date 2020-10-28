#include "stdafx.h"
#include "InsetWindow.h"


CInsetWindow::CInsetWindow(int Id)
{
	m_Id = Id;
}

CInsetWindow::~CInsetWindow()
{
}

void CInsetWindow::setAirport(string icao)
{
	this->icao = icao;
}

void CInsetWindow::OnClickScreenObject(const char * sItemString, POINT Pt, int Button)
{
	if (Button == EuroScopePlugIn::BUTTON_RIGHT)
	{
		if (m_TagAngles.find(sItemString) != m_TagAngles.end())
		{
			m_TagAngles[sItemString] = fmod(m_TagAngles[sItemString] + 45.0, 360.0);
		} else
		{
			m_TagAngles[sItemString] = 45.0;
		}
	}

	if (Button == EuroScopePlugIn::BUTTON_LEFT)
	{
		if (m_TagAngles.find(sItemString) != m_TagAngles.end())
		{
			m_TagAngles[sItemString] = fmod(m_TagAngles[sItemString] - 45.0, 360.0);
		}
		else
		{
			m_TagAngles[sItemString] = 45.0;
		}
	}
}

bool CInsetWindow::OnMoveScreenObject(const char * sObjectId, POINT Pt, RECT Area, bool Released)
{
	if (strcmp(sObjectId, "window") == 0) {
		if (!this->m_Grip)
		{
			m_OffsetInit = m_Offset;
			m_OffsetDrag = Pt;
			m_Grip = true;
		}

		POINT maxoffset = { (m_Area.right - m_Area.left) / 2, (m_Area.bottom - (m_Area.top + 15)) / 2 };
		m_Offset.x = max(-maxoffset.x, min(maxoffset.x, m_OffsetInit.x + (Pt.x - m_OffsetDrag.x)));
		m_Offset.y = max(-maxoffset.y, min(maxoffset.y, m_OffsetInit.y + (Pt.y - m_OffsetDrag.y)));

		if (Released)
		{
			m_Grip = false;
		}
	}
	if (strcmp(sObjectId, "resize") == 0) {
		POINT TopLeft = { m_Area.left, m_Area.top };
		POINT BottomRight = { Area.right, Area.bottom };

		CRect newSize(TopLeft, BottomRight);
		newSize.NormalizeRect();

		if (newSize.Height() < 100) {
			newSize.top = m_Area.top;
			newSize.bottom = m_Area.bottom;
		}

		if (newSize.Width() < 300) {
			newSize.left = m_Area.left;
			newSize.right = m_Area.right;
		}

		m_Area = newSize;

		return Released;
	}
	if (strcmp(sObjectId, "topbar") == 0) {

		CRect appWindowRect(m_Area);
		appWindowRect.NormalizeRect();

		POINT TopLeft = { Area.left, Area.bottom + 1 };
		POINT BottomRight = { TopLeft.x + appWindowRect.Width(), TopLeft.y + appWindowRect.Height() };
		CRect newPos(TopLeft, BottomRight);
		newPos.NormalizeRect();

		m_Area = newPos;

		return Released;
	}

	return true;
}

POINT CInsetWindow::projectPoint(CPosition pos)
{
	CRect areaRect(m_Area);
	areaRect.NormalizeRect();

	POINT refPt = areaRect.CenterPoint();
	refPt.x += m_Offset.x;
	refPt.y += m_Offset.y;

	POINT out = {0, 0};

	double dist = AptPositions[icao].DistanceTo(pos);
	double dir = TrueBearing(AptPositions[icao], pos);


	out.x = refPt.x + int(m_Scale * dist * sin(dir) + 0.5);
	out.y = refPt.y - int(m_Scale * dist * cos(dir) + 0.5);

	if (m_Rotation != 0)
	{
		return rotate_point(out, m_Rotation, refPt);
	} else
	{
		return out;
	}
}

void CInsetWindow::render(HDC hDC, CSMRRadar * radar_screen, Graphics* gdi, POINT mouseLocation, multimap<string, string> DistanceTools)
{
	CDC dc;
	dc.Attach(hDC);

	if (this->m_Id == -1)
		return;

	struct Utils
	{
		static string getEnumString(CSMRRadar::TagTypes type) {
			if (type == CSMRRadar::TagTypes::Departure)
				return "departure";
			if (type == CSMRRadar::TagTypes::Arrival)
				return "arrival";
			if (type == CSMRRadar::TagTypes::Uncorrelated)
				return "uncorrelated";
			return "airborne";
		}
		static RECT GetAreaFromText(CDC * dc, string text, POINT Pos) {
			RECT Area = { Pos.x, Pos.y, Pos.x + dc->GetTextExtent(text.c_str()).cx, Pos.y + dc->GetTextExtent(text.c_str()).cy };
			return Area;
		}

		static RECT drawToolbarButton(CDC * dc, string letter, CRect TopBar, int left, POINT mouseLocation)
		{
			POINT TopLeft = { TopBar.right - left, TopBar.top + 2 };
			POINT BottomRight = { TopBar.right - (left - 11), TopBar.bottom - 2 };
			CRect Rect(TopLeft, BottomRight);
			Rect.NormalizeRect();
			CBrush ButtonBrush(RGB(60, 60, 60));
			dc->FillRect(Rect, &ButtonBrush);
			dc->SetTextColor(RGB(0, 0, 0));
			dc->TextOutA(Rect.left + 2, Rect.top, letter.c_str());

			if (mouseWithin(mouseLocation, Rect))
				dc->Draw3dRect(Rect, RGB(45, 45, 45), RGB(75, 75, 75));
			else
				dc->Draw3dRect(Rect, RGB(75, 75, 75), RGB(45, 45, 45));

			return Rect;
		}
	};

	icao = radar_screen->ActiveAirport;
	AptPositions = radar_screen->AirportPositions;

	COLORREF qBackgroundColor = radar_screen->CurrentConfig->getConfigColorRef(radar_screen->CurrentConfig->getActiveProfile()["approach_insets"]["background_color"]);
	CRect windowAreaCRect(m_Area);
	windowAreaCRect.NormalizeRect();

	// We create the radar
	dc.FillSolidRect(windowAreaCRect, qBackgroundColor);
	radar_screen->AddScreenObject(m_Id, "window", m_Area, true, "");

	auto scale = m_Scale;

	POINT refPt = windowAreaCRect.CenterPoint();
	refPt.x += m_Offset.x;
	refPt.y += m_Offset.y;

	// Here we draw all runways for the airport
	CSectorElement rwy;
	for (rwy = radar_screen->GetPlugIn()->SectorFileElementSelectFirst(SECTOR_ELEMENT_RUNWAY);
		rwy.IsValid();
		rwy = radar_screen->GetPlugIn()->SectorFileElementSelectNext(rwy, SECTOR_ELEMENT_RUNWAY))
	{

		if (startsWith(icao.c_str(), rwy.GetAirportName()))
		{

			CPen RunwayPen(PS_SOLID, 1, radar_screen->CurrentConfig->getConfigColorRef(radar_screen->CurrentConfig->getActiveProfile()["approach_insets"]["runway_color"]));
			CPen ExtendedCentreLinePen(PS_SOLID, 1, radar_screen->CurrentConfig->getConfigColorRef(radar_screen->CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_color"]));
			CPen* oldPen = dc.SelectObject(&RunwayPen);

			CPosition EndOne, EndTwo;
			rwy.GetPosition(&EndOne, 0);
			rwy.GetPosition(&EndTwo, 1);

			POINT Pt1, Pt2;
			Pt1 = projectPoint(EndOne);
			Pt2 = projectPoint(EndTwo);

			POINT toDraw1, toDraw2;
			if (LiangBarsky(m_Area, Pt1, Pt2, toDraw1, toDraw2)) {
				dc.MoveTo(toDraw1);
				dc.LineTo(toDraw2);

			}

			if (rwy.IsElementActive(false, 0) || rwy.IsElementActive(false, 1))
			{
				CPosition Threshold, OtherEnd;
				if (rwy.IsElementActive(false, 0))
				{
					Threshold = EndOne; 
					OtherEnd = EndTwo;
				} else
				{
					Threshold = EndTwo; 
					OtherEnd = EndOne;
				}
					

				double reverseHeading = RadToDeg(TrueBearing(OtherEnd, Threshold));
				double lenght = double(radar_screen->CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_length"].GetDouble()) * 1852.0;

				// Drawing the extended centreline
				CPosition endExtended = BetterHarversine(Threshold, reverseHeading, lenght);

				Pt1 = projectPoint(Threshold);
				Pt2 = projectPoint(endExtended);

				if (LiangBarsky(m_Area, Pt1, Pt2, toDraw1, toDraw2)) {
					dc.SelectObject(&ExtendedCentreLinePen);
					dc.MoveTo(toDraw1);
					dc.LineTo(toDraw2);
				}

				// Drawing the ticks
				int increment = radar_screen->CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_ticks_spacing"].GetInt() * 1852;

				for (int j = increment; j <= int(radar_screen->CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_length"].GetInt() * 1852); j += increment) {

					CPosition tickPosition = BetterHarversine(Threshold, reverseHeading, j);
					CPosition tickBottom = BetterHarversine(tickPosition, fmod(reverseHeading - 90, 360), 500);
					CPosition tickTop = BetterHarversine(tickPosition, fmod(reverseHeading + 90, 360), 500);


					Pt1 = projectPoint(tickBottom);
					Pt2 = projectPoint(tickTop);

					if (LiangBarsky(m_Area, Pt1, Pt2, toDraw1, toDraw2)) {
						dc.SelectObject(&ExtendedCentreLinePen);
						dc.MoveTo(toDraw1);
						dc.LineTo(toDraw2);
					}

				}
			} 

			dc.SelectObject(&oldPen);
		}
	}

	// Aircrafts

	vector<POINT> appAreaVect = { windowAreaCRect.TopLeft(),{ windowAreaCRect.right, windowAreaCRect.top }, windowAreaCRect.BottomRight(),{ windowAreaCRect.left, windowAreaCRect.bottom } };
	CPen WhitePen(PS_SOLID, 1, radar_screen->ColorManager->get_corrected_color("symbol", Color::White).ToCOLORREF());

	CRadarTarget rt;
	for (rt = radar_screen->GetPlugIn()->RadarTargetSelectFirst();
		rt.IsValid();
		rt = radar_screen->GetPlugIn()->RadarTargetSelectNext(rt))
	{
		int radarRange = radar_screen->CurrentConfig->getActiveProfile()["filters"]["radar_range_nm"].GetInt();

		if (rt.GetGS() < 60 ||
			rt.GetPosition().GetPressureAltitude() > m_Filter ||
			!rt.IsValid() ||
			!rt.GetPosition().IsValid() ||
			rt.GetPosition().GetPosition().DistanceTo(AptPositions[icao]) > radarRange)
			continue;

		CPosition RtPos2 = rt.GetPosition().GetPosition();
		CRadarTargetPositionData RtPos = rt.GetPosition();
		auto fp = radar_screen->GetPlugIn()->FlightPlanSelect(rt.GetCallsign());
		auto reportedGs = RtPos.GetReportedGS();

		// Filtering the targets

		POINT RtPoint, hPoint;

		RtPoint = projectPoint(RtPos2);

		CRadarTargetPositionData hPos = rt.GetPreviousPosition(rt.GetPosition());
		for (int i = 1; i < radar_screen->Trail_App; i++) {
			if (!hPos.IsValid())
				continue;

			hPoint = projectPoint(hPos.GetPosition());

			if (Is_Inside(hPoint, appAreaVect)) {
				dc.SetPixel(hPoint, radar_screen->ColorManager->get_corrected_color("symbol", Color::White).ToCOLORREF());
			}

			hPos = rt.GetPreviousPosition(hPos);
		}

		if (Is_Inside(RtPoint, appAreaVect)) {
			dc.SelectObject(&WhitePen);

			if (RtPos.GetTransponderC()) {
				dc.MoveTo({ RtPoint.x, RtPoint.y - 4 });
				dc.LineTo({ RtPoint.x - 4, RtPoint.y });
				dc.LineTo({ RtPoint.x, RtPoint.y + 4 });
				dc.LineTo({ RtPoint.x + 4, RtPoint.y });
				dc.LineTo({ RtPoint.x, RtPoint.y - 4 });
			}
			else {
				dc.MoveTo(RtPoint.x, RtPoint.y);
				dc.LineTo(RtPoint.x - 4, RtPoint.y - 4);
				dc.MoveTo(RtPoint.x, RtPoint.y);
				dc.LineTo(RtPoint.x + 4, RtPoint.y - 4);
				dc.MoveTo(RtPoint.x, RtPoint.y);
				dc.LineTo(RtPoint.x - 4, RtPoint.y + 4);
				dc.MoveTo(RtPoint.x, RtPoint.y);
				dc.LineTo(RtPoint.x + 4, RtPoint.y + 4);
			}

			CRect TargetArea(RtPoint.x - 4, RtPoint.y - 4, RtPoint.x + 4, RtPoint.y + 4);
			TargetArea.NormalizeRect();
			radar_screen->AddScreenObject(DRAWING_AC_SYMBOL_APPWINDOW_BASE + (m_Id - APPWINDOW_BASE), rt.GetCallsign(), TargetArea, false, radar_screen->GetBottomLine(rt.GetCallsign()).c_str());
		}

		// Predicted Track Line
		// It starts 10 seconds away from the ac
		if (radar_screen->PredictedLength > 0) {
			double d = double(rt.GetPosition().GetReportedGS() * 0.514444) * 10;
			CPosition AwayBase = BetterHarversine(rt.GetPosition().GetPosition(), rt.GetTrackHeading(), d);

			d = double(rt.GetPosition().GetReportedGS() * 0.514444) * (radar_screen->PredictedLength * 60) - 10;
			CPosition PredictedEnd = BetterHarversine(AwayBase, rt.GetTrackHeading(), d);

			POINT liangOne, liangTwo;

			if (LiangBarsky(m_Area, projectPoint(AwayBase), projectPoint(PredictedEnd), liangOne, liangTwo))
			{
				dc.SelectObject(&WhitePen);
				dc.MoveTo(liangOne);
				dc.LineTo(liangTwo);
			}
		}

		if (mouseWithin(mouseLocation, { RtPoint.x - 4, RtPoint.y - 4, RtPoint.x + 4, RtPoint.y + 4 })) {
			dc.MoveTo(RtPoint.x, RtPoint.y - 6);
			dc.LineTo(RtPoint.x - 4, RtPoint.y - 10);
			dc.MoveTo(RtPoint.x, RtPoint.y - 6);
			dc.LineTo(RtPoint.x + 4, RtPoint.y - 10);

			dc.MoveTo(RtPoint.x, RtPoint.y + 6);
			dc.LineTo(RtPoint.x - 4, RtPoint.y + 10);
			dc.MoveTo(RtPoint.x, RtPoint.y + 6);
			dc.LineTo(RtPoint.x + 4, RtPoint.y + 10);

			dc.MoveTo(RtPoint.x - 6, RtPoint.y);
			dc.LineTo(RtPoint.x - 10, RtPoint.y - 4);
			dc.MoveTo(RtPoint.x - 6, RtPoint.y);
			dc.LineTo(RtPoint.x - 10, RtPoint.y + 4);

			dc.MoveTo(RtPoint.x + 6, RtPoint.y);
			dc.LineTo(RtPoint.x + 10, RtPoint.y - 4);
			dc.MoveTo(RtPoint.x + 6, RtPoint.y);
			dc.LineTo(RtPoint.x + 10, RtPoint.y + 4);
		}

		int lenght = 50;

		POINT TagCenter;
		if (m_TagAngles.find(rt.GetCallsign()) == m_TagAngles.end())
		{
			m_TagAngles[rt.GetCallsign()] = 45.0; // TODO: Not the best, ah well
		}

		TagCenter.x = long(RtPoint.x + float(lenght * cos(DegToRad(m_TagAngles[rt.GetCallsign()]))));
		TagCenter.y = long(RtPoint.y + float(lenght * sin(DegToRad(m_TagAngles[rt.GetCallsign()]))));
		// Drawing the tags, what a mess

		// ----- Generating the replacing map -----
		map<string, string> TagReplacingMap = CSMRRadar::GenerateTagData(rt, fp, radar_screen->IsCorrelated(fp, rt), radar_screen->CurrentConfig->getActiveProfile()["filters"]["pro_mode"]["enable"].GetBool(), radar_screen->GetPlugIn()->GetTransitionAltitude(), radar_screen->CurrentConfig->getActiveProfile()["labels"]["use_aspeed_for_gate"].GetBool(), icao);

		// ----- Generating the clickable map -----
		map<string, int> TagClickableMap;
		TagClickableMap[TagReplacingMap["callsign"]] = TAG_CITEM_CALLSIGN;
		TagClickableMap[TagReplacingMap["actype"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["sctype"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["sqerror"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["deprwy"]] = TAG_CITEM_RWY;
		TagClickableMap[TagReplacingMap["seprwy"]] = TAG_CITEM_RWY;
		TagClickableMap[TagReplacingMap["arvrwy"]] = TAG_CITEM_RWY;
		TagClickableMap[TagReplacingMap["srvrwy"]] = TAG_CITEM_RWY;
		TagClickableMap[TagReplacingMap["gate"]] = TAG_CITEM_GATE;
		TagClickableMap[TagReplacingMap["sate"]] = TAG_CITEM_GATE;
		TagClickableMap[TagReplacingMap["flightlevel"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["gs"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["tendency"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["wake"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["tssr"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["sid"]] = TagClickableMap[TagReplacingMap["shid"]] = TAG_CITEM_SID;
		TagClickableMap[TagReplacingMap["origin"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["dest"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["systemid"]] = TAG_CITEM_MANUALCORRELATE;
		TagClickableMap[TagReplacingMap["gstatus"]] = TAG_CITEM_GROUNDSTATUS;
		TagClickableMap[TagReplacingMap["uk_stand"]] = TAG_CITEM_UKSTAND;


		//
		// ----- Now the hard part, drawing (using gdi+) -------
		//	

		CSMRRadar::TagTypes TagType = CSMRRadar::TagTypes::Departure;
		CSMRRadar::TagTypes ColorTagType = CSMRRadar::TagTypes::Departure;

		if (fp.IsValid() && strcmp(fp.GetFlightPlanData().GetDestination(), radar_screen->getActiveAirport().c_str()) == 0) {
				TagType = CSMRRadar::TagTypes::Arrival;
				ColorTagType = CSMRRadar::TagTypes::Arrival;
		}

		if (reportedGs > 50) {
			TagType = CSMRRadar::TagTypes::Airborne;

			// Is "use_departure_arrival_coloring" enabled? if not, then use the airborne colors
			bool useDepArrColors = radar_screen->CurrentConfig->getActiveProfile()["labels"]["airborne"]["use_departure_arrival_coloring"].GetBool();
			if (!useDepArrColors) {
				ColorTagType = CSMRRadar::TagTypes::Airborne;
			}
		}

		bool AcisCorrelated = radar_screen->IsCorrelated(radar_screen->GetPlugIn()->FlightPlanSelect(rt.GetCallsign()), rt);
		if (!AcisCorrelated && reportedGs >= 3)
		{
			TagType = CSMRRadar::TagTypes::Uncorrelated;
			ColorTagType = CSMRRadar::TagTypes::Uncorrelated;
		}

		// First we need to figure out the tag size

		int TagWidth = 0, TagHeight = 0;
		RectF mesureRect;
		gdi->MeasureString(L" ", wcslen(L" "), radar_screen->customFonts[radar_screen->currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &mesureRect);
		int blankWidth = (int)mesureRect.GetRight();

		mesureRect = RectF(0, 0, 0, 0);
		gdi->MeasureString(L"AZERTYUIOPQSDFGHJKLMWXCVBN", wcslen(L"AZERTYUIOPQSDFGHJKLMWXCVBN"),
			radar_screen->customFonts[radar_screen->currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &mesureRect);
		int oneLineHeight = (int)mesureRect.GetBottom();

		const Value& LabelsSettings = radar_screen->CurrentConfig->getActiveProfile()["labels"];
		const Value& LabelLines = LabelsSettings[Utils::getEnumString(TagType).c_str()]["definition"];
		vector<vector<string>> ReplacedLabelLines;

		if (!LabelLines.IsArray())
			return;

		for (unsigned int i = 0; i < LabelLines.Size(); i++)
		{

			const Value& line = LabelLines[i];
			vector<string> lineStringArray;

			// Adds one line height
			TagHeight += oneLineHeight;

			int TempTagWidth = 0;

			for (unsigned int j = 0; j < line.Size(); j++)
			{
				mesureRect = RectF(0, 0, 0, 0);
				string element = line[j].GetString();

				for (auto& kv : TagReplacingMap)
					replaceAll(element, kv.first, kv.second);

				lineStringArray.push_back(element);

				wstring wstr = wstring(element.begin(), element.end());
				gdi->MeasureString(wstr.c_str(), wcslen(wstr.c_str()),
					radar_screen->customFonts[radar_screen->currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &mesureRect);

				TempTagWidth += (int) mesureRect.GetRight();

				if (j != line.Size() - 1)
					TempTagWidth += (int) blankWidth;
			}

			TagWidth = max(TagWidth, TempTagWidth);

			ReplacedLabelLines.push_back(lineStringArray);
		}
		TagHeight = TagHeight - 2;

		// Pfiou, done with that, now we can draw the actual rectangle.

		// We need to figure out if the tag color changes according to RIMCAS alerts, or not
		bool rimcasLabelOnly = radar_screen->CurrentConfig->getActiveProfile()["rimcas"]["rimcas_label_only"].GetBool();

		Color definedBackgroundColor = radar_screen->CurrentConfig->getConfigColor(LabelsSettings[Utils::getEnumString(ColorTagType).c_str()]["background_color"]);
		if (TagType == CSMRRadar::TagTypes::Departure) {
			if (!TagReplacingMap["sid"].empty() && radar_screen->CurrentConfig->isSidColorAvail(TagReplacingMap["sid"], radar_screen->getActiveAirport())) {
				definedBackgroundColor = radar_screen->CurrentConfig->getSidColor(TagReplacingMap["sid"], radar_screen->getActiveAirport());
			}

			if (fp.GetFlightPlanData().GetPlanType()[0] == 'I' && TagReplacingMap["asid"].empty() && LabelsSettings[Utils::getEnumString(ColorTagType).c_str()].HasMember("nosid_color")) {
				definedBackgroundColor = radar_screen->CurrentConfig->getConfigColor(LabelsSettings[Utils::getEnumString(ColorTagType).c_str()]["nosid_color"]);
			}
		}
			if (TagReplacingMap["actype"] == "NoFPL" && LabelsSettings[Utils::getEnumString(ColorTagType).c_str()].HasMember("nofpl_color")) {
				definedBackgroundColor = radar_screen->CurrentConfig->getConfigColor(LabelsSettings[Utils::getEnumString(ColorTagType).c_str()]["nofpl_color"]);
		}

		Color TagBackgroundColor = radar_screen->RimcasInstance->GetAircraftColor(rt.GetCallsign(),
			definedBackgroundColor,
			radar_screen->CurrentConfig->getConfigColor(LabelsSettings[Utils::getEnumString(ColorTagType).c_str()]["background_color_on_runway"]),
			radar_screen->CurrentConfig->getConfigColor(radar_screen->CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
			radar_screen->CurrentConfig->getConfigColor(radar_screen->CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"]));

		if (rimcasLabelOnly)
			TagBackgroundColor = radar_screen->RimcasInstance->GetAircraftColor(rt.GetCallsign(),
				definedBackgroundColor,
				radar_screen->CurrentConfig->getConfigColor(LabelsSettings[Utils::getEnumString(ColorTagType).c_str()]["background_color_on_runway"]));

		CRect TagBackgroundRect(TagCenter.x - (TagWidth / 2), TagCenter.y - (TagHeight / 2), TagCenter.x + (TagWidth / 2), TagCenter.y + (TagHeight / 2));

		if (Is_Inside(TagBackgroundRect.TopLeft(), appAreaVect) &&
			Is_Inside(RtPoint, appAreaVect) &&
			Is_Inside(TagBackgroundRect.BottomRight(), appAreaVect)) {

			SolidBrush TagBackgroundBrush(TagBackgroundColor);
			gdi->FillRectangle(&TagBackgroundBrush, CopyRect(TagBackgroundRect));

			SolidBrush FontColor(radar_screen->ColorManager->get_corrected_color("label",
				radar_screen->CurrentConfig->getConfigColor(LabelsSettings[Utils::getEnumString(ColorTagType).c_str()]["text_color"])));
			SolidBrush SquawkErrorColor(radar_screen->ColorManager->get_corrected_color("label",
				radar_screen->CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"])));
			SolidBrush RimcasTextColor(radar_screen->CurrentConfig->getConfigColor(radar_screen->CurrentConfig->getActiveProfile()["rimcas"]["alert_text_color"]));

			int heightOffset = 0;
			for (auto&& line : ReplacedLabelLines)
			{
				int widthOffset = 0;
				for (auto&& element : line)
				{
					SolidBrush* color = &FontColor;
					if (TagReplacingMap["sqerror"].size() > 0 && strcmp(element.c_str(), TagReplacingMap["sqerror"].c_str()) == 0)
						color = &SquawkErrorColor;

					if (radar_screen->RimcasInstance->getAlert(rt.GetCallsign()) != CRimcas::NoAlert)
						color = &RimcasTextColor;

					RectF mRect(0, 0, 0, 0);

					wstring welement = wstring(element.begin(), element.end());

					gdi->DrawString(welement.c_str(), wcslen(welement.c_str()), radar_screen->customFonts[radar_screen->currentFontSize],
						PointF(Gdiplus::REAL(TagBackgroundRect.left + widthOffset), Gdiplus::REAL(TagBackgroundRect.top + heightOffset)),
						&Gdiplus::StringFormat(), color);


					gdi->MeasureString(welement.c_str(), wcslen(welement.c_str()),
						radar_screen->customFonts[radar_screen->currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &mRect);

					CRect ItemRect(TagBackgroundRect.left + widthOffset, TagBackgroundRect.top + heightOffset,
						TagBackgroundRect.left + widthOffset + (int)mRect.GetRight(), TagBackgroundRect.top + heightOffset + (int)mRect.GetBottom());

					radar_screen->AddScreenObject(TagClickableMap[element], rt.GetCallsign(), ItemRect, false, radar_screen->GetBottomLine(rt.GetCallsign()).c_str());

					widthOffset += (int)mRect.GetRight();
					widthOffset += blankWidth;
				}

				heightOffset += oneLineHeight;
			}

			// Drawing the leader line
			RECT TagBackRectData = TagBackgroundRect;
			POINT toDraw1, toDraw2;
			if (LiangBarsky(TagBackRectData, RtPoint, TagBackgroundRect.CenterPoint(), toDraw1, toDraw2))
				gdi->DrawLine(&Pen(radar_screen->ColorManager->get_corrected_color("symbol", Color::White)), PointF(Gdiplus::REAL(RtPoint.x), Gdiplus::REAL(RtPoint.y)), PointF(Gdiplus::REAL(toDraw1.x), Gdiplus::REAL(toDraw1.y)));

			// If we use a RIMCAS label only, we display it, and adapt the rectangle
			CRect oldCrectSave = TagBackgroundRect;

			if (rimcasLabelOnly) {
				Color RimcasLabelColor = radar_screen->RimcasInstance->GetAircraftColor(rt.GetCallsign(), Color::AliceBlue, Color::AliceBlue,
					radar_screen->CurrentConfig->getConfigColor(radar_screen->CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
					radar_screen->CurrentConfig->getConfigColor(radar_screen->CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"]));

				if (RimcasLabelColor.ToCOLORREF() != Color(Color::AliceBlue).ToCOLORREF()) {
					int rimcas_height = 0;

					wstring wrimcas_height = wstring(L"ALERT");

					RectF RectRimcas_height;

					gdi->MeasureString(wrimcas_height.c_str(), wcslen(wrimcas_height.c_str()), radar_screen->customFonts[radar_screen->currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &RectRimcas_height);
					rimcas_height = int(RectRimcas_height.GetBottom());

					// Drawing the rectangle

					CRect RimcasLabelRect(TagBackgroundRect.left, TagBackgroundRect.top - rimcas_height, TagBackgroundRect.right, TagBackgroundRect.top);
					gdi->FillRectangle(&SolidBrush(RimcasLabelColor), CopyRect(RimcasLabelRect));
					TagBackgroundRect.top -= rimcas_height;

					// Drawing the text

					wstring rimcasw = wstring(L"ALERT");
					StringFormat stformat = new StringFormat();
					stformat.SetAlignment(StringAlignment::StringAlignmentCenter);
					gdi->DrawString(rimcasw.c_str(), wcslen(rimcasw.c_str()), radar_screen->customFonts[radar_screen->currentFontSize], PointF(Gdiplus::REAL((TagBackgroundRect.left + TagBackgroundRect.right) / 2), Gdiplus::REAL(TagBackgroundRect.top)), &stformat, &RimcasTextColor);

				}
			}

			// Adding the tag screen object

			//radar_screen->AddScreenObject(DRAWING_TAG, rt.GetCallsign(), TagBackgroundRect, true, GetBottomLine(rt.GetCallsign()).c_str());

			TagBackgroundRect = oldCrectSave;

			// Now adding the clickable zones
		}
	}

	// Distance tools here
	for (auto&& kv : DistanceTools)
	{
		CRadarTarget one = radar_screen->GetPlugIn()->RadarTargetSelect(kv.first.c_str());
		CRadarTarget two = radar_screen->GetPlugIn()->RadarTargetSelect(kv.second.c_str());

		int radarRange = radar_screen->CurrentConfig->getActiveProfile()["filters"]["radar_range_nm"].GetInt();

		if (one.GetGS() < 60 ||
			one.GetPosition().GetPressureAltitude() > m_Filter ||
			!one.IsValid() ||
			!one.GetPosition().IsValid() ||
			one.GetPosition().GetPosition().DistanceTo(AptPositions[icao]) > radarRange)
			continue;

		if (two.GetGS() < 60 ||
			two.GetPosition().GetPressureAltitude() > m_Filter ||
			!two.IsValid() ||
			!two.GetPosition().IsValid() ||
			two.GetPosition().GetPosition().DistanceTo(AptPositions[icao]) > radarRange)
			continue;

		CPen Pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen *oldPen = dc.SelectObject(&Pen);

		POINT onePoint = projectPoint(one.GetPosition().GetPosition());
		POINT twoPoint = projectPoint(two.GetPosition().GetPosition());

		POINT toDraw1, toDraw2;
		if (LiangBarsky(m_Area, onePoint, twoPoint, toDraw1, toDraw2)) {
			dc.MoveTo(toDraw1);
			dc.LineTo(toDraw2);
		}

		POINT TextPos = { twoPoint.x + 20, twoPoint.y };

		double Distance = one.GetPosition().GetPosition().DistanceTo(two.GetPosition().GetPosition());
		double Bearing = one.GetPosition().GetPosition().DirectionTo(two.GetPosition().GetPosition());

		string distances = std::to_string(Distance);
		size_t decimal_pos = distances.find(".");
		distances = distances.substr(0, decimal_pos + 2);

		string bearings = std::to_string(Bearing);
		decimal_pos = bearings.find(".");
		bearings = bearings.substr(0, decimal_pos + 2);

		string text = bearings;
		text += "° / ";
		text += distances;
		text += "nm";
		COLORREF old_color = dc.SetTextColor(RGB(0, 0, 0));

		CRect ClickableRect = { TextPos.x - 2, TextPos.y, TextPos.x + dc.GetTextExtent(text.c_str()).cx + 2, TextPos.y + dc.GetTextExtent(text.c_str()).cy };
		if (Is_Inside(ClickableRect.TopLeft(), appAreaVect) && Is_Inside(ClickableRect.BottomRight(), appAreaVect))
		{
			gdi->FillRectangle(&SolidBrush(Color(127, 122, 122)), CopyRect(ClickableRect));
			dc.Draw3dRect(ClickableRect, RGB(75, 75, 75), RGB(45, 45, 45));
			dc.TextOutA(TextPos.x, TextPos.y, text.c_str());

			radar_screen->AddScreenObject(RIMCAS_DISTANCE_TOOL, string(kv.first + "," + kv.second).c_str(), ClickableRect, false, "");
		}
		
		dc.SetTextColor(old_color);

		dc.SelectObject(oldPen);
	}

	// Resize square
	qBackgroundColor = RGB(60, 60, 60);
	POINT BottomRight = { m_Area.right, m_Area.bottom };
	POINT TopLeft = { BottomRight.x - 10, BottomRight.y - 10 };
	CRect ResizeArea = { TopLeft, BottomRight };
	ResizeArea.NormalizeRect();
	dc.FillSolidRect(ResizeArea, qBackgroundColor);
	radar_screen->AddScreenObject(m_Id, "resize", ResizeArea, true, "");

	dc.Draw3dRect(ResizeArea, RGB(0, 0, 0), RGB(0, 0, 0));

	// Sides
	//CBrush FrameBrush(RGB(35, 35, 35));
	CBrush FrameBrush(RGB(127, 122, 122));
	COLORREF TopBarTextColor(RGB(35, 35, 35));
	dc.FrameRect(windowAreaCRect, &FrameBrush);

	// Topbar
	TopLeft = windowAreaCRect.TopLeft();
	TopLeft.y = TopLeft.y - 15;
	BottomRight = { windowAreaCRect.right, windowAreaCRect.top };
	CRect TopBar(TopLeft, BottomRight);
	TopBar.NormalizeRect();
	dc.FillRect(TopBar, &FrameBrush);
	POINT TopLeftText = { TopBar.left + 5, TopBar.bottom - dc.GetTextExtent("SRW 1").cy };
	COLORREF oldTextColorC = dc.SetTextColor(TopBarTextColor);

	radar_screen->AddScreenObject(m_Id, "topbar", TopBar, true, "");

	string Toptext = "SRW " + std::to_string(m_Id - APPWINDOW_BASE);
	dc.TextOutA(TopLeftText.x + (TopBar.right-TopBar.left) / 2 - dc.GetTextExtent("SRW 1").cx , TopLeftText.y, Toptext.c_str());

	// Range button
	CRect RangeRect = Utils::drawToolbarButton(&dc, "Z", TopBar, 29, mouseLocation);
	radar_screen->AddScreenObject(m_Id, "range", RangeRect, false, "");

	// Filter button
	CRect FilterRect = Utils::drawToolbarButton(&dc, "F", TopBar, 42, mouseLocation);
	radar_screen->AddScreenObject(m_Id, "filter", FilterRect, false, "");

	// Rotate button
	CRect RotateRect = Utils::drawToolbarButton(&dc, "R", TopBar, 55, mouseLocation);
	radar_screen->AddScreenObject(m_Id, "rotate", RotateRect, false, "");

	dc.SetTextColor(oldTextColorC);

	// Close
	POINT TopLeftClose = { TopBar.right - 16, TopBar.top + 2 };
	POINT BottomRightClose = { TopBar.right - 5, TopBar.bottom - 2 };
	CRect CloseRect(TopLeftClose, BottomRightClose);
	CloseRect.NormalizeRect();
	CBrush CloseBrush(RGB(60, 60, 60));
	dc.FillRect(CloseRect, &CloseBrush);
	CPen BlackPen(PS_SOLID, 1, RGB(0, 0, 0));
	dc.SelectObject(BlackPen);
	dc.MoveTo(CloseRect.TopLeft());
	dc.LineTo(CloseRect.BottomRight());
	dc.MoveTo({ CloseRect.right - 1, CloseRect.top });
	dc.LineTo({ CloseRect.left - 1, CloseRect.bottom });

	if (mouseWithin(mouseLocation, CloseRect))
		dc.Draw3dRect(CloseRect, RGB(45, 45, 45), RGB(75, 75, 75));
	else
		dc.Draw3dRect(CloseRect, RGB(75, 75, 75), RGB(45, 45, 45));

	radar_screen->AddScreenObject(m_Id, "close", CloseRect, false, "");

	dc.Detach();
}