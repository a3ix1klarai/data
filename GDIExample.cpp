#include "sierrachart.h"

// SCDLLName("GDI Example")

// This file demonstrates the functionality to use the Windows Graphics Device
// Interface (GDI) with ACSIL to freely draw inside of the chart window

// Windows GDI documentation can be found here: 
// http://msdn.microsoft.com/en-nz/library/windows/desktop/dd145203%28v=vs.85%29.aspx

/*==========================================================================*/
// Drawing function declaration
void DrawToChart(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef sc); 


/*==========================================================================*/
SCSFExport scsf_DrawToChartExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Draw To Chart Example";

		sc.AutoLoop = 0;
		return;
	}


	// This is where we specify the drawing function. This function will be called
	// when the study graph is drawn on the chart. We are placing this after
	// if (sc.SetDefaults). So in case the study DLL is unloaded and reloaded,
	// this will continue to be set to the correct address.
	sc.p_GDIFunction = DrawToChart;


}
/*==========================================================================*/

// This is the actual drawing function. This function is specified by the
// "sc.p_GDIFunction" member in the main study function above. This drawing
// function is called when Sierra Chart draws the study on the chart. This
// will only occur after there has been a call to the main "scsf_" study
// function which is defined above.

// This drawing function has access to the ACSIL "sc." structure.
// However, any changes to the variable members will have no effect.

void DrawToChart(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef sc )
{
	//Create a Yellow brush
	HBRUSH Brush = CreateSolidBrush(RGB(255,255,0));

	//Select the  brush into the device context
	HGDIOBJ PriorBrush = SelectObject(DeviceContext, Brush);

	//Draw a rectangle at the top left of the chart
	Rectangle(DeviceContext, sc.StudyRegionLeftCoordinate + 5, sc.StudyRegionTopCoordinate + 5, sc.StudyRegionLeftCoordinate + 200, sc.StudyRegionTopCoordinate + 200);

	int RightCoordinate = sc.StudyRegionRightCoordinate;

	//Remove the brush from the device context and put the prior brush back in. This is critical!
	SelectObject(DeviceContext,PriorBrush);

	//Delete the brush.  This is critical!  If you do not do this, you will end up with
	// a GDI leak and crash Sierra Chart.
	DeleteObject(Brush);

	::SetTextAlign(DeviceContext, TA_NOUPDATECP);

	//Must use the wide character version
	::TextOutW(DeviceContext, 250, 250, L"Hello.", 6);

	// draw a line
	
	sc.DrawGraphics_MoveTo(0, 0);
	sc.DrawGraphics_LineTo(300, 300, COLOR_CYAN, 5, LINESTYLE_SOLID);

	return;
}

/*==========================================================================*/

