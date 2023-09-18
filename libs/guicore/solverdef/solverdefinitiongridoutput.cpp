#include "../scalarstocolors/colormapfactory.h"
#include "../scalarstocolors/colormapfactoryi.h"
#include "../scalarstocolors/colormapsettingcontaineri.h"
#include "../scalarstocolors/colormapsettingeditdialog.h"
#include "solverdefinitiongridoutput.h"
#include "private/solverdefinitiongridoutput_impl.h"

#include <misc/xmlsupport.h>

SolverDefinitionGridOutput::Impl::Impl(SolverDefinitionGridOutput* parent) :
	m_position {Position::Node},
	m_colorMapFactory {nullptr},
	m_parent {parent}
{}

SolverDefinitionGridOutput::Impl::~Impl()
{
	delete m_colorMapFactory;
}

void SolverDefinitionGridOutput::Impl::load(const QDomElement& elem)
{
	auto pos = elem.attribute("position", "node");
	if (pos == "node") {
		m_position = Position::Node;
	} else if (pos == "cell") {
		m_position = Position::CellCenter;
	} else if (pos == "edgei") {
		m_position = Position::EdgeI;
	} else if (pos == "edgej") {
		m_position = Position::EdgeJ;
	} else if (pos == "edgek") {
		m_position = Position::EdgeK;
	} else if (pos == "particle") {
		m_position = Position::Particle;
	} else if (pos == "polydata") {
		m_position = Position::PolyData;
	}
}

SolverDefinitionGridOutput::SolverDefinitionGridOutput(const std::string& name) :
	SolverDefinitionOutput(name),
	impl {new Impl {this}}
{
	impl->m_colorMapFactory = new ColorMapFactory();
}

SolverDefinitionGridOutput::SolverDefinitionGridOutput(const QDomElement& elem, SolverDefinition* solverDef, const SolverDefinitionTranslator& translator) :
	SolverDefinitionOutput(elem, solverDef, translator),
	impl {new Impl {this}}
{
	auto defElem = iRIC::getChildNode(elem, "Definition").toElement();

	impl->load(defElem);
	impl->m_colorMapFactory = new ColorMapFactory();

	auto s = iRIC::getChildNode(elem, "ColorMapSetting");
	if (! s.isNull()) {
		impl->m_defaultColorMapSetting = s.toElement();
	}
}

SolverDefinitionGridOutput::~SolverDefinitionGridOutput()
{}

SolverDefinitionGridOutput::Position SolverDefinitionGridOutput::position() const
{
	return impl->m_position;
}

QVariant SolverDefinitionGridOutput::attributeBrowserValue(double value) const
{
	return value;
}

ColorMapSettingContainerI* SolverDefinitionGridOutput::createColorMapSettingContainer() const
{
	auto cont = impl->m_colorMapFactory->createSettingContainer();
	setupColorMapSettingContainer(cont);

	if (! impl->m_defaultColorMapSetting.isNull()) {
		cont->load(impl->m_defaultColorMapSetting);
	}
	return cont;
}

ColorMapLegendSettingContainerI* SolverDefinitionGridOutput::createColorMapLegendSettingContainer() const
{
	return impl->m_colorMapFactory->createLegendSettingContainer();
}

ColorMapSettingEditWidgetI* SolverDefinitionGridOutput::createColorMapSettingEditWidget(QWidget* parent) const
{
	return impl->m_colorMapFactory->createSettingEditWidget(parent);
}

ColorMapSettingEditDialog* SolverDefinitionGridOutput::createColorMapSettingEditDialog(QWidget* parent) const
{
	auto dialog = new ColorMapSettingEditDialog(parent);
	dialog->setWidget(createColorMapSettingEditWidget(dialog));

	return dialog;
}

ColorMapSettingToolBarWidget* SolverDefinitionGridOutput::createColorMapSettingToolbarWidget(QWidget* parent) const
{
	return impl->m_colorMapFactory->createToolbarWidget(parent);
}

void SolverDefinitionGridOutput::setColorMapFactory(ColorMapFactoryI* factory)
{
	delete impl->m_colorMapFactory;
	impl->m_colorMapFactory = factory;
}

void SolverDefinitionGridOutput::setDefaultColorMapSetting(const QDomElement& elem)
{
	impl->m_defaultColorMapSetting = elem;
}

void SolverDefinitionGridOutput::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{}
