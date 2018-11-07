#ifndef HYDRAULICDATAIMPORTER_H
#define HYDRAULICDATAIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

#include <set>
#include <string>

class HydraulicData;
class HydraulicDataCreator;

class GUICOREDLL_EXPORT HydraulicDataImporter : public QObject
{
public:
	HydraulicDataImporter(HydraulicDataCreator* creator);
	virtual ~HydraulicDataImporter();

	HydraulicDataCreator* creator() const;

	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;

	bool init(const QString& filename, const QString& selectedFilter, int* count, QWidget* w);
	virtual bool importData(HydraulicData* data, int index, const std::set<QString>& usedCaptions, QWidget* w) = 0;

protected:
	const QString& filename() const;
	const QString& selectedFilter() const;

private:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, QWidget* w);

	class Impl;
	Impl* impl;
};

#endif // HYDRAULICDATAIMPORTER_H
