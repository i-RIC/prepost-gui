#ifndef STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_EDITCOMMAND_H
#define STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_EDITCOMMAND_H

#include "../structured15dgridwithcrosssectioncrosssectionwindow.h"

#include <QUndoCommand>

class Structured15DGridWithCrossSectionCrossSectionWindow::EditCommand : public QUndoCommand
{
public:
	EditCommand(bool apply, const QString& title, const std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude>& after, const std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude>& before, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, bool tableaction = false);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	bool m_first;
	bool m_tableaction;

	std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude> m_before;
	std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude> m_after;

	Structured15DGridWithCrossSectionCrossSectionWindow* m_window;
	PreProcessorNormal15DGridWithCrossSectionShapeDataItem* m_item;
};

#endif // STRUCTURED15DGRIDWITHCROSSSECTIONCROSSSECTIONWINDOW_EDITCOMMAND_H
