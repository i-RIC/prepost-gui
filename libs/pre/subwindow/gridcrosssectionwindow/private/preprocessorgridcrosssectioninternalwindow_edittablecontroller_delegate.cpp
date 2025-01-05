#include "../../../datamodel/preprocessorgriddataitem.h"
#include "preprocessorgridcrosssectioninternalwindow_edittablecontroller_delegate.h"

#include <crosssection/private/abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h>
#include <crosssection/private/abstractcrosssectionwindow_impl.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QLineEdit>

PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate::Delegate(EditTableController* controller) :
	AbstractCrosssectionWindow::EditTableController::Delegate(controller)
{}


QWidget* PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
	auto setting = displaySetting(index.column());
	auto data = index.model()->data(index, Qt::EditRole);
	if (data.isNull()) {return nullptr;}

	auto editor = setting->attributeDataProvider()->attributeEditWidget(setting->attributeName(), parent);
	return editor;
}

void PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	auto data = index.model()->data(index, Qt::EditRole);
	if (data.isNull()) {return;}

	auto e = dynamic_cast<GridAttributeEditWidget*> (editor);
	e->setVariantValue(data);
}

void PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto setting = displaySetting(index.column());

	auto e = dynamic_cast<GridAttributeEditWidget*> (editor);
	auto w = m_controller->impl->m_window;
	auto preW = dynamic_cast<PreProcessorGridCrosssectionInternalWindow*> (w);
	auto inputGrid = preW->m_item->grid();
	if (inputGrid == nullptr) {return;}

	auto container = inputGrid->attribute(setting->attributeName());
	auto def = container->gridAttribute();

	auto grid = dynamic_cast<v4Structured2dGrid*> (inputGrid->grid());
	auto pos = def->position();
	auto c = m_controller->impl->m_controller;

	vtkDataSetAttributes* atts = nullptr;
	vtkIdType idx = 0;
	int i, j;

	if (pos == SolverDefinitionGridAttribute::Position::Node) {
		atts = grid->vtkData()->data()->GetPointData();

		if (c->targetDirection() == Direction::I) {
			i = c->targetIndex();
			j = index.row();
		} else {
			i = index.row();
			j = c->targetIndex();
		}
		idx = i + grid->dimensionI() * j;
	} else if (pos == SolverDefinitionGridAttribute::Position::CellCenter) {
		atts = grid->vtkData()->data()->GetCellData();

		if (c->targetDirection() == Direction::I) {
			if (c->cellSide() == Controller::CellSide::Previous) {
				i = c->targetIndex() - 1;
			} else {
				i = c->targetIndex();
			}
			j = index.row() - 1;
		} else {
			i = index.row() - 1;
			if (c->cellSide() == Controller::CellSide::Previous) {
				j = c->targetIndex() - 1;
			} else {
				j = c->targetIndex();
			}
		}
		idx = i + (grid->dimensionI() - 1) * j;
	} else if (pos == SolverDefinitionGridAttribute::Position::IFace) {
		atts = grid->vtkIEdgeData()->data()->GetCellData();
	} else if (pos == SolverDefinitionGridAttribute::Position::JFace) {
		atts = grid->vtkJEdgeData()->data()->GetCellData();
	}
	std::vector<vtkIdType> indices;
	indices.push_back(idx);

	e->applyValue(container, indices, atts, preW->m_item);
}

void PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer* PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate::displaySetting(int column) const
{
	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_controller->impl->m_displaySettings) {
		if (! s.visible) {continue;}
		activeSettings.push_back(&s);
	}

	return activeSettings.at(column);
}
