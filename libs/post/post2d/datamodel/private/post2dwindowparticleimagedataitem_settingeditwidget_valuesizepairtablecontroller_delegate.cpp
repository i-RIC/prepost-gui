#include "post2dwindowparticleimagedataitem_settingeditwidget_valuesizepairtablecontroller_delegate.h"

#include <guibase/widget/realnumbereditwidget.h>

namespace {
	void paintRealEdit(QPainter* painter, double val, const QStyleOptionViewItem& option, bool disabled)
	{
		RealNumberEditWidget edit;
		edit.setValue(val);
		edit.resize(option.rect.size());
		edit.setDisabled(disabled);

		QPixmap pixmap(option.rect.size());
		edit.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	}

	QVariant variantData(const QModelIndex& index)
	{
		auto m = index.model();
		return m->data(index, Qt::EditRole);
	}

} // namespace

Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::Delegate(ValueSizePairTableController* controller, QObject* parent) :
	QItemDelegate {parent},
	m_controller {controller}
{}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	paintRealEdit(painter, variantData(index).toDouble(), option, false);
}

QWidget* Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
	auto editor = new RealNumberEditWidget(parent);
	editor->setAcceptEvents(true);
	return editor;
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	auto s = setting();
	auto w = dynamic_cast<RealNumberEditWidget*> (editor);
	w->setValue(variantData(index).toDouble());

	if (index.column() == 0) {
		auto m = index.model();
		if (index.row() < static_cast<int> (s->valueSizePairs.size()) - 1) {
			w->setMaximum(variantData(m->index(index.row() + 1, 0)).toDouble());
		}
		if (index.row() > 0) {
			w->setMinimum(variantData(m->index(index.row() - 1, 0)).toDouble());
		}
	}
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto w = dynamic_cast<RealNumberEditWidget*> (editor);
	model->setData(index, w->value());

	auto s = setting();
	auto& pair = s->valueSizePairs.at(index.row());
	if (index.column() == 0) {
		pair.value = w->value();
	} else if (index.column() == 1) {
		pair.size = w->value();
	}
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}

Post2dWindowParticleImageDataItem::Setting* Post2dWindowParticleImageDataItem::SettingEditWidget::ValueSizePairTableController::Delegate::setting() const
{
	return &m_controller->m_widget->m_setting;
}
