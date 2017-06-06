#include "arbitraryhwm.h"
#include "arbitraryhwmpreprocessorview.h"
#include "arbitraryhwmpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

ArbitraryHWMPreProcessorView::ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ArbitraryHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

ArbitraryHWMPreProcessorView::~ArbitraryHWMPreProcessorView()
{}
