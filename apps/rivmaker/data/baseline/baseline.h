#ifndef BASELINE_H
#define BASELINE_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <QVector2D>

#include <vector>

class CrossSection;

class QPointF;
class BaseLine : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	BaseLine(DataItem* parent);
	~BaseLine();

	const std::vector<QPointF>& polyLine() const;
	void setPolyLine(const std::vector<QPointF>& line);

	void getCrossingPoint(CrossSection* cs, bool* crosses, double* x, double* y, double* pos);

	void reverseDirection();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemController* buildPreProcessorDataItemController(Model*) override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/baseline_impl.h"
#endif // _DEBUG

#endif // BASELINE_H
