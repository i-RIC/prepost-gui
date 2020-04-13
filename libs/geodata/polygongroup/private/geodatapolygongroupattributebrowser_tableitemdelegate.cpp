#include "../geodatapolygongroup.h"
#include "../geodatapolygongrouppolygon.h"
#include "geodatapolygongroupattributebrowser_tableitemdelegate.h"

#include <geodata/polygon/geodatapolygon.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditwidget.h>
#include <guicore/solverdef/integerenumloader.h>

#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>

GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::TableItemDelegate(QObject *parent) :
	QStyledItemDelegate {parent},
	m_polygonGroup {nullptr}
{}

void GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::setPolygonGroup(GeoDataPolygonGroup* g)
{
	m_polygonGroup = g;
}

void GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_polygonGroup == nullptr) {return;}
	int col = index.column();
	auto att = m_polygonGroup->gridAttribute();
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (m_polygonGroup->parent()->parent());
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

QWidget* GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_polygonGroup == nullptr) {return nullptr;}
	int col = index.column();
	auto att = m_polygonGroup->gridAttribute();
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (m_polygonGroup->parent()->parent());
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

void GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (m_polygonGroup == nullptr) {return;}
	int col = index.column();
	auto att = m_polygonGroup->gridAttribute();
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

void GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (m_polygonGroup == nullptr) {return;}
	int col = index.column();
	auto att = m_polygonGroup->gridAttribute();
	bool isRef = att->isReferenceInformation();
	if (col == 0) {
		// name
		QStyledItemDelegate::setModelData(editor, model, index);
		QString newName = dynamic_cast<QLineEdit*> (editor)->text();
		auto targetPol = m_polygonGroup->editTargetPolygon();
		int targetIndex = m_polygonGroup->editTargetPolygonIndex();
		if (index.row() < targetIndex) {
			m_polygonGroup->allPolygons().at(index.row())->setName(newName);
		} else if (index.row() == targetIndex && targetPol != nullptr) {
			targetPol->setCaption(newName);
		} else {
			int offset = 0;
			if (targetPol != nullptr) {offset = -1;}
			m_polygonGroup->allPolygons().at(index.row() + offset)->setName(newName);
		}
	} else if (col == 1 && (! isRef)) {
		auto newValue = dynamic_cast<GridAttributeEditWidget*> (editor)->variantValue();
		auto targetPol = m_polygonGroup->editTargetPolygon();
		int targetIndex = m_polygonGroup->editTargetPolygonIndex();
		model->setData(index, newValue);
		if (index.row() < targetIndex) {
			m_polygonGroup->allPolygons().at(index.row())->setValue(newValue);
		} else if (index.row() == targetIndex && targetPol != nullptr) {
			targetPol->setVariantValue(newValue);
		} else {
			int offset = 0;
			if (targetPol != nullptr) {offset = -1;}
			m_polygonGroup->allPolygons().at(index.row() + offset)->setValue(newValue);
		}
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (m_polygonGroup->parent());
		m_polygonGroup->updateVtkObjects();
		p->informValueRangeChange();
		p->informDataChange();
	}
	else if (col == 2 || (col == 1 && isRef)) {
		return;
	}
}

void GeoDataPolygonGroupAttributeBrowser::TableItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}
