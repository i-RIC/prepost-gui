#include "geodatariversurveycrosssectionwindowgraphicsview_setdisplaysettingcommand.h"

GeoDataRiverSurveyCrosssectionWindowGraphicsView::SetDisplaySettingCommand::SetDisplaySettingCommand(const GeoDataRiverSurveyCrossSectionDisplaySetting& setting, GeoDataRiverSurveyCrosssectionWindowGraphicsView* view) :
	QUndoCommand {GeoDataRiverSurveyCrosssectionWindowGraphicsView::tr("Edit Display Setting")},
	m_newSetting {setting},
	m_oldSetting {view->m_displaySetting},
	m_view {view}
{}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::SetDisplaySettingCommand::redo()
{
	m_view->m_displaySetting = m_newSetting;
	emit m_view->displaySettingChanged(m_view->m_displaySetting);
	m_view->viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::SetDisplaySettingCommand::undo()
{
	m_view->m_displaySetting = m_oldSetting;
	emit m_view->displaySettingChanged(m_view->m_displaySetting);
	m_view->viewport()->update();
}
