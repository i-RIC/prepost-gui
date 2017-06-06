#include "arbitraryhwm.h"
#include "arbitraryhwmpreprocessorview.h"
#include "arbitraryhwmpreprocessorviewhelper.h"

ArbitraryHWMPreProcessorView::ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ArbitraryHWMPreProcessorViewHelper(this));
}

ArbitraryHWMPreProcessorView::~ArbitraryHWMPreProcessorView()
{}
