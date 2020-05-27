#include "geodatapolydata.h"
#include "private/geodatapolydata_editnameandvaluecommand.h"
#include "private/geodatapolydata_impl.h"
#include "private/geodatapolydata_setcolorsettingcommand.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/base/gridattributeeditnameandvaluedialog.h>

#include <QAction>
#include <QInputDialog>

GeoDataPolyData::Impl::Impl(GeoDataPolyData* parent) :
	m_editNameAction {new QAction(GeoDataPolyData::tr("Edit &Name"), parent)},
	m_editValueAction {new QAction(GeoDataPolyData::tr("Edit &Value..."), parent)},
	m_editNameAndValueAction {new QAction(GeoDataPolyData::tr("Edit &Name and Value..."), parent)},
	m_editColorSettingAction {new QAction(GeoDataPolyData::tr("Color &Setting..."), parent)}
{
	connect(m_editNameAction, SIGNAL(triggered()), parent, SLOT(editName()));
	connect(m_editValueAction, SIGNAL(triggered()), parent, SLOT(editValue()));
	connect(m_editNameAndValueAction, SIGNAL(triggered()), parent, SLOT(editNameAndValue()));
	connect(m_editColorSettingAction, SIGNAL(triggered()), parent, SLOT(editColorSetting()));
}

GeoDataPolyData::GeoDataPolyData(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition),
	impl {new Impl {this}}
{
	setupValues();
}

GeoDataPolyData::~GeoDataPolyData()
{
	delete impl;
}

void GeoDataPolyData::setCaptionAndEmitEdited(const QString& caption)
{
	pushCommand(new EditNameAndValueCommand(caption, variantValue(), this));
}

void GeoDataPolyData::setVariantValueAndEmitEdited(const QVariant& value)
{
	pushCommand(new EditNameAndValueCommand(caption(), value, this));
}

QColor GeoDataPolyData::color() const
{
	return impl->m_setting.color;
}

QDialog* GeoDataPolyData::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolyDataColorSettingDialog(parent);
	dialog->setWindowTitle(tr("%1 Color Setting").arg(shapeNameCamelCase()));
	dialog->setSetting(impl->m_setting);
	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		dialog->setIsReferenceInformation(gridAtt->isReferenceInformation());
	}

	return dialog;
}

void GeoDataPolyData::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GeoDataPolyDataColorSettingDialog*>(d);
	pushRenderCommand(new SetColorSettingCommand(dialog->setting(), this));
}

const QVariant& GeoDataPolyData::variantValue() const
{
	int index = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	return impl->m_variantValues.at(index);
}

void GeoDataPolyData::setVariantValue(const QVariant& v, bool disableInform)
{
	int index = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		index = dims->currentIndex();
	}
	impl->m_variantValues[index] = v;
	updateScalarValues();
	if (! disableInform) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent());
		p->informValueRangeChange();
		p->informDataChange();
	}
}

bool GeoDataPolyData::getValueRange(double* min, double* max)
{
	*min = variantValue().toDouble();
	*max = variantValue().toDouble();
	return true;
}

void GeoDataPolyData::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPolyData::editName()
{
	if (! m_gridAttribute || ! m_gridAttribute->isReferenceInformation()) {return;}

	bool ok;
	QString newCaption = QInputDialog::getText(preProcessorWindow(), tr("Edit name"), tr("Name:"), QLineEdit::Normal, caption(), &ok);
	if (! ok) {return;}

	pushCommand(new EditNameAndValueCommand(newCaption, variantValue(), this));
}

void GeoDataPolyData::editValue()
{
	if (m_gridAttribute && m_gridAttribute->isReferenceInformation()) {return;}

	auto dialog = m_gridAttribute->editDialog(preProcessorWindow());
	PreProcessorGeoDataGroupDataItemInterface* i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setLabel(tr("Please input new value in this %1.").arg(shapeName()));
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());

	if (! isReady()) {
		dialog->disableCancel();
	}

	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {return;}

	pushCommand(new EditNameAndValueCommand(caption(), dialog->variantValue(), this));

	delete dialog;
}

void GeoDataPolyData::editNameAndValue()
{
	if (m_gridAttribute && m_gridAttribute->isReferenceInformation()) {return;}

	auto dialog = m_gridAttribute->editNameAndValueDialog(preProcessorWindow());
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setName(caption());
	i->setupEditWidget(dialog->widget());
	dialog->setVariantValue(variantValue());
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {
		delete dialog;
		return;
	}
	pushCommand(new EditNameAndValueCommand(dialog->name(), dialog->variantValue(), this));

	delete dialog;
}

void GeoDataPolyData::handleDimensionCurrentIndexChange(int oldIndex, int newIndex)
{
	// @todo implement this!
}

void GeoDataPolyData::handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after)
{
	// @todo implement this!
}

void GeoDataPolyData::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPolyData::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_setting.load(node);
}

void GeoDataPolyData::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_setting.save(writer);
}

void GeoDataPolyData::setMapping(GeoDataPolyDataColorSettingDialog::Mapping m)
{
	impl->m_setting.mapping = m;
	updateActorSettings();
}

void GeoDataPolyData::setOpacity(int opacity)
{
	impl->m_setting.opacity = opacity;
	updateActorSettings();
}

void GeoDataPolyData::setColor(const QColor& color)
{
	impl->m_setting.color = color;
	updateActorSettings();
}

GeoDataPolyDataColorSettingDialog::Setting GeoDataPolyData::colorSetting() const
{
	return impl->m_setting;
}

void GeoDataPolyData::setColorSetting(GeoDataPolyDataColorSettingDialog::Setting setting)
{
	impl->m_setting = setting;
}

void GeoDataPolyData::setupValues()
{
	impl->m_variantValues.clear();

	int maxIndex = 0;
	GridAttributeDimensionsContainer* dims = dimensions();
	if (dims != nullptr) {
		maxIndex = dimensions()->maxIndex();
	}
	double defaultValue = 0;

	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		bool ok;
		defaultValue = gridAtt->variantDefaultValue().toDouble(&ok);
		if (! ok) {defaultValue = 0;}
	}

	for (int i = 0; i <= maxIndex; ++i) {
		impl->m_variantValues.push_back(defaultValue);
	}
}

const std::vector<QVariant>& GeoDataPolyData::variantValues() const
{
	return impl->m_variantValues;
}

std::vector<QVariant>& GeoDataPolyData::variantValues()
{
	return impl->m_variantValues;
}

void GeoDataPolyData::emitNameAndValueEdited()
{
	emit nameAndValueEdited();
}

QAction* GeoDataPolyData::editNameAction() const
{
	return impl->m_editNameAction;
}

QAction* GeoDataPolyData::editValueAction() const
{
	return impl->m_editValueAction;
}

QAction* GeoDataPolyData::editNameAndValueAction() const
{
	return impl->m_editNameAndValueAction;
}

QAction* GeoDataPolyData::editColorSettingAction() const
{
	return impl->m_editColorSettingAction;
}

void GeoDataPolyData::updateScalarValues()
{}
