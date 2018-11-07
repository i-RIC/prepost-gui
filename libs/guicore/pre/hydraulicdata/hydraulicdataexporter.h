#ifndef HYDRAULICDATAEXPORTER_H
#define HYDRAULICDATAEXPORTER_H

#include "../../guicore_global.h"

#include <QObject>

class HydraulicData;
class HydraulicDataCreator;
class ProjectData;

class QWidget;

class GUICOREDLL_EXPORT HydraulicDataExporter : public QObject
{
public:
	HydraulicDataExporter(const QString& caption, HydraulicDataCreator* creator);
	virtual ~HydraulicDataExporter();

	const QString& caption() const;
	HydraulicDataCreator* creator() const;

	virtual const QStringList fileDialogFilters() = 0;
	virtual bool doExport(HydraulicData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) = 0;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/hydraulicdataexporter_impl.h"
#endif // _DEBUG

#endif // HYDRAULICDATAEXPORTER_H
