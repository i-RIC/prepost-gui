#ifndef BASELINEPREPROCESSORVIEW_H
#define BASELINEPREPROCESSORVIEW_H

#include "../base/dataitemview.h"

class BaseLine;

class BaseLinePreProcessorView : public DataItemView
{
public:
	BaseLinePreProcessorView(Model* model, BaseLine* item);
	~BaseLinePreProcessorView();

private:
	QRectF doBoundingBox() const override;
};

#endif // BASELINEPREPROCESSORVIEW_H
