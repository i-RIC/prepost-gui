#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_SETDISPLAYSETTINGCOMMAND_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_SETDISPLAYSETTINGCOMMAND_H

#include "../geodatariversurveycrosssectiondisplaysetting.h"
#include "../geodatariversurveycrosssectionwindowgraphicsview.h"

#include <QUndoCommand>

class GeoDataRiverSurveyCrosssectionWindowGraphicsView::SetDisplaySettingCommand : public QUndoCommand
{
public:
	SetDisplaySettingCommand(const GeoDataRiverSurveyCrossSectionDisplaySetting& setting, GeoDataRiverSurveyCrosssectionWindowGraphicsView* view);

	void redo() override;
	void undo() override;

private:
	GeoDataRiverSurveyCrossSectionDisplaySetting m_newSetting;
	GeoDataRiverSurveyCrossSectionDisplaySetting m_oldSetting;

	GeoDataRiverSurveyCrosssectionWindowGraphicsView* m_view;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_SETDISPLAYSETTINGCOMMAND_H
