#ifndef HYDRAULICDATAFACTORY_H
#define HYDRAULICDATAFACTORY_H

#include <QObject>

#include <vector>

class HydraulicDataCreator;

class HydraulicDataFactory : public QObject
{
private:
	HydraulicDataFactory();

public:
	const std::vector<HydraulicDataCreator*>& creators() const;
	static HydraulicDataFactory& instance();

private:
	std::vector<HydraulicDataCreator*> m_creators;
	static HydraulicDataFactory* m_instance;
};

#endif // HYDRAULICDATAFACTORY_H
