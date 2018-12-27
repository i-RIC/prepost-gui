#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITCOLORCOMMAND_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITCOLORCOMMAND_H

#include "../hydraulicdatariversurveywaterelevation.h"

#include <QColor>
#include <QUndoCommand>

class HydraulicDataRiverSurveyWaterElevation::EditColorCommand : public QUndoCommand
{
public:
	EditColorCommand(const QColor& color, HydraulicDataRiverSurveyWaterElevation* we);

	void redo() override;
	void undo() override;

private:
	QColor m_newColor;
	QColor m_oldColor;

	HydraulicDataRiverSurveyWaterElevation* m_we;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITCOLORCOMMAND_H
