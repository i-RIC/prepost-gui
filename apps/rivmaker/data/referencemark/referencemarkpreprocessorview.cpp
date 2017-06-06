#include "referencemark.h"
#include "referencemarkpreprocessorview.h"
#include "referencemarkpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

ReferenceMarkPreProcessorView::ReferenceMarkPreProcessorView(Model* model, ReferenceMark* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ReferenceMarkPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

ReferenceMarkPreProcessorView::~ReferenceMarkPreProcessorView()
{}
