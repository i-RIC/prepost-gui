#include "gridcomplexconditiongroup.h"
#include "private/gridcomplexconditiongroup_impl.h"
#include "../../project/inputcond/inputconditionpage.h"
#include "../../solverdef/solverdefinition.h"
#include "../../solverdef/solverdefinitiontranslator.h"

GridComplexConditionGroup::Impl::Impl(SolverDefinition* def, const QDomElement& elem) :
	m_page {nullptr},
	m_widgetSet {},
	m_caption {},
	m_color {},
	m_isDefault {},
	m_containerSet {}
{
	auto t = def->buildTranslator();

	m_caption.setName("_caption");
	m_color.setName("_color");
	m_isDefault.setName("_isdefault");

	m_containerSet.setup(elem, *def, t, true);
	m_widgetSet.setup(elem, m_containerSet, *def, t, true);

	m_page = new InputConditionPage(elem, &m_widgetSet, t, nullptr);
}

GridComplexConditionGroup::Impl::~Impl()
{
	m_widgetSet.clear();
	m_containerSet.clear();
	delete m_page;
}

// public interfaces

GridComplexConditionGroup::Setting::Setting() :
	containerSet {nullptr}
{}

GridComplexConditionGroup::Setting::Setting(const Setting& s) :
	caption {s.caption},
	color {s.color},
	isDefault {s.isDefault},
	containerSet {s.containerSet->clone()}
{}

GridComplexConditionGroup::Setting::Setting(Setting&& s) :
	caption {s.caption},
	color {s.color},
	isDefault {s.isDefault},
	containerSet {s.containerSet}
{
	s.containerSet = nullptr;
}

GridComplexConditionGroup::Setting::~Setting()
{
	delete containerSet;
}

GridComplexConditionGroup::GridComplexConditionGroup(SolverDefinition* def, const QDomElement& elem) :
	impl {new Impl{def, elem}}
{}

GridComplexConditionGroup::~GridComplexConditionGroup()
{
	delete impl;
}

void GridComplexConditionGroup::setNameAndNumber(const std::string& name, int number)
{
	impl->m_caption.setComplexProperty(name, number);
	impl->m_color.setComplexProperty(name, number);
	impl->m_isDefault.setComplexProperty(name, number);
	impl->m_containerSet.setComplexProperty(name, number);
}

void GridComplexConditionGroup::load()
{
	impl->m_caption.load();
	impl->m_color.load();
	impl->m_isDefault.load();
	impl->m_containerSet.load();
}

void GridComplexConditionGroup::save()
{
	impl->m_caption.load();
	impl->m_color.load();
	impl->m_isDefault.load();
	impl->m_containerSet.load();
}

QWidget* GridComplexConditionGroup::widget() const
{
	return impl->m_page;
}

InputConditionContainerSet* GridComplexConditionGroup::containerSet() const
{
	return &(impl->m_containerSet);
}

QString GridComplexConditionGroup::caption() const
{
	return impl->m_caption.value();
}

void GridComplexConditionGroup::setCaption(const QString& caption)
{
	impl->m_caption.setValue(caption);
}

QColor GridComplexConditionGroup::color() const
{
	return QColor(impl->m_color.value());
}

void GridComplexConditionGroup::setColor(const QColor& color)
{
	impl->m_color.setValue(color.name());
}

bool GridComplexConditionGroup::isDefault() const
{
	return impl->m_isDefault.value() == 1;
}

void GridComplexConditionGroup::setIsDefault(bool isDefault)
{
	impl->m_isDefault.setValue(isDefault ? 1 : 0);
}

GridComplexConditionGroup::Setting GridComplexConditionGroup::setting() const
{
	Setting setting;
	setting.caption = impl->m_caption.value();
	setting.color = QColor(impl->m_color.value());
	setting.isDefault = (impl->m_isDefault.value() == 1);
	setting.containerSet = impl->m_containerSet.clone();

	return setting;
}

void GridComplexConditionGroup::setSetting(const Setting& setting)
{
	impl->m_caption.setValue(setting.caption);
	impl->m_color.setValue(setting.color.name());
	impl->m_isDefault.setValue(setting.isDefault ? 1 : 0);
	impl->m_containerSet.copyValues(setting.containerSet);
}
