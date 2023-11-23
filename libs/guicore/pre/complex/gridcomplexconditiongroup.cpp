#include "gridcomplexconditiongroup.h"
#include "private/gridcomplexconditiongroup_impl.h"
#include "../../project/inputcond/inputconditionpage.h"
#include "../../project/inputcond/inputconditionwidget.h"
#include "../../solverdef/solverdefinition.h"
#include "../../solverdef/solverdefinitiontranslator.h"

#include <misc/stringtool.h>

#include <QDomElement>

#include <iriclib_errorcodes.h>

#include <unordered_map>

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
	m_containerSetBackup.setup(elem, *def, t, true);
	m_widgetSet.setup(elem, m_containerSet, *def, t, true);
	m_tableWidgetSet.setup(elem, m_containerSet, *def, t, true, true);

	auto names = widgetNames(elem);
	for (const auto& name : names) {
		auto w = m_tableWidgetSet.widget(name);
		m_tableWidgets.push_back(w);

		QWidget* label = nullptr;
		auto labelName = QString("%0_caption").arg(name.c_str());
		auto l = m_tableWidgetSet.widget(iRIC::toStr(labelName));
		if (l != nullptr) {
			label = l;
		}
		m_tableLabels.push_back(label);
	}

	std::unordered_map<std::string, QPushButton*> emptyButtons;

	m_page = new InputConditionPage(elem, &m_widgetSet, emptyButtons, t, nullptr);
}

GridComplexConditionGroup::Impl::~Impl()
{
	m_widgetSet.clear();
	m_containerSet.clear();
	delete m_page;
}

std::vector<std::string> GridComplexConditionGroup::Impl::widgetNames(const QDomElement& elem)
{
	std::vector<std::string> ret;
	widgetNamesRec(elem, &ret);

	return ret;
}

void GridComplexConditionGroup::Impl::widgetNamesRec(const QDomElement& elem, std::vector<std::string>* names)
{
	if (elem.nodeName() == "Item") {
		names->push_back(iRIC::toStr(elem.attribute("name")));
	} else {
		auto children = elem.childNodes();
		for (int i = 0; i < children.size(); ++i) {
			auto c = children.at(i);
			if (c.nodeType() != QDomNode::NodeType::ElementNode) {continue;}

			widgetNamesRec(c.toElement(), names);
		}
	}
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

GridComplexConditionGroup::Setting GridComplexConditionGroup::Setting::copy() const
{
	Setting ret(*this);
	return ret;
}

GridComplexConditionGroup::Setting& GridComplexConditionGroup::Setting::operator=(const Setting& setting)
{
	caption = setting.caption;
	color = setting.color;
	isDefault = setting.isDefault;
	containerSet->copyValues(setting.containerSet);

	return *this;
}

GridComplexConditionGroup::GridComplexConditionGroup(SolverDefinition* def, const QDomElement& elem) :
	impl {new Impl{def, elem}}
{}

GridComplexConditionGroup::~GridComplexConditionGroup()
{}

int GridComplexConditionGroup::load(const iRICLib::H5CgnsConditionGroup& group)
{
	int ier = 0;
	ier = impl->m_caption.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = impl->m_color.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = impl->m_isDefault.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = impl->m_containerSet.load(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	return IRIC_NO_ERROR;
}

int GridComplexConditionGroup::save(iRICLib::H5CgnsConditionGroup* group)
{
	int ier = 0;
	ier = impl->m_caption.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	impl->m_color.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	impl->m_isDefault.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	impl->m_containerSet.save(group);
	if (ier != IRIC_NO_ERROR) {return ier;}

	return IRIC_NO_ERROR;
}

QWidget* GridComplexConditionGroup::widget() const
{
	return impl->m_page;
}

const std::vector<QWidget*>& GridComplexConditionGroup::tableLabels() const
{
	return impl->m_tableLabels;
}

const std::vector<QWidget*>& GridComplexConditionGroup::tableWidgets() const
{
	return impl->m_tableWidgets;
}

InputConditionContainerSet* GridComplexConditionGroup::containerSet() const
{
	return &(impl->m_containerSet);
}

std::vector<std::string> GridComplexConditionGroup::widgetNames(const QDomElement& elem)
{
	return Impl::widgetNames(elem);
}

QString GridComplexConditionGroup::caption() const
{
	return impl->m_caption.value();
}

void GridComplexConditionGroup::setCaption(const QString& caption)
{
	auto oldCaption = impl->m_caption.value();
	impl->m_caption.setValue(caption);

	if (oldCaption != caption) {
		emit captionChanged(caption);
	}
}

QColor GridComplexConditionGroup::color() const
{
	return QColor(impl->m_color.value());
}

void GridComplexConditionGroup::setColor(const QColor& color)
{
	auto oldColor = impl->m_color.value();
	impl->m_color.setValue(color.name());

	if (oldColor != color.name()) {
		emit colorChanged(color);
	}
}

bool GridComplexConditionGroup::isDefault() const
{
	return impl->m_isDefault.value() == 1;
}

void GridComplexConditionGroup::setIsDefault(bool isDefault)
{
	int oldIsDefault = impl->m_isDefault.value();
	int intIsDefault = isDefault ? 1 : 0;
	impl->m_isDefault.setValue(intIsDefault);

	if (oldIsDefault != intIsDefault) {
		emit isDefaultChanged(isDefault);
	}
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

void GridComplexConditionGroup::backup()
{
	impl->m_captionBackup = impl->m_caption;
	impl->m_colorBackup = impl->m_color;
	impl->m_isDefaultBackup = impl->m_isDefault;

	impl->m_containerSetBackup.copyValues(&impl->m_containerSet);
}

void GridComplexConditionGroup::restore()
{
	impl->m_caption = impl->m_captionBackup;
	impl->m_color = impl->m_colorBackup;
	impl->m_isDefault = impl->m_isDefaultBackup;

	impl->m_containerSet.copyValues(&impl->m_containerSetBackup);
}

void GridComplexConditionGroup::setSetting(const Setting& setting)
{
	impl->m_caption.setValue(setting.caption);
	impl->m_color.setValue(setting.color.name());
	impl->m_isDefault.setValue(setting.isDefault ? 1 : 0);
	impl->m_containerSet.copyValues(setting.containerSet);
}
