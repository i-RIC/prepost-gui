#include "../public/abstractcrosssectionwindow_controller.h"
#include "abstractcrosssectionwindow_edittablecontroller_delegate.h"
#include "abstractcrosssectionwindow_impl.h"

#include <guicore/pre/gridcond/base/gridattributestringconverter.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QLineEdit>

AbstractCrosssectionWindow::EditTableController::Delegate::Delegate(EditTableController* controller) :
	QItemDelegate {controller},
	m_controller {controller}
{}

void AbstractCrosssectionWindow::EditTableController::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	std::vector<GridAttributeDisplaySettingContainer*> activeSettings;
	for (auto& s : m_controller->m_impl->m_displaySettings) {
		if (! s.visible) {continue;}
		activeSettings.push_back(&s);
	}

	int col = index.column();
	auto setting = activeSettings.at(col);

	auto data = index.model()->data(index, Qt::EditRole);
	if (data.isNull()) {return;}

	auto converter = setting->attributeDataProvider()->createStringConverter(setting->attributeName());
	QLineEdit edit;
	edit.setText(converter->convert(data));

	edit.resize(option.rect.size());

	QPixmap pixmap(option.rect.size());
	edit.render(&pixmap);
	painter->drawPixmap(option.rect, pixmap);

	delete converter;
}

QWidget* AbstractCrosssectionWindow::EditTableController::Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return nullptr;
}
