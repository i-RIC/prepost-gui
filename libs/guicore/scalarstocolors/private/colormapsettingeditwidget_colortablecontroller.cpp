#include "colormapsettingeditwidget_colortablecontroller.h"
#include "colormapsettingeditwidget_colortablecontroller_delegate.h"

#include <QColorDialog>
#include <QItemSelectionModel>

namespace {

// const int VALUEWIDTH = 170;
const int COLORWIDTH = 50;
const int TRANSPARENTWIDTH = 80;

void getMinMax(const ColorMapSettingContainer& setting, double* min, double* max)
{
	if (setting.autoValueRange) {
		*min = setting.autoMinValue;
		*max = setting.autoMaxValue;
	} else {
		*min = setting.minValue;
		*max = setting.maxValue;
	}
}

void applyTableRelativeContinuous(const ColorMapSettingContainer& setting, QStandardItemModel* model, QTableView* view)
{
	double min, max;
	getMinMax(setting, &min, &max);

	const auto& colors = setting.colors;

	model->setColumnCount(2);
	model->setRowCount(setting.colors.size());

	model->setHeaderData(0, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Value"));
	model->setHeaderData(1, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Color"));
	// view->setColumnWidth(0, VALUEWIDTH);
	view->setColumnWidth(1, COLORWIDTH);
	for (int i = 0; i < colors.size(); ++i) {
		const auto& c = colors.at(i);
		auto itemV = new QStandardItem();
		itemV->setData(min + (max - min) * c.value.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 0, itemV);

		auto itemC = new QStandardItem();
		itemC->setData(c.color.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 1, itemC);
	}
}

void applyTableRelativeDiscrete(const ColorMapSettingContainer& setting, QStandardItemModel* model, QTableView* view)
{
	double min, max;
	getMinMax(setting, &min, &max);

	const auto& colors = setting.colors;

	model->setColumnCount(4);
	model->setRowCount(setting.colors.size());

	model->setHeaderData(0, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Min value"));
	model->setHeaderData(1, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Max value"));
	model->setHeaderData(2, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Color"));
	model->setHeaderData(3, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Transparent"));
	// view->setColumnWidth(0, VALUEWIDTH);
	// view->setColumnWidth(1, VALUEWIDTH);
	view->setColumnWidth(2, COLORWIDTH);
	view->setColumnWidth(3, TRANSPARENTWIDTH);

	for (int i = 0; i < colors.size(); ++i) {
		auto itemMin = new QStandardItem();
		if (i == 0) {
			itemMin->setData(min + (max - min) * setting.colorTableMinValue.value(), Qt::EditRole);
		} else {
			itemMin->setEditable(false);
			itemMin->setData(min + (max - min) * colors.at(i - 1).value.value(), Qt::EditRole);
		}
		model->setItem(colors.size() - 1 - i, 0, itemMin);

		auto itemMax = new QStandardItem();
		itemMax->setData(min + (max - min) * colors.at(i).value.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 1, itemMax);

		auto itemC = new QStandardItem();
		itemC->setData(colors.at(i).color.value(), Qt::EditRole);
		itemC->setEditable(false);
		model->setItem(colors.size() - 1 - i, 2, itemC);

		auto itemT = new QStandardItem();
		itemT->setData(colors.at(i).transparent.value(), Qt::EditRole);
		itemT->setEditable(false);
		model->setItem(colors.size() - 1 - i, 3, itemT);
	}
}

void applyTableAbsoluteContinuous(const ColorMapSettingContainer& setting, QStandardItemModel* model, QTableView* view)
{
	const auto& colors = setting.colors;

	model->setColumnCount(2);
	model->setRowCount(setting.colors.size());

	model->setHeaderData(0, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Value"));
	model->setHeaderData(1, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Color"));
	// view->setColumnWidth(0, VALUEWIDTH);
	view->setColumnWidth(1, COLORWIDTH);

	for (int i = 0; i < colors.size(); ++i) {
		const auto& c = colors.at(i);
		auto itemV = new QStandardItem();
		itemV->setData(c.value.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 0, itemV);

		auto itemC = new QStandardItem();
		itemC->setData(c.color.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 1, itemC);
	}
}

void applyTableAbsoluteDiscrete(const ColorMapSettingContainer& setting, QStandardItemModel* model, QTableView* view)
{
	const auto& colors = setting.colors;

	model->setColumnCount(4);
	model->setRowCount(setting.colors.size());

	model->setHeaderData(0, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Min value"));
	model->setHeaderData(1, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Max value"));
	model->setHeaderData(2, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Color"));
	model->setHeaderData(3, Qt::Orientation::Horizontal, ColorMapSettingEditWidget::tr("Transparent"));
	// view->setColumnWidth(0, VALUEWIDTH);
	// view->setColumnWidth(1, VALUEWIDTH);
	view->setColumnWidth(2, COLORWIDTH);
	view->setColumnWidth(3, TRANSPARENTWIDTH);

	for (int i = 0; i < colors.size(); ++i) {
		auto itemMin = new QStandardItem();
		if (i == 0) {
			itemMin->setData(setting.colorTableMinValue.value(), Qt::EditRole);
		} else {
			itemMin->setData(colors.at(i - 1).value.value(), Qt::EditRole);
			itemMin->setEditable(false);
		}
		model->setItem(colors.size() - 1 - i, 0, itemMin);

		auto itemMax = new QStandardItem();
		itemMax->setData(colors.at(i).value.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 1, itemMax);

		auto itemC = new QStandardItem();
		itemC->setData(colors.at(i).color.value(), Qt::EditRole);
		model->setItem(colors.size() - 1 - i, 2, itemC);

		auto itemT = new QStandardItem();
		itemT->setData(colors.at(i).transparent.value(), Qt::EditRole);
		itemT->setEditable(false);
		model->setItem(colors.size() - 1 - i, 3, itemT);
	}
}

} // namespace

ColorMapSettingEditWidget::ColorTableController::ColorTableController(ColorMapSettingEditWidget* widget) :
	QObject {nullptr},
	m_widget {widget},
	m_delegate(new Delegate(this))
{
	auto t = m_widget->colorTable();
	t->setModel(&m_model);
	t->setItemDelegate(m_delegate);

	connect(t, &QTableView::clicked, this, &ColorMapSettingEditWidget::ColorTableController::handleItemClick);

	auto selModel = new QItemSelectionModel(&m_model);
	t->setSelectionModel(selModel);
	connect(selModel, &QItemSelectionModel::selectionChanged, this, &ColorTableController::handleTableSelectionChange);
}

ColorMapSettingEditWidget::ColorTableController::~ColorTableController()
{
	delete m_delegate;
}

void ColorMapSettingEditWidget::ColorTableController::applyToTable()
{
	const auto& s = m_widget->m_concreteSetting;
	auto t = m_widget->colorTable();

	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {
		if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
			applyTableRelativeContinuous(s, &m_model, t);
		} else if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
			applyTableRelativeDiscrete(s, &m_model, t);
		}
	} else if (s.valueMode == ColorMapSettingContainer::ValueMode::Absolute) {
		if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
			applyTableAbsoluteContinuous(s, &m_model, t);
		} else if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
			applyTableAbsoluteDiscrete(s, &m_model, t);
		}
	}
}

void ColorMapSettingEditWidget::ColorTableController::handleItemClick(const QModelIndex& index)
{
	auto& s = m_widget->m_concreteSetting;
	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {return;}

	auto& c = s.colors.at(s.colors.size() - 1 - index.row());

	if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		if (index.column() == 1) {
			editColor(index, &c);
		}
	} else if (s.transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		if (index.column() == 2) {
			editColor(index, &c);
		} else if (index.column() == 3) {
			editTransparent(index, &c);
		}
	}
}

void ColorMapSettingEditWidget::ColorTableController::handleTableSelectionChange(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
	const auto& s = m_widget->m_concreteSetting;
	if (s.valueMode == ColorMapSettingContainer::ValueMode::Relative) {return;}

	auto rows = m_widget->colorTable()->selectionModel()->selectedRows();
	bool removeOk = (rows.size() > 0 && s.colors.size() - rows.size() >= 2);

	m_widget->removeButton()->setEnabled(removeOk);
}

void ColorMapSettingEditWidget::ColorTableController::editColor(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c)
{
	auto newColor = QColorDialog::getColor(c->color.value(), m_widget);
	if (! newColor.isValid()) {return;}

	c->color = newColor;
	m_model.setData(index, newColor);
}

void ColorMapSettingEditWidget::ColorTableController::editTransparent(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c)
{
	c->transparent = ! (c->transparent.value());
	m_model.setData(index, c->transparent.value());
}
