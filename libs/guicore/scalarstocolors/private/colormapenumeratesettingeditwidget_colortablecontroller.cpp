#include "colormapenumeratesettingeditwidget_colortablecontroller.h"
#include "colormapenumeratesettingeditwidget_colortablecontroller_delegate.h"

#include <QColorDialog>
#include <QItemSelectionModel>

namespace {

// const int VALUEWIDTH = 170;
const int COLORWIDTH = 50;
const int TRANSPARENTWIDTH = 100;

} // namespace

ColorMapEnumerateSettingEditWidget::ColorTableController::ColorTableController(ColorMapEnumerateSettingEditWidget* widget) :
	QObject {nullptr},
	m_widget {widget},
	m_delegate(new Delegate(this))
{
	auto t = m_widget->colorTable();
	t->setModel(&m_model);
	t->setItemDelegate(m_delegate);

	t->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect(t, &QTableView::clicked, this, &ColorMapEnumerateSettingEditWidget::ColorTableController::handleItemClick);
}

ColorMapEnumerateSettingEditWidget::ColorTableController::~ColorTableController()
{
	delete m_delegate;
}

void ColorMapEnumerateSettingEditWidget::ColorTableController::applyToTable()
{
	const auto& s = m_widget->m_concreteSetting;
	auto t = m_widget->colorTable();

	const auto& colors = s.colors;

	m_model.setColumnCount(3);
	m_model.setRowCount(static_cast<int> (colors.size()));

	m_model.setHeaderData(0, Qt::Orientation::Horizontal, ColorMapEnumerateSettingEditWidget::tr("Value"));
	m_model.setHeaderData(1, Qt::Orientation::Horizontal, ColorMapEnumerateSettingEditWidget::tr("Color"));
	m_model.setHeaderData(2, Qt::Orientation::Horizontal, ColorMapEnumerateSettingEditWidget::tr("Transparent"));
	// view->setColumnWidth(0, VALUEWIDTH);
	// view->setColumnWidth(1, VALUEWIDTH);
	t->setColumnWidth(1, COLORWIDTH);
	t->setColumnWidth(2, TRANSPARENTWIDTH);

	for (int i = 0; i < colors.size(); ++i) {
		const auto c = colors.at(i);

		auto itemValue = new QStandardItem();
		itemValue->setData(s.valueCaptions.at(c.value), Qt::EditRole);
		m_model.setItem(i, 0, itemValue);

		auto itemC = new QStandardItem();
		itemC->setData(c.color.value(), Qt::EditRole);
		m_model.setItem(i, 1, itemC);

		auto itemT = new QStandardItem();
		itemT->setData(c.transparent.value(), Qt::EditRole);
		m_model.setItem(i, 2, itemT);
	}
}

void ColorMapEnumerateSettingEditWidget::ColorTableController::handleItemClick(const QModelIndex& index)
{
	auto& s = m_widget->m_concreteSetting;
	auto& c = s.colors.at(index.row());

	if (index.column() == 1) {
		editColor(index, &c);
	} else if (index.column() == 2) {
		editTransparent(index, &c);
	}
}

void ColorMapEnumerateSettingEditWidget::ColorTableController::editColor(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c)
{
	auto newColor = QColorDialog::getColor(c->color.value(), m_widget);
	if (! newColor.isValid()) {return;}

	c->color = newColor;
	m_model.setData(index, newColor);
}

void ColorMapEnumerateSettingEditWidget::ColorTableController::editTransparent(const QModelIndex& index, ColorMapSettingValueColorPairContainer* c)
{
	c->transparent = ! (c->transparent.value());
	m_model.setData(index, c->transparent.value());
}
