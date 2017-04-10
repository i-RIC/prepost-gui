#ifndef CROSSSECTIONPREPROCESSORVIEW_H
#define CROSSSECTIONPREPROCESSORVIEW_H

#include "../base/dataitemview.h"

class CrossSection;

class CrossSectionPreProcessorView : public DataItemView
{
public:
	CrossSectionPreProcessorView(Model* model, CrossSection* item);
	~CrossSectionPreProcessorView();

private:
	QRectF doBoundingBox() const override;
};

#endif // CROSSSECTIONPREPROCESSORVIEW_H
