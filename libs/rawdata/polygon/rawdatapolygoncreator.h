#ifndef RAWDATAPOLYGONCREATOR_H
#define RAWDATAPOLYGONCREATOR_H

#include "rd_polygon_global.h"
#include <guicore/pre/rawdata/rawdatacreator.h>

class RD_POLYGON_EXPORT RawDataPolygonCreator : public RawDataCreator
{
	Q_OBJECT
public:
	/// Constructor
	RawDataPolygonCreator();
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
	virtual bool isCreatable() const override {return true;}
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // RAWDATAPOLYGONCREATOR_H
