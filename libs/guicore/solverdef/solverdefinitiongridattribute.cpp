#include "../pre/gridcond/base/gridattributeeditdialog.h"
#include "../pre/gridcond/base/gridattributevariationeditdialog.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "../scalarstocolors/colortransferfunctioneditwidget.h"
#include "../scalarstocolors/lookuptablecontainer.h"
#include "../scalarstocolors/lookuptableeditwidget.h"
#include "../scalarstocolors/scalarstocolorseditdialog.h"
#include "solverdefinitiongridattribute.h"
#include "solverdefinitiongridattributedimension.h"
#include "solverdefinitiongridattributedimensioncreator.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinitiongridattribute_impl.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QSettings>

SolverDefinitionGridAttribute::Impl::Impl(const QDomElement& elem, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute *parent) :
	m_parent {parent}
{
	load(elem, translator);
}

SolverDefinitionGridAttribute::Impl::~Impl()
{
	for (SolverDefinitionGridAttributeDimension* dim : m_dimensions) {
		delete dim;
	}
}

void SolverDefinitionGridAttribute::Impl::load(const QDomElement& elem, const SolverDefinitionTranslator& translator)
{
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
				SolverDefinitionGridAttributeDimension* dim = SolverDefinitionGridAttributeDimensionCreator::create(childNode.toElement(), translator, m_parent);
				m_dimensions.append(dim);
			}
		}
	}
}

// Public interfaces

SolverDefinitionGridAttribute::SolverDefinitionGridAttribute(const QDomElement& elem, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order) :
	SolverDefinitionNode {elem, translator},
	m_impl {new Impl {elem, translator, this}}
{
	m_impl->m_position = pos;
	m_impl->m_isOption = isOption;
	m_impl->m_order = order;
}

SolverDefinitionGridAttribute::~SolverDefinitionGridAttribute()
{
	delete m_impl;
}

const std::string& SolverDefinitionGridAttribute::name() const
{
	return m_impl->m_name;
}

const std::string& SolverDefinitionGridAttribute::englishCaption() const
{
	return m_impl->m_englishCaption;
}

const QString& SolverDefinitionGridAttribute::caption() const
{
	return m_impl->m_caption;
}

const QVariant& SolverDefinitionGridAttribute::variantDefaultValue() const
{
	return m_impl->m_variantDefaultValue;
}

const QVariant& SolverDefinitionGridAttribute::variantMaximumValue() const
{
	return m_impl->m_variantMaximumValue;
}

const QVariant& SolverDefinitionGridAttribute::variantMinimumValue() const
{
	return m_impl->m_variantMinimumValue;
}

SolverDefinitionGridAttribute::Position SolverDefinitionGridAttribute::position() const
{
	return m_impl->m_position;
}

bool SolverDefinitionGridAttribute::isOption() const
{
	return m_impl->m_isOption;
}

int SolverDefinitionGridAttribute::order() const
{
	return m_impl->m_order;
}

const QList<SolverDefinitionGridAttributeDimension*>& SolverDefinitionGridAttribute::dimensions() const
{
	return m_impl->m_dimensions;
}

QList<SolverDefinitionGridAttributeDimension*>& SolverDefinitionGridAttribute::dimensions()
{
	return m_impl->m_dimensions;
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

void SolverDefinitionGridAttribute::setPosition(Position pos)
{
	m_impl->m_position = pos;
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
