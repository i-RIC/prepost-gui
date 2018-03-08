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

class SolverDefinitionGridType;
class ScalarsToColorsContainer;
class ScalarsToColorsEditDialog;

class GUICOREDLL_EXPORT PostZonePointSeriesDataContainer : public PostSeriesDataContainer
{

public:
	PostZonePointSeriesDataContainer(PostSolutionInfo::Dimension dim, const std::string& zoneName, const QString& pName, int pointIndex, GridLocation_t gridLocation, ProjectDataItem* parent);
	const QList<double>& data() const {return m_data;}
	bool handleCurrentStepUpdate(const int /*fn*/) override {
		// do nothing.
		return true;
	}
	void update(const int fn) {
		loadFromCgnsFile(fn);
	}
	const std::string& zoneName() const {return m_zoneName;}
	/// Caption is the region name in pre-processor.
	/// Currently, zone name is used instead, temporally.
	QString caption() const {return zoneName().c_str();}
	void setPointIndex(int index) {m_pointIndex = index;}
	int pointIndex() const {return m_pointIndex;}
	void setGridLocation(GridLocation_t location) { m_gridLocation = location; }
	GridLocation_t gridLocation() const { return m_gridLocation; }

protected:
	bool loadData(const int fn) override;
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}

	bool loadData(const int fn, GridLocation_t location);

private:
	bool setZoneId(const int fn);

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
