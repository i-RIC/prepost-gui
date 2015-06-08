#include "postexportsetting.h"

PostExportSetting::PostExportSetting()
{
	allSteps = true;
	startStep = 0;
	endStep = 0;
	skipRate = 1;

	fullRange = true;
	iMin = 0;
	iMax = 0;
	jMin = 0;
	jMax = 0;
	kMin = 0;
	kMax = 0;

	folder = "";
	prefix = "Result_";
}

PostExportSetting::~PostExportSetting()
{

}

