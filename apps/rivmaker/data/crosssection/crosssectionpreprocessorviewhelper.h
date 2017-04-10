#ifndef CROSSSECTIONPREPROCESSORVIEWHELPER_H
#define CROSSSECTIONPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class CrossSectionPreProcessorViewHelper : public DataItemViewHelperI
{
public:
	CrossSectionPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter *painter) const override;
};

#endif // CROSSSECTIONPREPROCESSORVIEWHELPER_H
