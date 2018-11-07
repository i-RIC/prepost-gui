#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITCAPTIONCOMMAND_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITCAPTIONCOMMAND_H

#include "../hydraulicdatariversurveywaterelevation.h"

#include <QUndoCommand>

class HydraulicDataRiverSurveyWaterElevation::EditCaptionCommand : public QUndoCommand
{
public:
	EditCaptionCommand(const QString& caption, HydraulicDataRiverSurveyWaterElevation* we);

	void redo() override;
	void undo() override;

private:
	QString m_newCaption;
	QString m_oldCaption;

	HydraulicDataRiverSurveyWaterElevation* m_we;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATION_EDITCAPTIONCOMMAND_H
