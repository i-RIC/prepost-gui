#ifndef V4POSTZONEDATACONTAINER_H
#define V4POSTZONEDATACONTAINER_H

#include "../guicore_global.h"
#include "postdatacontainer.h"

namespace iRICLib {
	class H5CgnsZone;
} // namespace

class v4InputGrid;
class v4PostCalculatedResult;
class PreProcessorGridTypeDataItemI;
class SolverDefinitionGridType;
class v4SolutionGrid;

class GUICOREDLL_EXPORT v4PostZoneDataContainer : public PostDataContainer
{
public:
	v4PostZoneDataContainer(const std::string& zoneName, SolverDefinitionGridType* gridType, PostSolutionInfo* parent);
	~v4PostZoneDataContainer();

	const std::string& zoneName() const;
	QString caption() const;
	SolverDefinitionGridType* gridType() const;

	v4InputGrid* inputGridData() const;

	v4SolutionGrid* gridData() const;

	v4SolutionGrid* particleData() const;

	v4SolutionGrid* particleGroup(const std::string& name) const;
	const std::map<std::string, v4SolutionGrid*>& particleGroupMap() const;

	v4SolutionGrid* particleGroupImage(const std::string& name) const;
	const std::map<std::string, v4SolutionGrid*>& particleGroupImageMap() const;

	v4SolutionGrid* polyData(const std::string& name) const;
	const std::map<std::string, v4SolutionGrid*>& polyDataMap() const;

	int loadFromCgnsFile(iRICLib::H5CgnsZone* zone, PreProcessorGridTypeDataItemI* gtItem, const QString tmpPath, bool disableCalculatedResult);
	int loadIfEmpty(iRICLib::H5CgnsZone* zone, PreProcessorGridTypeDataItemI* gtItem, const QString tmpPath);

	void applyOffset(const QPointF& offset);

	std::vector<v4PostCalculatedResult*>& calculatedResults();
	const std::vector<v4PostCalculatedResult*>& calculatedResults() const;
	std::vector<v4PostCalculatedResult*> detachCalculatedResult();
	void attachCalculatedResult(const std::vector<v4PostCalculatedResult*>& results);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // V4POSTZONEDATACONTAINER_H
