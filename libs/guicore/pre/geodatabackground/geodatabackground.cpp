#include "../base/preprocessorgeodatadataitemi.h"
#include "../base/preprocessorgeodatagroupdataitemi.h"
#include "../gridcond/base/gridattributeeditdialog.h"
#include "geodatabackground.h"
#include "geodatabackgroundeditdialog.h"
#include "../base/preprocessorwindowi.h"
#include "private/geodatabackground_editvaluecommand.h"

#include <misc/iricundostack.h>

#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QStandardItem>

GeoDataBackground::GeoDataBackground(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataWithSingleMapper {d, creator, condition}
{
	setName("background");
	setCaption(GeoDataBackground::tr("Default"));
	m_editValueAction = new QAction(GeoDataBackground::tr("Edit value..."), this);
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
	setMapped(true);
}

void GeoDataBackground::setupDataItem()
{
	// hide from object browser.
	PreProcessorGeoDataDataItemI* item = dynamic_cast<PreProcessorGeoDataDataItemI*>(parent());
	QStandardItem* pItem = item->standardItem()->parent();
	pItem->takeRow(item->standardItem()->row());
}

const QVariant& GeoDataBackground::customValue() const
{
	return m_customValue;
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

void GeoDataBackground::editValue()
{
	if (m_gridAttribute->isOption()) {
		GridAttributeEditDialog* dialog = m_gridAttribute->editDialog(preProcessorWindow());
		PreProcessorGeoDataGroupDataItemI* i = dynamic_cast<PreProcessorGeoDataGroupDataItemI*>(parent()->parent());
		dialog->setWindowTitle(QString(tr("Edit %1 default value")).arg(i->condition()->caption()));
		dialog->setLabel(tr("Please input new default value."));
		dialog->setVariantValue(m_customValue);
		int ret = dialog->exec();
		if (ret == QDialog::Accepted) {
			iRICUndoStack::instance().push(new EditValueCommand(m_type, dialog->variantValue(), this));
		}
		delete dialog;
	} else {
		GeoDataBackgroundEditDialog dialog(preProcessorWindow());
		PreProcessorGeoDataGroupDataItemI* i = dynamic_cast<PreProcessorGeoDataGroupDataItemI*>(parent()->parent());
		dialog.setWindowTitle(QString(tr("Edit %1 default value")).arg(i->condition()->caption()));
		GridAttributeEditWidget* widget = m_gridAttribute->editWidget(0);
		dialog.setWidget(widget);
		dialog.setType(m_type);
		dialog.setCustomValue(m_customValue);
		dialog.setupDialog();;
		int ret = dialog.exec();
		if (ret == QDialog::Accepted) {
			iRICUndoStack::instance().push(new EditValueCommand(dialog.type(), dialog.customValue(), this));
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
	PreProcessorGeoDataGroupDataItemI* gi = dynamic_cast<PreProcessorGeoDataGroupDataItemI*>(parent()->parent());
	double min, max;
	gi->getValueRange(&min, &max);
	if (m_type == Maximum) {
		m_customValue = max;
	} else if (m_type == Minimum) {
		m_customValue = min;
	}
	return m_customValue;
}

GeoDataBackground::Type GeoDataBackground::type() const
{
	return m_type;
}

void GeoDataBackground::setType(Type t) {
	m_type = t;
}
