#include "../base/preprocessorrawdatadataiteminterface.h"
#include "../base/preprocessorrawdatagroupdataiteminterface.h"
#include "../gridcond/base/gridattributeeditdialog.h"
#include "rawdatabackground.h"
#include "rawdatabackgroundeditdialog.h"
//#include "pre/preprocessorwindow.h"

#include <misc/iricundostack.h>

#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QStandardItem>
#include <QUndoCommand>

RawDataBackground::RawDataBackground(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridAttribute* condition)
	: RawData(d, creator, condition)
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

void RawDataBackground::setupDataItem()
{
	// hide from object browser.
	PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent());
	QStandardItem* pItem = item->standardItem()->parent();
	pItem->takeRow(item->standardItem()->row());
}

void RawDataBackground::doLoadFromProjectMainFile(const QDomNode& node)
{
	RawData::doLoadFromProjectMainFile(node);
}

void RawDataBackground::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	RawData::doSaveToProjectMainFile(writer);
}

void RawDataBackground::loadExternalData(const QString& filename)
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

void RawDataBackground::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	qint32 intv = static_cast<qint32>(m_type);
	s << intv;
	s << m_customValue;
	f.close();
}

void RawDataBackground::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editValueAction);
}

class RawDataBackgroundEditValueCommand : public QUndoCommand
{
public:
	RawDataBackgroundEditValueCommand(RawDataBackground* b, RawDataBackground::Type oldt, const QVariant& oldv, RawDataBackground::Type newt, const QVariant& newv)
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
	RawDataBackground::Type m_oldType;
	RawDataBackground::Type m_newType;
	RawDataBackground* m_background;
};

void RawDataBackground::editValue()
{
	if (m_gridRelatedCondition->isOption()) {
		GridAttributeEditDialog* dialog = m_gridRelatedCondition->editDialog(preProcessorWindow());
		PreProcessorRawDataGroupDataItemInterface* i = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
		dialog->setWindowTitle(QString(tr("Edit %1 default value")).arg(i->condition()->caption()));
		dialog->setLabel(tr("Please input new default value."));
		dialog->setVariantValue(m_customValue);
		int ret = dialog->exec();
		if (ret == QDialog::Accepted) {
			iRICUndoStack::instance().push(new RawDataBackgroundEditValueCommand(this, m_type, m_customValue, m_type, dialog->variantValue()));
		}
		delete dialog;
	} else {
		RawDataBackgroundEditDialog dialog(preProcessorWindow());
		PreProcessorRawDataGroupDataItemInterface* i = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
		dialog.setWindowTitle(QString(tr("Edit %1 default value")).arg(i->condition()->caption()));
		GridAttributeEditWidget* widget = m_gridRelatedCondition->editWidget(0);
		dialog.setWidget(widget);
		dialog.setType(m_type);
		dialog.setCustomValue(m_customValue);
		dialog.setupDialog();;
		int ret = dialog.exec();
		if (ret == QDialog::Accepted) {
			iRICUndoStack::instance().push(new RawDataBackgroundEditValueCommand(this, m_type, m_customValue, dialog.type(), dialog.customValue()));
		}
	}
}

bool RawDataBackground::getValueRange(double* min, double* max)
{
	if (m_type == Custom) {
		*min = m_customValue.toDouble();
		*max = m_customValue.toDouble();
		return true;
	}
	return false;
}

QVariant RawDataBackground::variantValue()
{
	PreProcessorRawDataGroupDataItemInterface* gi = dynamic_cast<PreProcessorRawDataGroupDataItemInterface*>(parent()->parent());
	double min, max;
	gi->getValueRange(&min, &max);
	if (m_type == Maximum) {
		m_customValue = max;
	} else if (m_type == Minimum) {
		m_customValue = min;
	}
	return m_customValue;
}
