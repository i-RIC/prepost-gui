#include "geodatariversurvey_editcrosssectioncommand.h"
#include "../geodatariversurveycrosssectionwindow.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>

GeoDataRiverSurvey::EditCrosssectionCommand::EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, int after_sel, const GeoDataRiverCrosssection::AltitudeList& before, int before_sel, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction, QUndoCommand* parentcommand) :
	QUndoCommand(title, parentcommand),
	m_apply {apply},
	m_first {true},
	m_tableaction {tableaction},
	m_point {p},
	m_before (before),
	m_beforeSelection {before_sel},
	m_after (after),
	m_afterSelection {after_sel},
	m_window {w},
	m_groupDataItem {w->groupDataItem()},
	m_rs {rs}
{}

void GeoDataRiverSurvey::EditCrosssectionCommand::redo()
{
	m_point->crosssection().AltitudeInfo() = m_after;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (m_apply || (m_tableaction && m_first)) {
		m_window->updateView();
	} else {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_groupDataItem->updateCrossectionWindows();
		m_rs->setMapped(false);
	}
	m_first = false;
	if (m_afterSelection != NO_SEL) {
		m_window->setSelectedRow(m_afterSelection);
	}
}

void GeoDataRiverSurvey::EditCrosssectionCommand::undo()
{
	m_point->crosssection().AltitudeInfo() = m_before;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (! m_apply) {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_groupDataItem->updateCrossectionWindows();
	}
	if (m_beforeSelection != NO_SEL) {
		m_window->setSelectedRow(m_beforeSelection);
	}
}
