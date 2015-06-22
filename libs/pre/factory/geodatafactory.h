#ifndef GEODATAFACTORY_H
#define GEODATAFACTORY_H

#include <QObject>
#include <QList>

class QDomNode;
class GeoData;
class GeoDataCreator;
class ProjectDataItem;
class SolverDefinitionGridAttribute;

class GeoDataFactory : public QObject
{

private:
	/// Constructor
	/**
	 * this function is made to be private (Singleton pattern)
	 */
	GeoDataFactory();

public:
	static GeoDataFactory& instance() {
		if (m_instance == nullptr) {
			m_instance = new GeoDataFactory();
		}
		return *m_instance;
	}
	const QList<GeoDataCreator*> creators() const {return m_creators;}
	const QList<GeoDataCreator*> compatibleCreators(SolverDefinitionGridAttribute* condition) const;
	GeoData* restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const;

private:
	QList<GeoDataCreator*> m_creators;
	static GeoDataFactory* m_instance;
};

#endif // GEODATAFACTORY_H
