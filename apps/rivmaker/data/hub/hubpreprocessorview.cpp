#include "hub.h"
#include "hubpreprocessorview.h"
#include "hubpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

HubPreProcessorView::HubPreProcessorView(Model* model, Hub* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new HubPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

HubPreProcessorView::~HubPreProcessorView()
{}
