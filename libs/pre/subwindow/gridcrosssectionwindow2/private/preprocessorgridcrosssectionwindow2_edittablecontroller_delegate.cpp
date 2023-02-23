#include "preprocessorgridcrosssectionwindow2_controller.h"
#include "preprocessorgridcrosssectionwindow2_edittablecontroller_delegate.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"

#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

PreProcessorGridCrosssectionWindow2::EditTableController::Delegate::Delegate(EditTableController* controller) :
	QItemDelegate {controller},
	m_controller {controller}
{}

void PreProcessorGridCrosssectionWindow2::EditTableController::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_controller->m_impl->m_displaySettings) {
		if (! s.visible) {continue;}
		activeSettings.push_back(&s);
	}

	int col = index.column();
	auto setting = activeSettings.at(col);

	auto widget = setting->attribute()->editWidget(m_controller->m_view);

	auto data = index.model()->data(index, Qt::EditRole);
	if (data.isNull()) {return;}

	widget->setVariantValue(data);
	widget->resize(option.rect.size());

	QPixmap pixmap(option.rect.size());
	widget->render(&pixmap);
	painter->drawPixmap(option.rect, pixmap);

	delete widget;
}

QWidget* PreProcessorGridCrosssectionWindow2::EditTableController::Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return nullptr;
}
