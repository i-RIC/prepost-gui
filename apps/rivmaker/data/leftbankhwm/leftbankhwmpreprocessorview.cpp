#include "leftbankhwmpreprocessorview.h"
#include "leftbankhwmpreprocessorviewhelper.h"

LeftBankHWMPreProcessorView::LeftBankHWMPreProcessorView(Model* model, LeftBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new LeftBankHWMPreProcessorViewHelper(this));
}

LeftBankHWMPreProcessorView::~LeftBankHWMPreProcessorView()
{}
