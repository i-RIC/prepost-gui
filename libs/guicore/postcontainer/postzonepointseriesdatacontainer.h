#ifndef POSTZONEPOINTSERIESDATACONTAINER_H
#define POSTZONEPOINTSERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postsolutioninfo.h"
#include "postseriesdatacontainer.h"
#include <QString>
#include <QList>

#include <h5cgnszone.h>

class PostCalculatedResult;
class SolverDefinitionGridType;
class ScalarsToColorsContainer;
class ScalarsToColorsEditDialog;

class GUICOREDLL_EXPORT PostZonePointSeriesDataContainer : public PostSeriesDataContainer
{
public:
	PostZonePointSeriesDataContainer(PostSolutionInfo::Dimension dim, const std::string& zoneName, const std::string valueName, int index, iRICLib::H5CgnsZone::SolutionPosition position, PostSolutionInfo* sinfo);

	const std::string& zoneName() const;
	QString caption() const;	/// Caption is the region name in pre-processor.
	int pointIndex() const;

private:
	int loadData() override;
	int loadData(const std::string& name, iRICLib::H5CgnsZone* zone, double* value);

	int loadCalculatedData(PostCalculatedResult* result, iRICLib::H5CgnsZone* zone, double* value);
	int loadResultData(const std::string& name, iRICLib::H5CgnsZone* zone, double* value);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	PostZoneDataContainer* zoneDataContainer() const;

protected:
	std::string m_zoneName;
	std::string m_valueName;

	int m_pointIndex;
	iRICLib::H5CgnsZone::SolutionPosition m_position;
};

#endif // POSTZONEPOINTSERIESDATACONTAINER_H
