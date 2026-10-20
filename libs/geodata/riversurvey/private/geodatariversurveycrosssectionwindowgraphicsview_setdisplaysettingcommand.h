#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_SETDISPLAYSETTINGCOMMAND_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_SETDISPLAYSETTINGCOMMAND_H

#include "../geodatariversurveycrosssectiondisplaysetting.h"
#include "../geodatariversurveycrosssectionwindowgraphicsview.h"

#include <QPointer>
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

	// the window may be closed while this command remains in the undo stack
	QPointer<GeoDataRiverSurveyCrosssectionWindowGraphicsView> m_view;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWGRAPHICSVIEW_SETDISPLAYSETTINGCOMMAND_H
