// The top of every source code file must include this line
#include "sierrachart.h"

/*****************************************************************************

	For reference, please refer to the Advanced Custom Study Interface
	and Language documentation on the Sierra Chart website. 
	
*****************************************************************************/

// Name of the custom study.
//SCDLLName("Intraday Intensity Oscillator With Filters") 

//Custom study starts here.
SCSFExport scsf_IntradayIntensityOscillatorWithFilters(SCStudyGraphRef sc)
{
	//Aliases to make things easier to read and maintain.
	SCSubgraphRef ZeroLine = sc.Subgraph[0];
	SCSubgraphRef Graph_IIO = sc.Subgraph[1];
	SCInputRef In_IIOPeriod = sc.Input[0];
	SCInputRef In_VolumeFilterType = sc.Input[1];
	SCInputRef In_MAClipSmoothingPeriod = sc.Input[2];
	SCInputRef In_MAClipSmoothingMAType = sc.Input[3];
	SCInputRef In_IIOSetSmoothing = sc.Input[4];
	SCInputRef In_IIOSmoothingPeriod = sc.Input[5];
	SCInputRef In_IIOSmoothingMAType = sc.Input[6];
	SCSubgraphRef II = sc.Subgraph[2];
	SCSubgraphRef IIO = sc.Subgraph[3];
	SCSubgraphRef VolumeMA = sc.Subgraph[4];
	SCSubgraphRef VolumeFiltered = sc.Subgraph[5];
	//SCFloatArrayRef FloatArray = Graph_IIO.Arrays[0];

	//remember local variables are not persistent.
	int i, j, k, Period, IndexTemp;
	float RunningTotalII, RunningTotalVolume;
	
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		//Long descriptive name.
		sc.GraphName = "Intraday Intensity Oscillator With Filters";
		
		sc.StudyVersion = 1.0;
		
		// During development set this flag to 1, so the DLL can be rebuilt without restarting Sierra Chart. When development is completed, set it to 0 to improve performance.
		sc.FreeDLL = 0;
		
		//Data for the "Display Study Documentation" button
sc.StudyDescription = "Intraday Intensity Oscillator With Filters. A price and volume based indicator that attempts to estimate accumulation and distribution of institutional traders. Normal ranges are +/-0.3. Spikes can go higher. Filtering can changes these values some.<p>If you have tick by tick volume up/down data, use the \"By Tick\" version.<p>This version includes extras to help clean up the graph and make it easier to integrate with automation. The \"Set IIO Smoothing?\" option enables cleaning up some of the IIO noise after the calculation. It is recommended to smooth IIO out with an EMA 3-7.<p>This version has a Volume Filter option to handle cases when a volume spike smashes the indicator making it hard to read. Large institutions sometimes do this to hide their activity afterward. The \"Straight\" option passes volume data through without any filtering. The \"Log\" option runs each volume bar through the log function making a type of \"log scale\". The \"Square Root\" option runs each volume bar through the square root function making a type of \"square root scale\". The \"MA Clip\" option will clip volume above a chosen moving average. To learn the movement characteristics of each, it is recommended to put them all on one chart for equal comparison.<p>To use traditional IIO, turn off all smoothing and filtering options.";
		
		//sc.GraphRegion = 1; //not main price graph, let it auto-select.

		sc.AutoLoop = 1;  //Auto looping is enabled. 
		
		//Output Graph
		Graph_IIO.Name = "IIO"; //drawn graphs must have a name.
		Graph_IIO.PrimaryColor = RGB(0, 255, 0);
		Graph_IIO.DrawStyle = DRAWSTYLE_LINE;
		Graph_IIO.LineStyle = LINESTYLE_SOLID; //if LineWidth>1, lines will appear solid
		Graph_IIO.LineWidth = 2;
		Graph_IIO.DrawZeros = 0;
		
		ZeroLine.Name="Zero";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.LineStyle = LINESTYLE_DOT;
		ZeroLine.LineWidth = 1;
		ZeroLine.PrimaryColor = RGB(128, 128, 128);;
		ZeroLine.DisplayNameValueInWindowsFlags = 0;
		ZeroLine.DrawZeros = 1;
		
		//User Inputs
		
		In_IIOPeriod.Name = "IIO Period";
		In_IIOPeriod.SetInt(20);
		In_IIOPeriod.SetIntLimits(3, 300);
		In_IIOPeriod.SetDescription("Number of bars for the period. Useful values are 20 and higher.");
		
		In_VolumeFilterType.Name = "Volume Filtering Type";
		In_VolumeFilterType.SetCustomInputIndex(0);
		In_VolumeFilterType.SetCustomInputStrings("Straight;Log;Square Root;MA Clip");
		In_VolumeFilterType.SetDescription("\"Straight\" for no filtering and normal calculations. \"Log\" for applying log(Volume). \"Square Root\" for applying sqrt(Volume). \"MA Clip\" for clipping the volume above the chosen moving average.");
		
		In_MAClipSmoothingPeriod.Name = "MA Clip Smoothing Period";
		In_MAClipSmoothingPeriod.SetInt(3);
		In_MAClipSmoothingPeriod.SetIntLimits(2, 100);
		In_MAClipSmoothingPeriod.SetDescription("Only if MA Clip is selected. Number of bars for the period. Usually 3-30.");
		
		In_MAClipSmoothingMAType.Name = "MA Clip Moving Average Type";
		In_MAClipSmoothingMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		In_MAClipSmoothingMAType.SetDescription("Only if MA Clip is selected. Usually EMA, but others can be experimented with.");	
		
		In_IIOSetSmoothing.Name = "Set IIO Smoothing?";
		In_IIOSetSmoothing.SetYesNo(1);
		In_IIOSetSmoothing.SetDescription("Enables extended IIO smoothing options.");
		
		In_IIOSmoothingPeriod.Name = "IIO Smoothing Period";
		In_IIOSmoothingPeriod.SetInt(7);
		In_IIOSmoothingPeriod.SetIntLimits(2, 50);
		In_IIOSmoothingPeriod.SetDescription("Number of bars for the period. Usually 3-7.");
		
		In_IIOSmoothingMAType.Name = "IIO Smoothing Moving Average Type";
		In_IIOSmoothingMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		In_IIOSmoothingMAType.SetDescription("Usually EMA, but others can be experimented with.");
		
		//Keep alerts from going crazy.
		sc.AlertOnlyOncePerBar = true;
		
		//Set printed output value format. Number is the index of the selected format.
		//sc.ValueFormat = 3;  // 3 decimal places
		sc.ValueFormat = 3;
		
		return;
		}
	
	// Section 2 - Do data processing here
	
	i = sc.Index; //easier to read traditional C/C++ index i in multiple arrays on one line.
	Period = In_IIOPeriod.GetInt(); //easier to read.
	
	//Handle the zero line.
	ZeroLine[i] = 0.0;
	
	//if first run
	if (i == 0) {
		// Set the index of the first array element to begin drawing at
		sc.DataStartIndex = Period - 1 - (In_IIOSetSmoothing.GetYesNo() ? In_IIOSmoothingPeriod.GetInt() : 0.0);
		}

	//Handle volume filtering.
	if(In_VolumeFilterType.GetIndex() == 0) { // 0: straight
		VolumeFiltered[i] = (float)sc.Volume[i];
		}
	else if(In_VolumeFilterType.GetIndex() == 1) { // 1: log
		VolumeFiltered[i] = log( (float)sc.Volume[i] < 1.0 ? 1.0 : (float)sc.Volume[i] );
		}
	else if(In_VolumeFilterType.GetIndex() == 2) { // 2: sqrt
		VolumeFiltered[i] = sqrt( (float)sc.Volume[i] < 1.0 ? 1.0 : (float)sc.Volume[i] );
		}
	else { // 3: MA Clip
		sc.MovingAverage(sc.Volume, VolumeMA, In_MAClipSmoothingMAType.GetMovAvgType(), In_MAClipSmoothingPeriod.GetInt() );
		if((float)sc.Volume[i] <= VolumeMA[i]) {VolumeFiltered[i] = (float)sc.Volume[i];} //handle normal volume
		else {VolumeFiltered[i] = VolumeMA[i];} //handle clipped volume
		}

	//Generate Intraday Intensity values.
	//Handle divide by 0 problems.
	if( (VolumeFiltered[i] < 0.01) || (sc.High[i] == sc.Low[i]) ) {II[i] = 0.0;}
	//else calculate as normal.
	//Analysis: ScalingFactor * Volume. If close is in the middle, ScalingFactor=0.0.
	//If close is at the top, ScalingFactor=1.0. If close is at the bottom, ScalingFactor=-1.0.
	else {II[i] = (((2.0 * sc.Close[i]) - sc.High[i] - sc.Low[i]) / (sc.High[i] - sc.Low[i])) * VolumeFiltered[i];}
	
	//Not enough data yet for the IIO calculation.
	if( i < Period ) {return;}
	
	//do the inner loop
	for(j=i-Period+1; j<=i; j++) {
		RunningTotalII = 0.0;
		RunningTotalVolume = 0.0;
		IndexTemp = j - Period +1;
		if(IndexTemp < 0) {IndexTemp = 0;}
		for(k=IndexTemp; k<=j; k++) {
			RunningTotalII += II[k];
			//Safety: Under some very rare circumstances, a volume moving average number may be negative.
			//Filter those out so the indicator doesn't get trashed.
			if(VolumeFiltered[k] > 0.0) {RunningTotalVolume += VolumeFiltered[k];}
			}
		}
	
	//Handle divide by 0.
	//Analysis: ScaledVolume/TotalVolume for the period.
	if(RunningTotalVolume > 0.0) {IIO[i] = RunningTotalII / RunningTotalVolume;}
	else {IIO[i] = 0.0;}
	
	if( In_IIOSetSmoothing.GetYesNo() ) {
		sc.MovingAverage(IIO, Graph_IIO, In_IIOSmoothingMAType.GetMovAvgType(), In_IIOSmoothingPeriod.GetInt() );
		}
	else {
		Graph_IIO[i] = IIO[i];
		}	
}
