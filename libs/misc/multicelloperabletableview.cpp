#include "multicelloperabletableview.h"

#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

MultiCellOperableTableView::MultiCellOperableTableView(QWidget* parent) :
	QTableView {parent}
{}

void MultiCellOperableTableView::keyPressEvent(QKeyEvent* event)
{
	QModelIndexList selectedRows = selectionModel()->selectedRows();
	auto ii = selectedIndexes();

	if(!selectedIndexes().isEmpty()){
		if (event->matches(QKeySequence::Copy)) {
			QString text;
			QItemSelectionRange range = selectionModel()->selection().first();
			for (auto i = range.top(); i <= range.bottom(); ++i) {
				QStringList rowContents;
				for (auto j = range.left(); j <= range.right(); ++j) {
					rowContents << model()->index(i, j).data().toString();
				}
				text += rowContents.join("\t");
				text += "\n";
			}
			QApplication::clipboard()->setText(text);
		} else if (event->matches(QKeySequence::Paste)) {
			QString text = QApplication::clipboard()->text();
			QStringList rowContents = text.split("\n", QString::SkipEmptyParts);

			QModelIndex initIndex = selectedIndexes().at(0);
			auto initRow = initIndex.row();
			auto initCol = initIndex.column();

			for (auto i = 0; i < rowContents.size(); ++i) {
				QStringList columnContents = rowContents.at(i).split("\t");
				for (auto j = 0; j < columnContents.size(); ++j) {
					model()->setData(model()->index(initRow + i, initCol + j), columnContents.at(j));
				}
			}
		}
		else
			QTableView::keyPressEvent(event);
	}
}
