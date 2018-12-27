#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITITEMCOMMAND_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITITEMCOMMAND_H

#include "../hydraulicdatariversurveywaterelevation.h"

#include <QString>
#include <QUndoCommand>

class HydraulicDataRiverSurveyWaterElevation::EditItemCommand : public QUndoCommand
{
public:
	EditItemCommand(const QString& name, bool specified, double value, HydraulicDataRiverSurveyWaterElevation* we);

	void redo() override;
	void undo() override;

private:
	bool m_newSpecified;
	double m_newValue;

	bool m_oldSpecified;
	double m_oldValue;

	bool m_isNew;
	QString m_name;
	HydraulicDataRiverSurveyWaterElevation* m_we;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITITEMCOMMAND_H
