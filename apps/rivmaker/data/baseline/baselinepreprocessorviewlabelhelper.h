#ifndef BASELINEPREPROCESSORVIEWLABELHELPER_H
#define BASELINEPREPROCESSORVIEWLABELHELPER_H

#include "../base/dataitemviewhelperi.h"
#include <QObject>

class BaseLinePreProcessorViewLabelHelper : public QObject, public DataItemViewHelperI
{
	Q_OBJECT

public:
	BaseLinePreProcessorViewLabelHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // BASELINEPREPROCESSORVIEWLABELHELPER_H
