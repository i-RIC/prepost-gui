#ifndef HYDRAULICDATAMULTIEXPORTER_H
#define HYDRAULICDATAMULTIEXPORTER_H

#include "../../guicore_global.h"

#include <QObject>

#include <vector>

class HydraulicData;
class HydraulicDataCreator;
class ProjectData;

class QWidget;

class GUICOREDLL_EXPORT HydraulicDataMultiExporter : public QObject
{
public:
	HydraulicDataMultiExporter(const QString& caption, HydraulicDataCreator* creator);
	virtual ~HydraulicDataMultiExporter();

	const QString& caption() const;
	HydraulicDataCreator* creator() const;

	virtual const QStringList fileDialogFilters() = 0;
	virtual bool doExport(const std::vector<HydraulicData*>& data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) = 0;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/hydraulicdatamultiexporter_impl.h"
#endif // _DEBUG

#endif // HYDRAULICDATAMULTIEXPORTER_H
