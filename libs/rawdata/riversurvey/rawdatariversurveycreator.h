#ifndef RAWDATARIVERSURVEYCREATOR_H
#define RAWDATARIVERSURVEYCREATOR_H

#include "rd_riversurvey_global.h"
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <guicore/solverdef/solverdefinitiongridattributet.h>

class RD_RIVERSURVEY_EXPORT RawDataRiverSurveyCreator : public RawDataCreator
{
	Q_OBJECT

public:
	/// Constructor
	RawDataRiverSurveyCreator();
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override;
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	virtual RawData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // RAWDATARIVERSURVEYCREATOR_H
