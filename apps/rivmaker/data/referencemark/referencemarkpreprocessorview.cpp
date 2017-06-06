#include "referencemark.h"
#include "referencemarkpreprocessorview.h"
#include "referencemarkpreprocessorviewhelper.h"

ReferenceMarkPreProcessorView::ReferenceMarkPreProcessorView(Model* model, ReferenceMark* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ReferenceMarkPreProcessorViewHelper(this));
}

ReferenceMarkPreProcessorView::~ReferenceMarkPreProcessorView()
{}
