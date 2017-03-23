#ifndef POINTS_H
#define POINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class QVector3D;

class Points : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	Points(DataItem* parent);
	virtual ~Points();

	const std::vector<QVector3D*>& points() const;
	std::vector<QVector3D*>& points();

	void setPoints(const std::vector<QVector3D*>& points);
	void clearPoints();

	DataItemController* buildPreProcessorDataItemController(Model* model) override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/points_impl.h"
#endif // _DEBUG

#endif // POINTS_H
