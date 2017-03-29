#ifndef CROSSSECTIONPREPROCESSORVIEWLABELHELPER_H
#define CROSSSECTIONPREPROCESSORVIEWLABELHELPER_H

#include "../base/dataitemviewhelperi.h"

#include <QObject>

class CrossSectionPreProcessorViewLabelHelper : public QObject, public DataItemViewHelperI
{
	Q_OBJECT

public:
	CrossSectionPreProcessorViewLabelHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // CROSSSECTIONPREPROCESSORVIEWLABELHELPER_H
