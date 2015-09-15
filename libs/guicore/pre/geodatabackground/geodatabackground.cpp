#include "../base/preprocessorgeodatadataiteminterface.h"
#include "../base/preprocessorgeodatagroupdataiteminterface.h"
#include "../gridcond/base/gridattributeeditdialog.h"
#include "geodatabackground.h"
#include "geodatabackgroundeditdialog.h"
#include "../base/preprocessorwindowinterface.h"

#include <misc/iricundostack.h>

#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QStandardItem>
#include <QUndoCommand>

GeoDataBackground::GeoDataBackground(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData {d, creator, condition}
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
	PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
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
	GeoDataBackgroundEditValueCommand(GeoDataBackground::Type type, const QVariant& val, GeoDataBackground* b) :
		QUndoCommand(GeoDataBackground::tr("Background edit value")),
		m_newType {type},
		m_newValue {val},
		m_oldType {b->m_type},
		m_oldValue {b->variantValue()},
		m_oldMapped {b->isMapped()},
		m_background {b}
	{}
	void redo() override {
		applyValues(m_newType, m_newValue, false);
	}
	void undo() override {
		applyValues(m_oldType, m_oldValue, m_oldMapped);
	}

private:
	void applyValues(GeoDataBackground::Type t, const QVariant& val, bool mapped)
	{
		m_background->m_type = t;
		m_background->m_customValue = val;
		m_background->setMapped(mapped);

		auto geoData = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (m_background->parent());
		geoData->informValueRangeChange();
		geoData->informDataChange();
	}

	GeoDataBackground::Type m_newType;
	QVariant m_newValue;
	GeoDataBackground::Type m_oldType;
	QVariant m_oldValue;
	bool m_oldMapped;

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
			iRICUndoStack::instance().push(new GeoDataBackgroundEditValueCommand(m_type, dialog->variantValue(), this));
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
			iRICUndoStack::instance().push(new GeoDataBackgroundEditValueCommand(dialog.type(), dialog.customValue(), this));
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
