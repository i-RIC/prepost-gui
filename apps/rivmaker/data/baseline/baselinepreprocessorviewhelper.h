#ifndef BASELINEPREPROCESSORVIEWHELPER_H
#define BASELINEPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class BaseLinePreProcessorViewHelper : public DataItemViewHelperI
{
public:
	BaseLinePreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // BASELINEPREPROCESSORVIEWHELPER_H
