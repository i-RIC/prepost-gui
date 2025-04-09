#ifndef GEODATAIMPORTERSETTING_H
#define GEODATAIMPORTERSETTING_H

#include "../../guicore_global.h"

#include <QObject>

class GeoDataCreator;

class GUICOREDLL_EXPORT GeoDataImporterSetting : public QObject
{
	Q_OBJECT

public:
	struct Item {
		QString name;
		QString value;
	};

	GeoDataImporterSetting();
	virtual ~GeoDataImporterSetting();

	const std::string& name() const;
	void setName(const std::string& name);

	QString fileName() const;
	void setFileName(const QString& fileName);

	QString selectedFilter() const;
	void setSelectedFilter(const QString& filter);

	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer);

	std::vector<Item> items(GeoDataCreator* creator) const;

private:
	virtual void doLoadFromProjectMainFile(const QDomNode& node);
	virtual void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	virtual std::vector<Item> customItems() const;

	std::string m_name;

	QString m_fileName;
	QString m_selectedFilter;
};

#endif // GEODATAIMPORTERSETTING_H
