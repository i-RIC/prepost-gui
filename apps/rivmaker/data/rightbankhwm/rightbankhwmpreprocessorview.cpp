#include "rightbankhwm.h"
#include "rightbankhwmpreprocessorview.h"
#include "rightbankhwmpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

RightBankHWMPreProcessorView::RightBankHWMPreProcessorView(Model* model, RightBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new RightBankHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

RightBankHWMPreProcessorView::~RightBankHWMPreProcessorView()
{}
