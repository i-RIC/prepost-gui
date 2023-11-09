#include "colormapsettingeditwidget_colortablecontroller_delegate.h"
#include "../colormapsettingcontainer.h"

#include <guibase/widget/centeredcheckbox.h>
#include <guibase/widget/coloreditwidget.h>
#include <guibase/widget/realnumbereditwidget.h>

#include <QBrush>
#include <QPainter>
#include <QPixmap>

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

	void paintColor(QPainter* painter, const QColor& col, const QStyleOptionViewItem& option)
	{
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	}

	void paintCheckBox(QPainter* painter, bool value, const QStyleOptionViewItem& option)
	{
		CenteredCheckBox checkbox;
		checkbox.setChecked(value);
		checkbox.resize(option.rect.size());

		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	}

	QVariant variantData(const QModelIndex& index)
	{
		auto m = index.model();
		return m->data(index, Qt::EditRole);
	}

} // namespace

ColorMapSettingEditWidget::ColorTableController::Delegate::Delegate(ColorMapSettingEditWidget::ColorTableController* controller, QObject* parent) :
	QItemDelegate {parent},
	m_controller {controller}
{}

void ColorMapSettingEditWidget::ColorTableController::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto s = setting();

	if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		if (index.column() == 0) {
			paintRealEdit(painter, variantData(index).toDouble(), option, false);
		} else {
			paintColor(painter, variantData(index).value<QColor>(), option);
		}
	} else if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		if (index.column() < 2) {
			paintRealEdit(painter, variantData(index).toDouble(), option, index.column() == 0 && index.row() < s->colors.size() - 1);
		} else if (index.column() == 2) {
			paintColor(painter, variantData(index).value<QColor>(), option);
		} else {
			paintCheckBox(painter, variantData(index).value<bool>(), option);
		}
	}
}

QWidget* ColorMapSettingEditWidget::ColorTableController::Delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
	auto s = setting();

	if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		if (index.column() == 0) {
			auto edit = new RealNumberEditWidget(parent);
			edit->setAcceptEvents(true);
			return edit;
		} else {
			return nullptr;
		}
	} else if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		if (index.column() < 2) {
			auto edit = new RealNumberEditWidget(parent);
			edit->setDisabled(index.column() == 0 && index.row() < s->colors.size() - 1);
			edit->setAcceptEvents(true);
			return edit;
		} else {
			return nullptr;
		}
	}
	return nullptr;
}

void ColorMapSettingEditWidget::ColorTableController::Delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	auto s = setting();

	if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		if (index.column() == 0) {
			auto w = dynamic_cast<RealNumberEditWidget*> (editor);
			w->setValue(variantData(index).toDouble());

			auto m = index.model();
			if (index.row() < s->colors.size() - 1) {
				w->setMinimum(variantData(m->index(index.row() + 1, 0)).toDouble());
			}
			if (index.row() > 0) {
				w->setMaximum(variantData(m->index(index.row() - 1, 0)).toDouble());
			}
		}
	} else if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		if (index.column() < 2) {
			auto w = dynamic_cast<RealNumberEditWidget*> (editor);
			w->setValue(variantData(index).toDouble());

			auto m = index.model();
			if (index.column() == 0 && index.row() == s->colors.size() - 1) {
				w->setMaximum(variantData(m->index(static_cast<int> (s->colors.size()) - 1, 1)).toDouble());
			} else if (index.column() == 1) {
				if (index.row() == s->colors.size() - 1) {
					w->setMinimum(variantData(m->index(static_cast<int> (s->colors.size()) - 1, 0)).toDouble());
				} else {
					w->setMinimum(variantData(m->index(index.row() + 1, 1)).toDouble());
				}
				if (index.row() > 0) {
					w->setMaximum(variantData(m->index(index.row() - 1, 1)).toDouble());
				}
			}
		}
	}
}

void ColorMapSettingEditWidget::ColorTableController::Delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto s = setting();

	if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		if (index.column() == 0) {
			auto w = dynamic_cast<RealNumberEditWidget*> (editor);
			auto& c = s->colors.at(s->colors.size() - 1 - index.row());
			c.value = w->value();
			model->setData(index, w->value());
		}
	} else if (s->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		if (index.column() < 2) {
			auto w = dynamic_cast<RealNumberEditWidget*> (editor);
			if (index.column() == 0 && index.row() == s->colors.size() - 1) {
				s->colorTableMinValue = w->value();
				model->setData(index, w->value());
			} else if (index.column() == 1) {
				auto& c = s->colors.at(s->colors.size() - 1 - index.row());
				c.value = w->value();
				model->setData(index, w->value());
				if (index.row() != 0) {
					model->setData(model->index(index.row() - 1, 0), w->value());
				}
			}
		}
	}
}

void ColorMapSettingEditWidget::ColorTableController::Delegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}

ColorMapSettingContainer* ColorMapSettingEditWidget::ColorTableController::Delegate::setting() const
{
	return &m_controller->m_widget->m_concreteSetting;
}
