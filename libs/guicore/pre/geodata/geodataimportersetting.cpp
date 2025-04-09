#include "geodatacreator.h"
#include "geodataimportersetting.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

GeoDataImporterSetting::GeoDataImporterSetting() :
	m_name {},
	m_fileName {},
	m_selectedFilter {}
{}

GeoDataImporterSetting::~GeoDataImporterSetting()
{}

const std::string& GeoDataImporterSetting::name() const
{
	return m_name;
}

void GeoDataImporterSetting::setName(const std::string& name)
{
	m_name = name;
}

QString GeoDataImporterSetting::fileName() const
{
	return m_fileName;
}

void GeoDataImporterSetting::setFileName(const QString& fileName)
{
	m_fileName = fileName;
}

QString GeoDataImporterSetting::selectedFilter() const
{
	return m_selectedFilter;
}

void GeoDataImporterSetting::setSelectedFilter(const QString& filter)
{
	m_selectedFilter = filter;
}

void GeoDataImporterSetting::loadFromProjectMainFile(const QDomNode& node)
{
	m_name = iRIC::toStr(node.toElement().attribute("name"));
	m_fileName = node.toElement().attribute("fileName");
	m_selectedFilter = node.toElement().attribute("selectedFilter");

	doLoadFromProjectMainFile(node);
}

void GeoDataImporterSetting::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_name.c_str());
	writer.writeAttribute("fileName", m_fileName);
	writer.writeAttribute("selectedFilter", m_selectedFilter);

	doSaveToProjectMainFile(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataImporterSetting::items(GeoDataCreator* creator) const
{
	std::vector<Item> ret;
	ret.push_back(Item {tr("File name"), m_fileName});
	QString copied;
	if (creator->isReadOnly()) {
		copied = tr("No");
	} else {
		copied = tr("Yes");
	}
	ret.push_back(Item {tr("Saved to to project"), copied});
	for (auto item : customItems()) {
		ret.push_back(item);
	}
	return ret;
}

void GeoDataImporterSetting::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

std::vector<GeoDataImporterSetting::Item> GeoDataImporterSetting::customItems() const
{
	std::vector<Item> ret;

	return ret;
}
