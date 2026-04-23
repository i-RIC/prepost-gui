#include "geodatariversurvey_editcrosssectioncommand.h"
#include "../geodatariversurveycrosssectionwindow.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <misc/qundocommandhelper.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>

GeoDataRiverSurvey::EditCrosssectionCommand::EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, int after_sel, const GeoDataRiverCrosssection::AltitudeList& before, int before_sel, const GeoDataRiverPathPointOdnData& odn, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction, QUndoCommand* parentcommand) :
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
{
	m_beforeOdn = odn;
	m_afterOdn = odn;

	std::vector<double> posvec;
	for (const auto& alt : after) {
		posvec.push_back(alt.position());
	}

	for (int i = 0; i < 6; ++i) {
		int index = m_beforeOdn.nb(i);
		if (index < 0 || index >= static_cast<int> (posvec.size())) {
			if (i < 3) {
				index = 0;
			} else {
				index = before.size() - 1;
			}
		}
		int index2 = index + after.size() - before.size();

		auto odnPos = before.at(index).position();
		if (index >= 0 && index < static_cast<int> (posvec.size()) && posvec[index] == odnPos) {
			m_afterOdn.setNb(i, index);
		} else if (index2 >= 0 && index2 < static_cast<int> (posvec.size()) && posvec[index2] == odnPos) {
			m_afterOdn.setNb(i, index2);
		} else {
			auto it = std::lower_bound(posvec.begin(), posvec.end(), odnPos);
			if (*it == odnPos) {
				m_afterOdn.setNb(i, it - posvec.begin());
			} else {
				if (i < 3) {
					m_afterOdn.setNb(i, 0);
				} else {
					m_afterOdn.setNb(i, posvec.size() - 1);
				}
			}
		}
	}
}

void GeoDataRiverSurvey::EditCrosssectionCommand::redo()
{
	m_point->crosssection().AltitudeInfo() = m_after;
	m_point->odn() = m_afterOdn;
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
	m_point->odn() = m_beforeOdn;
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
	m_afterOdn = com->m_afterOdn;

	return true;
}
