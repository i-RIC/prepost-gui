#include "hub.h"
#include "hubpreprocessorview.h"
#include "hubpreprocessorviewhelper.h"

HubPreProcessorView::HubPreProcessorView(Model* model, Hub* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new HubPreProcessorViewHelper(this));
}

HubPreProcessorView::~HubPreProcessorView()
{}
