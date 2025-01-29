#include "projectdefaultcolormapsettings.h"

#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "../scalarstocolors/colormapsettingcontaineri.h"
#include "../scalarstocolors/colormapsettingcontainer.h"
#include <misc/stringtool.h>

ProjectDefaultColorMapSettings::ProjectDefaultColorMapSettings() :
	m_colorMaps {}
{}

ProjectDefaultColorMapSettings::~ProjectDefaultColorMapSettings()
{
	clear();
}


const std::unordered_map<std::string, ColorMapSettingContainerI*>& ProjectDefaultColorMapSettings::colorMaps() const
{
	return m_colorMaps;
}

ColorMapSettingContainerI* ProjectDefaultColorMapSettings::colorMap(const std::string& name) const
{
	auto it = m_colorMaps.find(name);
	if (it == m_colorMaps.end()) {return nullptr;}

	return it->second;
}

void ProjectDefaultColorMapSettings::add(const std::string& name, ColorMapSettingContainerI* colorMap)
{
	auto it = m_colorMaps.find(name);
	if (it != m_colorMaps.end()) {
		// delete old setting;
		delete it->second;
		m_colorMaps.erase(it);
	}

	auto c = colorMap->copy();
	m_colorMaps.insert({name, c});
}

void ProjectDefaultColorMapSettings::load(const QDomNode& node)
{
	clear();

	for (int i = 0; i < node.childNodes().size(); ++i) {
		auto child = node.childNodes().at(i);
		auto elem = child.toElement();
		auto name = iRIC::toStr(elem.attribute("name"));
		ColorMapSettingContainerI* cm = nullptr;
		if (elem.tagName() == "ColorMapSetting") {
			cm = new ColorMapSettingContainer();
		} else if (elem.tagName() == "EnumerateColorMapSetting") {
			cm = new ColorMapEnumerateSettingContainer();
		}

		if (cm != nullptr) {
			cm->load(elem);
			m_colorMaps.insert({name, cm});
		}
	}
}

void ProjectDefaultColorMapSettings::save(QXmlStreamWriter& writer)
{
	for (auto& pair : m_colorMaps) {
		auto cm = dynamic_cast<ColorMapSettingContainer*> (pair.second);
		auto cme = dynamic_cast<ColorMapEnumerateSettingContainer*> (pair.second);

		if (cm != nullptr) {
			writer.writeStartElement("ColorMapSetting");
		} else if (cme != nullptr) {
			writer.writeStartElement("EnumerateColorMapSetting");
		}
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);

		writer.writeEndElement();
	}
}

void ProjectDefaultColorMapSettings::clear()
{
	for (auto& pair : m_colorMaps) {
		delete pair.second;
	}

	m_colorMaps.clear();
}
