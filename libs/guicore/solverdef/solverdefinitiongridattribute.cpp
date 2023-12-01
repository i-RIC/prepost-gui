#include "../pre/gridcond/base/gridattributeeditdialog.h"
#include "../pre/gridcond/base/gridattributeeditnameandvaluedialog.h"
#include "../pre/gridcond/base/gridattributevariationeditdialog.h"
#include "../scalarstocolors/colormapfactory.h"
#include "../scalarstocolors/colormapfactoryi.h"
#include "../scalarstocolors/colormapsettingcontaineri.h"
#include "../scalarstocolors/colormapsettingeditdialog.h"
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

SolverDefinitionGridAttribute::Impl::Impl(SolverDefinitionGridAttribute* parent) :
	m_isReferenceInformation {false},
	m_colorMapFactory {nullptr},
	m_parent {parent}
{}

SolverDefinitionGridAttribute::Impl::Impl(const QDomElement& elem, SolverDefinition *solverDef, SolverDefinitionGridAttribute *parent) :
	Impl {parent}
{
	load(elem, solverDef);
}

SolverDefinitionGridAttribute::Impl::~Impl()
{
	for (SolverDefinitionGridAttributeDimension* dim : m_dimensions) {
		delete dim;
	}

	delete m_colorMapFactory;
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
		m_isDirection = iRIC::getBooleanAttribute(e, "direction");
		m_variantDefaultValue = e.attribute("default", "");
		m_variantMaximumValue = e.attribute("max", "");
		m_variantMinimumValue = e.attribute("min", "");
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

SolverDefinitionGridAttribute::SolverDefinitionGridAttribute() :
	SolverDefinitionNode {},
	impl {new Impl {this}}
{
	setColorMapFactory(new ColorMapFactory());
}

SolverDefinitionGridAttribute::SolverDefinitionGridAttribute(const QDomElement& elem, SolverDefinition* definition, Position pos, bool isOption, int order) :
	SolverDefinitionNode {elem, definition},
	impl {new Impl {elem, definition, this}}
{
	impl->m_position = pos;
	impl->m_isOption = isOption;
	impl->m_order = order;

	setColorMapFactory(new ColorMapFactory());
}

SolverDefinitionGridAttribute::~SolverDefinitionGridAttribute()
{}

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

void SolverDefinitionGridAttribute::setCaption(const QString& cap)
{
	impl->m_caption = cap;
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

bool SolverDefinitionGridAttribute::isDirection() const
{
	return impl->m_isDirection;
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

GridAttributeContainer* SolverDefinitionGridAttribute::container(v4InputGrid* grid)
{
	return buildContainer(grid);
}

GridAttributeEditDialog* SolverDefinitionGridAttribute::editDialog(QWidget* parent)
{
	auto dialog = new GridAttributeEditDialog(parent);
	auto widget = editWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

GridAttributeEditNameAndValueDialog* SolverDefinitionGridAttribute::editNameAndValueDialog(QWidget* parent)
{
	auto dialog	= new GridAttributeEditNameAndValueDialog(parent);
	auto w = editWidget(dialog);
	dialog->setWidget(w);
	return dialog;
}

GridAttributeVariationEditDialog* SolverDefinitionGridAttribute::variationEditDialog(QWidget* parent)
{
	auto dialog = new GridAttributeVariationEditDialog(parent);
	auto widget = variationEditWidget(dialog);
	dialog->setWidget(widget);
	return dialog;
}

QVariant SolverDefinitionGridAttribute::colorMapValue(const QVariant& value) const
{
	return value;
}

ColorMapSettingContainerI* SolverDefinitionGridAttribute::createColorMapSettingContainer() const
{
	auto cont = impl->m_colorMapFactory->createSettingContainer();
	cont->valueCaption = impl->m_caption;
	setupColorMapSettingContainer(cont);
	return cont;
}

ColorMapLegendSettingContainerI* SolverDefinitionGridAttribute::createColorMapLegendSettingContainer() const
{
	return impl->m_colorMapFactory->createLegendSettingContainer();
}

ColorMapSettingEditWidgetI* SolverDefinitionGridAttribute::createColorMapSettingEditWidget(QWidget* parent) const
{
	return impl->m_colorMapFactory->createSettingEditWidget(parent);
}

ColorMapSettingEditDialog* SolverDefinitionGridAttribute::createColorMapSettingEditDialog(QWidget* parent) const
{
	auto dialog = new ColorMapSettingEditDialog(parent);
	dialog->setWidget(createColorMapSettingEditWidget(dialog));

	return dialog;
}

ColorMapSettingToolBarWidget* SolverDefinitionGridAttribute::createColorMapSettingToolbarWidget(QWidget* parent) const
{
	return impl->m_colorMapFactory->createToolbarWidget(parent);
}

void SolverDefinitionGridAttribute::setPosition(Position pos)
{
	impl->m_position = pos;
}

void SolverDefinitionGridAttribute::setColorMapFactory(ColorMapFactoryI* factory)
{
	if (impl->m_colorMapFactory != nullptr) {
		delete impl->m_colorMapFactory;
	}

	impl->m_colorMapFactory = factory;
}

void SolverDefinitionGridAttribute::setVariantDefaultValue(const QVariant& v)
{
	impl->m_variantDefaultValue = v;
}

void SolverDefinitionGridAttribute::setupColorMapSettingContainer(ColorMapSettingContainerI*) const
{}
