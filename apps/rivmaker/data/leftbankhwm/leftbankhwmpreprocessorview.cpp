#include "leftbankhwm.h"
#include "leftbankhwmpreprocessorview.h"
#include "leftbankhwmpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

LeftBankHWMPreProcessorView::LeftBankHWMPreProcessorView(Model* model, LeftBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new LeftBankHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

LeftBankHWMPreProcessorView::~LeftBankHWMPreProcessorView()
{}
