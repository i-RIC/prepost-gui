#ifndef POSTZONEPOINTSERIESDATACONTAINER_H
#define POSTZONEPOINTSERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postsolutioninfo.h"
#include "postseriesdatacontainer.h"
#include <QString>
#include <QList>
#include <cgnslib.h>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

class PostCalculatedResult;
class SolverDefinitionGridType;
class ScalarsToColorsContainer;
class ScalarsToColorsEditDialog;

class GUICOREDLL_EXPORT PostZonePointSeriesDataContainer : public PostSeriesDataContainer
{

public:
	PostZonePointSeriesDataContainer(PostSolutionInfo::Dimension dim, const std::string& zoneName, const QString& pName, int pointIndex, GridLocation_t gridLocation, PostSolutionInfo* sinfo);
	const QList<double>& data() const;

	bool handleCurrentStepUpdate(const int fn) override;
	void update(const int fn);
	const std::string& zoneName() const;
	/// Caption is the region name in pre-processor.
	/// Currently, zone name is used instead, temporally.
	QString caption() const;
	void setPointIndex(int index);
	int pointIndex() const;
	void setGridLocation(GridLocation_t location);
	GridLocation_t gridLocation() const;

protected:
	bool loadData(const int fn) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	bool loadData(const int fn, GridLocation_t location);

private:
	bool setZoneId(const int fn);
	bool loadData(int fn, int solId, const QString& name, double* value);
	bool loadCalculatedData(int fn, int solId, PostCalculatedResult* result, double* value);
	bool loadResultData(int fn, int solId, const QString& name, double* value);
	PostZoneDataContainer* zoneDataContainer() const;

protected:
	QList<double> m_data;
	std::string m_zoneName;
	int m_zoneId;
	QString m_physName;
	int m_pointIndex;
	GridLocation_t m_gridLocation;
	cgsize_t m_sizes[9];
};

#endif // POSTZONEPOINTSERIESDATACONTAINER_H
