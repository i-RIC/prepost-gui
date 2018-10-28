#include "../pre/gridcond/base/gridattributeeditdialog.h"
#include "../pre/gridcond/base/gridattributevariationeditdialog.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "../scalarstocolors/colortransferfunctioneditwidget.h"
#include "../scalarstocolors/lookuptablecontainer.h"
#include "../scalarstocolors/lookuptableeditwidget.h"
#include "../scalarstocolors/scalarstocolorseditdialog.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattribute.h"
#include "solverdefinitiongridattributedimension.h"
#include "solverdefinitiongridattributedimensioncreator.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinitiongridattribute_impl.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QSettings>

SolverDefinitionGridAttribute::Impl::Impl(const QDomElement& elem, SolverDefinition *solverDef, SolverDefinitionGridAttribute *parent) :
	m_isReferenceInformation {false},
	m_solverDefinition {solverDef},
	m_parent {parent}
{
	load(elem, solverDef);
}

SolverDefinitionGridAttribute::Impl::~Impl()
{
	for (SolverDefinitionGridAttributeDimension* dim : m_dimensions) {
		delete dim;
	}
}

void SolverDefinitionGridAttribute::Impl::load(const QDomElement& elem, SolverDefinition* solverDef)
{
	SolverDefinitionTranslator translator = solverDef->buildTranslator();

	m_name = iRIC::toStr(elem.attribute("name"));
	m_englishCaption = iRIC::toStr(elem.attribute("caption"));
	m_caption = translator.translate(elem.attribute("caption"));
	QDomNode defNode = iRIC::getChildNode(elem, "Definition");
	if (! defNode.isNull()) {
		QDomElement e = defNode.toElement();
		m_variantDefaultValue = e.attribute("default", "");
		m_variantMaximumValue = e.attribute("max", "");
		m_variantMaximumValue = e.attribute("min", "");
		QDomNodeList children = e.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomNode childNode = children.at(i);
			if (childNode.nodeName() == "Dimension") {
				// add dimension
				auto dim = SolverDefinitionGridAttributeDimensionCreator::create(childNode.toElement(), solverDef, m_parent);
				m_dimensions.push_back(dim);
			}
		}
		m_mapping = e.attribute("mapping");
		m_mappingArgs = e.attribute("mappingArgs");
	}
}

// Public interfaces

SolverDefinitionGridAttribute::SolverDefinitionGridAttribute(const QDomElement& elem, SolverDefinition* definition, Position pos, bool isOption, int order) :
	SolverDefinitionNode {elem, definition},
	impl {new Impl {elem, definition, this}}
{
	impl->m_position = pos;
	impl->m_isOption = isOption;
	impl->m_order = order;
}

SolverDefinitionGridAttribute::~SolverDefinitionGridAttribute()
{
	delete impl;
}

const std::string& SolverDefinitionGridAttribute::name() const
{
	return impl->m_name;
}

const std::string& SolverDefinitionGridAttribute::englishCaption() const
{
	return impl->m_englishCaption;
}

const QString& SolverDefinitionGridAttribute::caption() const
{
	return impl->m_caption;
}

const QVariant& SolverDefinitionGridAttribute::variantDefaultValue() const
{
	return impl->m_variantDefaultValue;
}

const QVariant& SolverDefinitionGridAttribute::variantMaximumValue() const
{
	return impl->m_variantMaximumValue;
}

const QVariant& SolverDefinitionGridAttribute::variantMinimumValue() const
{
	return impl->m_variantMinimumValue;
}

SolverDefinitionGridAttribute::Position SolverDefinitionGridAttribute::position() const
{
	return impl->m_position;
}

bool SolverDefinitionGridAttribute::isOption() const
{
	return impl->m_isOption;
}

const QString& SolverDefinitionGridAttribute::mapping() const
{
	return impl->m_mapping;
}

const QString& SolverDefinitionGridAttribute::mappingArgs() const
{
	return impl->m_mappingArgs;
}

int SolverDefinitionGridAttribute::order() const
{
	return impl->m_order;
}

bool SolverDefinitionGridAttribute::isReferenceInformation() const
{
	return impl->m_isReferenceInformation;
}

void SolverDefinitionGridAttribute::setIsReferenceInformation(bool ref)
{
	impl->m_isReferenceInformation = ref;
}

const std::vector<SolverDefinitionGridAttributeDimension*>& SolverDefinitionGridAttribute::dimensions() const
{
	return impl->m_dimensions;
}

std::vector<SolverDefinitionGridAttributeDimension *>& SolverDefinitionGridAttribute::dimensions()
{
	return impl->m_dimensions;
}

GridAttributeContainer* SolverDefinitionGridAttribute::container(Grid* grid)
{
	return buildContainer(grid);
}

GridAttributeEditDialog* SolverDefinitionGridAttribute::editDialog(QWidget* parent)
{
	GridAttributeEditDialog* dialog = new GridAttributeEditDialog(parent);
	GridAttributeEditWidget* widget = editWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

GridAttributeVariationEditDialog* SolverDefinitionGridAttribute::variationEditDialog(QWidget* parent)
{
	GridAttributeVariationEditDialog* dialog = new GridAttributeVariationEditDialog(parent);
	GridAttributeVariationEditWidget* widget = variationEditWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

ScalarsToColorsContainer* SolverDefinitionGridAttribute::createScalarsToColorsContainer(ProjectDataItem* d)
{
	LookupTableContainer* container = new LookupTableContainer(d);
	QSettings setting;
	int cm = setting.value("graphics/colormap", static_cast<int>(ColorMapSettingWidget::Rainbow)).value<int>();
	ColorMapSettingWidget::ColorMap cmVal = static_cast<ColorMapSettingWidget::ColorMap>(cm);
	container->setColorMap(cmVal);
	return container;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttribute::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createLookupTableEditWidget(parent);
}

ScalarsToColorsEditDialog* SolverDefinitionGridAttribute::createScalarsToColorsEditDialog(QWidget* parent) const
{
	ScalarsToColorsEditDialog* dialog = new ScalarsToColorsEditDialog(parent);
	ScalarsToColorsEditWidget* widget = createScalarsToColorsEditWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

SolverDefinition* SolverDefinitionGridAttribute::solverDefinition() const
{
	return impl->m_solverDefinition;
}

void SolverDefinitionGridAttribute::setPosition(Position pos)
{
	impl->m_position = pos;
}

ColorTransferFunctionContainer* SolverDefinitionGridAttribute::createColorTransferFunctionContainer(ProjectDataItem* d)
{
	return new ColorTransferFunctionContainer(d);
}

LookupTableContainer* SolverDefinitionGridAttribute::createLookupTableContainer(ProjectDataItem* d) const
{
	LookupTableContainer* container = new LookupTableContainer(d);
	QSettings setting;
	int cm = setting.value("graphics/colormap", static_cast<int>(ColorMapSettingWidget::Rainbow)).value<int>();
	ColorMapSettingWidget::ColorMap cmVal = static_cast<ColorMapSettingWidget::ColorMap>(cm);
	container->setColorMap(cmVal);
	return container;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttribute::createColorTransferFunctionEditWidget(QWidget* parent) const
{
	return new ColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttribute::createLookupTableEditWidget(QWidget* parent) const
{
	LookupTableEditWidget* widget = new LookupTableEditWidget(parent);
	widget->hideFillSettings();
	widget->hideDivisionNumber();
	return widget;
}
