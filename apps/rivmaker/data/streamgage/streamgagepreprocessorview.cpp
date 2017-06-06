#include "streamgage.h"
#include "streamgagepreprocessorview.h"
#include "streamgagepreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

StreamGagePreProcessorView::StreamGagePreProcessorView(Model* model, StreamGage* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new StreamGagePreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

StreamGagePreProcessorView::~StreamGagePreProcessorView()
{}
