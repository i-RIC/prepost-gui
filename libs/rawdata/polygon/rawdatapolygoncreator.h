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
	QString name(unsigned int index);
	QString defaultCaption(unsigned int index);
	virtual bool isCreatable(){return true;}
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridRelatedCondition* condition);
};

#endif // RAWDATAPOLYGONCREATOR_H
