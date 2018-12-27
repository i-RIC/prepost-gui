#ifndef HYDRAULICDATACREATOR_H
#define HYDRAULICDATACREATOR_H

#include "../../guicore_global.h"

#include <QObject>

#include <vector>

class HydraulicData;
class HydraulicDataExporter;
class HydraulicDataImporter;
class HydraulicDataMultiExporter;
class ProjectDataItem;

class QDomNode;

class GUICOREDLL_EXPORT HydraulicDataCreator : public QObject
{
public:
	HydraulicDataCreator(const std::string& typeName, const QString& caption);
	virtual ~HydraulicDataCreator();

	const std::string& typeName() const;
	const QString& caption() const;

	virtual QString createName(unsigned int index) const = 0;
	virtual QString createCaption(unsigned int index) const = 0;

	virtual HydraulicData* create(ProjectDataItem* parent) = 0;

	const std::vector<HydraulicDataImporter*>& importers() const;
	std::vector<HydraulicDataImporter*>& importers();

	const std::vector<HydraulicDataExporter*>& exporters() const;
	std::vector<HydraulicDataExporter*>& exporters();

	const std::vector<HydraulicDataMultiExporter*>& multiExporters() const;
	std::vector<HydraulicDataMultiExporter*>& multiExporters();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/hydraulicdatacreator_impl.h"
#endif // _DEBUG

#endif // HYDRAULICDATACREATOR_H
