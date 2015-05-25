#ifndef RAWDATAIMPORTER_H
#define RAWDATAIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

class RawData;
class RawDataCreator;
class SolverDefinitionGridRelatedCondition;
class PreProcessorRawDataGroupDataItemInterface;

class GUICOREDLL_EXPORT RawDataImporter : public QObject
{
public:
	/// Constructor
	RawDataImporter(RawDataCreator* creator);
	/// Initializes this class to prepare for importing the rawdata from the specified file.
	bool importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w);
	/// Import the rawdata into data.
	virtual bool importData(RawData* data, int index, QWidget* w) = 0;
	virtual const QStringList fileDialogFilters() = 0;
	virtual const QStringList acceptableExtensions() = 0;
	RawDataCreator* creator();
	const QString& caption() const {return m_caption;}

protected:
	virtual bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w);
	QString m_caption;
	QString m_filename;
	QString m_selectedFilter;
};

#endif // RAWDATAIMPORTER_H
