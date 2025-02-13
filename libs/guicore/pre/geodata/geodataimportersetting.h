#ifndef GEODATAIMPORTERSETTING_H
#define GEODATAIMPORTERSETTING_H

#include "../../guicore_global.h"

class GUICOREDLL_EXPORT GeoDataImporterSetting
{
public:
	GeoDataImporterSetting();
	GeoDataImporterSetting(const std::string& name);
	virtual ~GeoDataImporterSetting();

	const std::string& name() const;

	bool copiedToProject() const;
	void setCopiedToProject(bool copied);

	QString fileName() const;
	void setFileName(const QString& fileName);

	QString selectedFilter() const;
	void setSelectedFilter(const QString& filter);

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

private:
	virtual void doLoadFromProjectMainFile(const QDomNode& node);
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer);

	std::string m_name;

	bool m_copiedToProject;
	QString m_fileName;
	QString m_selectedFilter;
};

#endif // GEODATAIMPORTERSETTING_H
