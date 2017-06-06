#include "streamgage.h"
#include "streamgagepreprocessorview.h"
#include "streamgagepreprocessorviewhelper.h"

StreamGagePreProcessorView::StreamGagePreProcessorView(Model* model, StreamGage* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new StreamGagePreProcessorViewHelper(this));
}

StreamGagePreProcessorView::~StreamGagePreProcessorView()
{}
