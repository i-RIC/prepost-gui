#ifndef RAWDATARIVERSURVEYCREATOR_H
#define RAWDATARIVERSURVEYCREATOR_H

#include "rd_riversurvey_global.h"
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiont.h>

class RD_RIVERSURVEY_EXPORT RawDataRiverSurveyCreator : public RawDataCreator
{
	Q_OBJECT
public:
	/// Constructor
	RawDataRiverSurveyCreator();
	bool isCompatibleWith(SolverDefinitionGridRelatedCondition* condition);
	QString name(unsigned int index);
	QString defaultCaption(unsigned int index);
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridRelatedCondition* condition);
	virtual RawData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridRelatedCondition* condition);
};

#endif // RAWDATARIVERSURVEYCREATOR_H
