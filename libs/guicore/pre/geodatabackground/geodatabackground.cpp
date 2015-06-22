#include "../base/preprocessorgeodatadataiteminterface.h"
#include "../base/preprocessorgeodatagroupdataiteminterface.h"
#include "../gridcond/base/gridattributeeditdialog.h"
#include "geodatabackground.h"
#include "geodatabackgroundeditdialog.h"
//#include "pre/preprocessorwindow.h"

#include <misc/iricundostack.h>

#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QStandardItem>
#include <QUndoCommand>

GeoDataBackground::GeoDataBackground(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition)
	: GeoData(d, creator, condition)
{
	m_name = "background";
	m_caption = tr("Default");
	m_filename = "background.dat";
	m_editValueAction = new QAction(tr("Edit value..."), this);
	connect(m_editValueAction, SIGNAL(triggered()), this, SLOT(editValue()));

	// set default value
	QVariant def = condition->variantDefaultValue();
	if (def == "min") {
		m_type = Minimum;
	} else if (def == "max") {
		m_type = Maximum;
	} else {
		m_type = Custom;
		m_customValue = def;
	}
	m_mapped = true;
}

void GeoDataBackground::setupDataItem()
{
	// hide from object browser.
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	QStandardItem* pItem = item->standardItem()->parent();
	pItem->takeRow(item->standardItem()->row());
}

void GeoDataBackground::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
}

void GeoDataBackground::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
}

void GeoDataBackground::loadExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);
	QVariant val;
	qint32 intv;
	s >> intv;
	s >> val;
	f.close();
	m_type = static_cast<Type>(intv);
	m_customValue = val;
}

void GeoDataBackground::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	qint32 intv = static_cast<qint32>(m_type);
	s << intv;
	s << m_customValue;
	f.close();
}

void GeoDataBackground::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editValueAction);
}

class GeoDataBackgroundEditValueCommand : public QUndoCommand
{
public:
	GeoDataBackgroundEditValueCommand(GeoDataBackground* b, GeoDataBackground::Type oldt, const QVariant& oldv, GeoDataBackground::Type newt, const QVariant& newv)
		: QUndoCommand(QObject::tr("Background edit value")) {
		m_background = b;
		m_oldType = oldt;
		m_oldValue = oldv;
		m_newType = newt;
		m_newValue = newv;
	}
	void redo() {
		m_background->m_type = m_newType;
		m_background->m_customValue = m_newValue;
		dynamic_cast<PreProcessorRawdataDataItemInterface*>(m_background->parent())->informValueRangeChange();
		dynamic_cast<PreProcessorRawdataDataItemInterface*>(m_background->parent())->informDataChange();
		m_background->m_mapped = false;
	}
	void undo() {
		m_background->m_type = m_oldType;
		m_background->m_customValue = m_oldValue;
		dynamic_cast<PreProcessorRawdataDataItemInterface*>(m_background->parent())->informValueRangeChange();
		dynamic_cast<PreProcessorRawdataDataItemInterface*>(m_background->parent())->informDataChange();
	}
private:
	QVariant m_oldValue;
	QVariant m_newValue;
	GeoDataBackground::Type m_oldType;
	GeoDataBackground::Type m_newType;
	GeoDataBackground* m_background;
};

void GeoDataBackground::editValue()
{
	if (m_gridRelatedCondition->isOption()) {
		GridAttributeEditDialog* dialog = m_gridRelatedCondition->editDialog(preProcessorWindow());
		PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
		dialog->setWindowTitle(QString(tr("Edit %1 default value")).arg(i->condition()->caption()));
		dialog->setLabel(tr("Please input new default value."));
		dialog->setVariantValue(m_customValue);
		int ret = dialog->exec();
		if (ret == QDialog::Accepted) {
			iRICUndoStack::instance().push(new GeoDataBackgroundEditValueCommand(this, m_type, m_customValue, m_type, dialog->variantValue()));
		}
		delete dialog;
	} else {
		GeoDataBackgroundEditDialog dialog(preProcessorWindow());
		PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
		dialog.setWindowTitle(QString(tr("Edit %1 default value")).arg(i->condition()->caption()));
		GridAttributeEditWidget* widget = m_gridRelatedCondition->editWidget(0);
		dialog.setWidget(widget);
		dialog.setType(m_type);
		dialog.setCustomValue(m_customValue);
		dialog.setupDialog();;
		int ret = dialog.exec();
		if (ret == QDialog::Accepted) {
			iRICUndoStack::instance().push(new GeoDataBackgroundEditValueCommand(this, m_type, m_customValue, dialog.type(), dialog.customValue()));
		}
	}
}

bool GeoDataBackground::getValueRange(double* min, double* max)
{
	if (m_type == Custom) {
		*min = m_customValue.toDouble();
		*max = m_customValue.toDouble();
		return true;
	}
	return false;
}

QVariant GeoDataBackground::variantValue()
{
	PreProcessorGeoDataGroupDataItemInterface* gi = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	double min, max;
	gi->getValueRange(&min, &max);
	if (m_type == Maximum) {
		m_customValue = max;
	} else if (m_type == Minimum) {
		m_customValue = min;
	}
	return m_customValue;
}
