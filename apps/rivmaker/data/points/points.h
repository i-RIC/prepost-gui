#ifndef POINTS_H
#define POINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class GeometryPoint;

class Points : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	Points(DataItem* parent);
	virtual ~Points();

	const std::vector<GeometryPoint*>& points() const;
	std::vector<GeometryPoint*>& points();

	virtual QString caption() const = 0;

	virtual void setPoints(const std::vector<GeometryPoint*>& points);
	void clearPoints();

	DataItemController* buildPreProcessorDataItemController(Model* model) override;

protected:
	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) const override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/points_impl.h"
#endif // _DEBUG

#endif // POINTS_H
