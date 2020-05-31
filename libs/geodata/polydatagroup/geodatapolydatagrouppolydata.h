#ifndef GEODATAPOLYDATAGROUPPOLYDATA_H
#define GEODATAPOLYDATAGROUPPOLYDATA_H

#include "gd_polydatagroup_global.h"

#include <QObject>

class GeoDataPolyData;
class GeoDataPolyDataGroup;

class GD_POLYDATAGROUP_EXPORT GeoDataPolyDataGroupPolyData : public QObject
{
	Q_OBJECT

public:
	GeoDataPolyDataGroupPolyData(GeoDataPolyDataGroup* group);
	virtual ~GeoDataPolyDataGroupPolyData();

	GeoDataPolyDataGroup* group() const;

	QString id() const;
	void setId(const QString& id);

	QString name() const;
	void setName(const QString& name);

	QVariant value() const;
	void setValue(const QVariant& v);

	unsigned int order() const;
	void setOrder(unsigned int order);

	unsigned int indexOffset() const;
	void setIndexOffset(unsigned int offset);

	virtual void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) = 0;
	virtual void copyShapeFrom(GeoDataPolyData* data) = 0;
	virtual void copyShapeTo(GeoDataPolyData* data) = 0;

	virtual void loadExternalData(QDataStream* stream) = 0;
	virtual void saveExternalData(QDataStream* stream) = 0;

	virtual void applyOffset(double x, double y) = 0;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/geodatapolydatagrouppolydata_impl.h"
#endif

#endif // GEODATAPOLYDATAGROUPPOLYDATA_H
