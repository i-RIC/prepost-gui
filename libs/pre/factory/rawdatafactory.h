#ifndef RAWDATAFACTORY_H
#define RAWDATAFACTORY_H

#include <QObject>
#include <QList>

class QDomNode;
class RawData;
class RawDataCreator;
class ProjectDataItem;
class SolverDefinitionGridAttribute;

class RawDataFactory : public QObject
{

private:
	/// Constructor
	/**
	 * this function is made to be private (Singleton pattern)
	 */
	RawDataFactory();

public:
	static RawDataFactory& instance() {
		if (m_instance == nullptr) {
			m_instance = new RawDataFactory();
		}
		return *m_instance;
	}
	const QList<RawDataCreator*> creators() const {return m_creators;}
	const QList<RawDataCreator*> compatibleCreators(SolverDefinitionGridAttribute* condition) const;
	RawData* restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const;

private:
	QList<RawDataCreator*> m_creators;
	static RawDataFactory* m_instance;
};

#endif // RAWDATAFACTORY_H
