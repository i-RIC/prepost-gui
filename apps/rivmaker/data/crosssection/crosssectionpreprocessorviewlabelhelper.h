#ifndef CROSSSECTIONPREPROCESSORVIEWLABELHELPER_H
#define CROSSSECTIONPREPROCESSORVIEWLABELHELPER_H

#include "../base/dataitemviewhelperi.h"

class CrossSectionPreProcessorViewLabelHelper : public DataItemViewHelperI
{
public:
	CrossSectionPreProcessorViewLabelHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // CROSSSECTIONPREPROCESSORVIEWLABELHELPER_H
