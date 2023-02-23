#include "preprocessorgridcrosssectionwindow2_controller.h"
#include "preprocessorgridcrosssectionwindow2_edittablecontroller.h"
#include "preprocessorgridcrosssectionwindow2_edittablecontroller_delegate.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"
#include "../public/preprocessorgridcrosssectionwindow2_graphicsview.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/pre/gridcond/base/gridattributestringconverter.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QFile>
#include <QTextStream>

PreProcessorGridCrosssectionWindow2::EditTableController::EditTableController(QTableView* view, Impl* impl) :
	m_view {view},
	m_impl {impl},
	m_delegate {new Delegate {this}}
{
	view->setModel(&m_model);
	view->setEditTriggers(QTableView::AllEditTriggers);
	view->setItemDelegate(m_delegate);
}

const QStandardItemModel& PreProcessorGridCrosssectionWindow2::EditTableController::model() const
{
	return m_model;
}

bool PreProcessorGridCrosssectionWindow2::EditTableController::saveCsvFile(const QString& fileName)
{
	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	std::vector<GridAttributeStringConverter*> converters;

	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
		converters.push_back(s.attribute()->stringConverter());
	}

	QFile file(fileName);
	bool ok = file.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QTextStream stream(&file);

	stream << PreProcessorGridCrosssectionWindow2::tr("Index")
				 << "," << PreProcessorGridCrosssectionWindow2::tr("Distance(m)");
	for (auto s : activeSettings) {
		stream << "," << s->attribute()->caption();
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

void PreProcessorGridCrosssectionWindow2::EditTableController::applyToTable()
{
	auto grid = m_impl->grid();
	auto controller = m_impl->m_controller;

	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_impl->m_displaySettings) {
		if (! s.visible) {continue;}

		activeSettings.push_back(&s);
	}

	m_model.setColumnCount(activeSettings.size());

	int valueCount = 0;
	if (controller->targetDirection() == Direction::I) {
		valueCount = grid->dimensionJ();
	} else if (controller->targetDirection() == Direction::J) {
		valueCount = grid->dimensionI();
	}

	m_model.setRowCount(valueCount);

	QStringList labels;
	for (auto s : activeSettings) {
		labels.append(s->attribute()->caption());
	}

	m_model.setHorizontalHeaderLabels(labels);

	int col = 0;
	for (auto& s : activeSettings) {
		auto att = grid->gridAttribute(s->attribute()->name());
		auto editWidget = s->attribute()->editWidget(m_view);
		if (s->attribute()->position() == SolverDefinitionGridAttribute::Position::Node) {
			if (controller->targetDirection() == Direction::I) {
				int i = controller->targetIndex();
				if (i == -1) {return;}
				for (int j = 0; j < valueCount; ++j) {
					QVector<vtkIdType> indices;
					indices.append(grid->vertexIndex(i, j));
					editWidget->scanAndSetDefault(att, indices);
					m_model.setData(m_model.index(j, col), editWidget->variantValue(), Qt::EditRole);
				}
			} else if (controller->targetDirection() == Direction::J) {
				int j = controller->targetIndex();
				if (j == -1) {return;}
				for (int i = 0; i < valueCount; ++i) {
					QVector<vtkIdType> indices;
					indices.append(grid->vertexIndex(i, j));
					editWidget->scanAndSetDefault(att, indices);
					m_model.setData(m_model.index(i, col), editWidget->variantValue(), Qt::EditRole);
				}
			}
		} else if (s->attribute()->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
			if (controller->targetDirection() == Direction::I) {
				int i = controller->targetIndex();
				if (i == -1) {return;}
				if (controller->cellSide() == Controller::CellSide::Previous) {
					-- i;
				}
				if (i >= 0 && i <= grid->dimensionI() - 2) {
					m_model.setData(m_model.index(0, col), QVariant::fromValue(nullptr), Qt::EditRole);
					for (int j = 0; j < valueCount - 1; ++j) {
						QVector<vtkIdType> indices;
						indices.append(grid->cellIndex(i, j));
						editWidget->scanAndSetDefault(att, indices);
						m_model.setData(m_model.index(j + 1, col), editWidget->variantValue(), Qt::EditRole);
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
						QVector<vtkIdType> indices;
						indices.append(grid->cellIndex(i, j));
						editWidget->scanAndSetDefault(att, indices);
						m_model.setData(m_model.index(i + 1, col), editWidget->variantValue(), Qt::EditRole);
					}
				} else {
					for (int i = 0; i < valueCount; ++i) {
						m_model.setData(m_model.index(i, col), QVariant::fromValue(nullptr), Qt::EditRole);
					}
				}
			}
		}
		delete editWidget;
		++ col;
	}
}
