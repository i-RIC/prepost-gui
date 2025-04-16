#include "geodatariversurvey_editcrosssectioncommand.h"
#include "../geodatariversurveycrosssectionwindow.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <misc/qundocommandhelper.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>

GeoDataRiverSurvey::EditCrosssectionCommand::EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, int after_sel, const GeoDataRiverCrosssection::AltitudeList& before, int before_sel, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction, QUndoCommand* parentcommand) :
	QUndoCommand(title, parentcommand),
	m_apply {apply},
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
	if (m_apply) {
		m_window->updateView();
	} else {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_groupDataItem->updateCrossectionWindows();
		m_rs->setMapped(false);
	}
	if (m_afterSelection != NO_SEL) {
		m_window->setSelectedRow(m_afterSelection);
	}
}

void GeoDataRiverSurvey::EditCrosssectionCommand::undo()
{
	m_point->crosssection().AltitudeInfo() = m_before;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (m_apply) {
		m_window->updateView();
	} else {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_groupDataItem->updateCrossectionWindows();
	}
	if (m_beforeSelection != NO_SEL) {
		m_window->setSelectedRow(m_beforeSelection);
	}
}

int GeoDataRiverSurvey::EditCrosssectionCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurvey::EditCrosssectionCommand");
}

bool GeoDataRiverSurvey::EditCrosssectionCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto com = dynamic_cast<const EditCrosssectionCommand*> (other);
	if (com == nullptr) {return false;}

	if (m_point != com->m_point) {return false;}
	if (m_rs != com->m_rs) {return false;}
	if (m_window != com->m_window) {return false;}

	m_apply = com->m_apply;
	m_after = com->m_after;
	return true;
}
