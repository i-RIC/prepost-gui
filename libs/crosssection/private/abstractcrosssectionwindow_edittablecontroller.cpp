#include "../public/abstractcrosssectionwindow_controller.h"
#include "../public/abstractcrosssectionwindow_graphicsview.h"
#include "abstractcrosssectionwindow_edittablecontroller.h"
#include "abstractcrosssectionwindow_edittablecontroller_delegate.h"
#include "abstractcrosssectionwindow_impl.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/pre/gridcond/base/gridattributestringconverter.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QFile>
#include <QTextStream>

#include <vtkCellData.h>

AbstractCrosssectionWindow::EditTableController::EditTableController(QTableView* view, Impl* impl) :
	m_view {view},
	m_impl {impl},
	m_delegate {new Delegate {this}}
{
	view->setModel(&m_model);
	view->setEditTriggers(QTableView::AllEditTriggers);
	view->setItemDelegate(m_delegate);
}

const QStandardItemModel& AbstractCrosssectionWindow::EditTableController::model() const
{
	return m_model;
}

bool AbstractCrosssectionWindow::EditTableController::saveCsvFile(const QString& fileName)
{
	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	std::vector<GridAttributeStringConverter*> converters;

	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
		converters.push_back(s.attributeDataProvider()->createStringConverter(s.attributeName()));
	}

	QFile file(fileName);
	bool ok = file.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QTextStream stream(&file);
	QString originStr;
	if (m_impl->m_controller->targetDirection() == Direction::I) {
		originStr = "J = 1";
	} else if (m_impl->m_controller->targetDirection() == Direction::I) {
		originStr = "I = 1";
	}

	stream << AbstractCrosssectionWindow::tr("Index")
				 << "," << AbstractCrosssectionWindow::tr("Distance(m) from %1").arg(originStr);
	for (auto s : activeSettings) {
		auto attName = s->attributeName();
		stream << "," << s->attributeDataProvider()->caption(attName);
	}
	stream << "\n";

	auto nodePositions = m_impl->graphicsView()->setupNodePositions();
	for (int i = 0; i < m_model.rowCount(); ++i) {
		stream << (i + 1)
					 << "," << nodePositions.at(i);
		for (unsigned int j = 0; j < converters.size(); ++j) {
			auto converter = converters.at(j);
			QVariant v = m_model.data(m_model.index(i, j), Qt::EditRole);
			if (v.isNull()) {
				stream << ",";
			} else {
				stream << "," << converter->convert(v);
			}
		}
		stream << "\n";
	}

	for (auto c : converters) {
		delete c;
	}

	return true;
}

void AbstractCrosssectionWindow::EditTableController::applyToTable()
{
	auto controller = m_impl->m_controller;

	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
	}

	m_model.setColumnCount(static_cast<int> (activeSettings.size()));

	auto grid = m_impl->m_window->grid();
	if (grid == nullptr) {
		m_model.setRowCount(0);
		return;
	}

	int valueCount = 0;
	if (controller->targetDirection() == Direction::I) {
		valueCount = grid->dimensionJ();
	} else if (controller->targetDirection() == Direction::J) {
		valueCount = grid->dimensionI();
	}

	m_model.setRowCount(valueCount);

	QStringList labels;
	for (auto s : activeSettings) {
		auto attName = s->attributeName();
		labels.append(s->attributeDataProvider()->caption(attName));
	}

	m_model.setHorizontalHeaderLabels(labels);

	int col = 0;
	for (const auto& s : activeSettings) {
		setDataToModel(col, *s, valueCount);
		++ col;
	}
}

void AbstractCrosssectionWindow::EditTableController::setDataToModel(int col, const GridAttributeDisplaySettingContainer& s, int valueCount)
{
	auto controller = m_impl->m_controller;

	auto grid = s.grid;

	if (s.position() == GridAttributeDisplaySettingContainer::Position::Node) {
		auto array = grid->vtkData()->data()->GetPointData()->GetArray(s.attributeName().c_str());
		if (controller->targetDirection() == Direction::I) {
			int i = controller->targetIndex();
			if (i == -1) {return;}
			for (int j = 0; j < valueCount; ++j) {
				auto value = array->GetTuple1(grid->pointIndex(i, j));
				m_model.setData(m_model.index(j, col), value, Qt::EditRole);
			}
		} else if (controller->targetDirection() == Direction::J) {
			int j = controller->targetIndex();
			if (j == -1) {return;}
			for (int i = 0; i < valueCount; ++i) {
				auto value = array->GetTuple1(grid->pointIndex(i, j));
				m_model.setData(m_model.index(i, col), value, Qt::EditRole);
			}
		}
	} else if (s.position() == GridAttributeDisplaySettingContainer::Position::Cell) {
		auto array = grid->vtkData()->data()->GetCellData()->GetArray(s.attributeName().c_str());
		if (controller->targetDirection() == Direction::I) {
			int i = controller->targetIndex();
			if (i == -1) {return;}
			if (controller->cellSide() == Controller::CellSide::Previous) {
				-- i;
			}
			if (i >= 0 && i <= grid->dimensionI() - 2) {
				m_model.setData(m_model.index(0, col), QVariant::fromValue(nullptr), Qt::EditRole);
				for (int j = 0; j < valueCount - 1; ++j) {
					auto value = array->GetTuple1(grid->cellIndex(i, j));
					m_model.setData(m_model.index(j + 1, col), value, Qt::EditRole);
				}
			} else {
				for (int j = 0; j < valueCount; ++j) {
					m_model.setData(m_model.index(j, col), QVariant::fromValue(nullptr), Qt::EditRole);
				}
			}
		} else if (controller->targetDirection() == Direction::J) {
			int j = controller->targetIndex();
			if (j == -1) {return;}
			if (controller->cellSide() == Controller::CellSide::Previous) {
				-- j;
			}
			if (j >= 0 && j <= grid->dimensionJ() - 2) {
				m_model.setData(m_model.index(0, col), QVariant::fromValue(nullptr), Qt::EditRole);
				for (int i = 0; i < valueCount - 1; ++i) {
					auto value = array->GetTuple1(grid->cellIndex(i, j));
					m_model.setData(m_model.index(i + 1, col), value, Qt::EditRole);
				}
			} else {
				for (int i = 0; i < valueCount; ++i) {
					m_model.setData(m_model.index(i, col), QVariant::fromValue(nullptr), Qt::EditRole);
				}
			}
		}
	}
}
