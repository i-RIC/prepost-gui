#include "rightbankhwmpreprocessorview.h"
#include "rightbankhwmpreprocessorviewhelper.h"

RightBankHWMPreProcessorView::RightBankHWMPreProcessorView(Model* model, RightBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new RightBankHWMPreProcessorViewHelper(this));
}

RightBankHWMPreProcessorView::~RightBankHWMPreProcessorView()
{}
