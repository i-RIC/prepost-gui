#ifndef HYDRAULICDATAIMPORTERFACTORY_H
#define HYDRAULICDATAIMPORTERFACTORY_H

#include <QObject>
#include <QList>

class HydraulicDataImporter;

class HydraulicDataImporterFactory : public QObject
{

private:
	/// Constructor
	/**
	 * this function is made to be private (Singleton pattern)
	 */
	HydraulicDataImporterFactory();

public:
	static HydraulicDataImporterFactory& instance() {
		if (m_instance == nullptr) {
			m_instance = new HydraulicDataImporterFactory();
		}
		return *m_instance;
	}
	const QList<HydraulicDataImporter*> importers() const {return m_importers;}

private:
	QList<HydraulicDataImporter*> m_importers;
	static HydraulicDataImporterFactory* m_instance;
};

#endif // GEODATAFACTORY_H
