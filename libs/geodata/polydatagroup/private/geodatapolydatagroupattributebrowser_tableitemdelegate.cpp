#include "../geodatapolydatagroup.h"
#include "../geodatapolydatagrouppolydata.h"
#include "geodatapolydatagroupattributebrowser_tableitemdelegate.h"

#include <geodata/polydata/geodatapolydata.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/solverdef/integerenumloader.h>

#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>

GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::TableItemDelegate(QObject *parent) :
	QStyledItemDelegate {parent},
	m_group {nullptr}
{}

void GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::setPolyDataGroup(GeoDataPolyDataGroup* g)
{
	m_group = g;
}

void GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_group == nullptr) {return;}
	int col = index.column();
	auto att = m_group->gridAttribute();
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (m_group->parent()->parent());
	bool isRef = att->isReferenceInformation();
	if (col == 0) {
		// name
		QStyledItemDelegate::paint(painter, option, index);
	} else if (col == 1 && (! isRef)) {
		// value
		auto w = att->editWidget(nullptr);
		i->setupEditWidget(w);
		w->setVariantValue(index.data());
		w->setGeometry(option.rect);
		QPixmap pixmap(option.rect.size());
		w->render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
		delete w;
	} else if (col == 2 || (col == 1 && isRef)) {
		// show button
		QPushButton button(nullptr);
		button.setText(tr("Show"));
		button.resize(option.rect.size());
		QPixmap pixmap(option.rect.size());
		button.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	}
}

QWidget* GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_group == nullptr) {return nullptr;}
	int col = index.column();
	auto att = m_group->gridAttribute();
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (m_group->parent()->parent());
	bool isRef = att->isReferenceInformation();
	if (col == 0) {
		// name
		return QStyledItemDelegate::createEditor(parent, option, index);
	} else if (col == 1 && (! isRef)) {
		// value
		auto w = att->editWidget(parent);
		i->setupEditWidget(w);
		return w;
	} else {
		return nullptr;
	}
}

void GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (m_group == nullptr) {return;}
	int col = index.column();
	auto att = m_group->gridAttribute();
	bool isRef = att->isReferenceInformation();
	if (col == 0) {
		// name
		return QStyledItemDelegate::setEditorData(editor, index);
	} else if (col == 1 && (! isRef)) {
		auto w = dynamic_cast<GridAttributeEditWidget*> (editor);
		w->setVariantValue(index.data());
	} else if (col == 2 || (col == 1 && isRef)) {
		return;
	}
}

void GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (m_group == nullptr) {return;}
	int col = index.column();
	auto att = m_group->gridAttribute();
	bool isRef = att->isReferenceInformation();
	if (col == 0) {
		// name
		QStyledItemDelegate::setModelData(editor, model, index);
		QString newName = dynamic_cast<QLineEdit*> (editor)->text();
		auto targetData = m_group->editTargetData();
		int targetIndex = m_group->editTargetDataIndex();
		if (index.row() < targetIndex) {
			m_group->data().at(index.row())->setName(newName);
		} else if (index.row() == targetIndex && targetData != nullptr) {
			targetData->setCaptionAndEmitEdited(newName);
		} else {
			int offset = 0;
			if (targetData != nullptr) { offset = -1; }
			m_group->data().at(index.row() + offset)->setName(newName);
		}
	} else if (col == 1 && (! isRef)) {
		// value
		auto newValue = dynamic_cast<GridAttributeEditWidget*> (editor)->variantValue();
		model->setData(index, newValue);
		auto targetData = m_group->editTargetData();
		int targetIndex = m_group->editTargetDataIndex();
		if (index.row() < targetIndex) {
			m_group->data().at(index.row())->setValue(newValue);
		} else if (index.row() == targetIndex && targetData != nullptr) {
			targetData->setVariantValueAndEmitEdited(newValue);
		} else {
			int offset = 0;
			if (targetData != nullptr) {offset = -1;}
			m_group->data().at(index.row() + offset)->setValue(newValue);
		}
		auto p = dynamic_cast<PreProcessorGeoDataDataItemI*> (m_group->parent());
		m_group->updateVtkObjects();
		p->informValueRangeChange();
		p->informDataChange();
	}
	else if (col == 2 || (col == 1 && isRef)) {
		return;
	}
}

void GeoDataPolyDataGroupAttributeBrowser::TableItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}
