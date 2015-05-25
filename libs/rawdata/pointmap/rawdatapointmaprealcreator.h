#ifndef RAWDATAPOINTMAPREALCREATOR_H
#define RAWDATAPOINTMAPREALCREATOR_H

#include "rd_pointmap_global.h"
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiont.h>

class RD_POINTMAP_EXPORT RawDataPointmapRealCreator : public RawDataCreator
{
	Q_OBJECT
public:
	/// Constructor
	RawDataPointmapRealCreator();
	bool isCompatibleWith(SolverDefinitionGridRelatedCondition* condition) override;
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridRelatedCondition* condition) override;
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
};

#endif // RAWDATAPOINTMAPREALCREATOR_H
