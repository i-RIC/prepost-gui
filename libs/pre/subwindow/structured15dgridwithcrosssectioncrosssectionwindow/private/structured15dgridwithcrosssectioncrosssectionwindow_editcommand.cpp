#include "../../../datamodel/preprocessornormal15dgridwithcrosssectionshapedataitem.h"
#include "structured15dgridwithcrosssectioncrosssectionwindow_editcommand.h"

Structured15DGridWithCrossSectionCrossSectionWindow::EditCommand::EditCommand(bool apply, const QString& title, const std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude>& after, const std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude>& before, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, bool tableaction) :
	QUndoCommand(title),
	m_apply {apply},
	m_first {true},
	m_tableaction {tableaction},
	m_before {before},
	m_after {after},
	m_window {w},
	m_item {item}
{}

void Structured15DGridWithCrossSectionCrossSectionWindow::EditCommand::redo()
{
	m_window->m_blackLineCrossSection->altitudeInfo() = m_after;
	if (m_apply || (m_tableaction && m_first)) {
		m_window->updateView();
	} else {
		m_item->updateCrossSectionWindows();
	}
	m_first = false;
}

void Structured15DGridWithCrossSectionCrossSectionWindow::EditCommand::undo()
{
	m_window->m_blackLineCrossSection->setAltitudeInfo(m_before);

	if (! m_apply) {
		m_item->updateCrossSectionWindows();
	}
}
