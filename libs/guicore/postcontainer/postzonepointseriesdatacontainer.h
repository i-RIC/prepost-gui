#ifndef POSTZONEPOINTSERIESDATACONTAINER_H
#define POSTZONEPOINTSERIESDATACONTAINER_H

#include "../guicore_global.h"
#include "postsolutioninfo.h"
#include "postseriesdatacontainer.h"
#include <QString>
#include <QList>
#include <vtkPointSet.h>
#include <vtkSmartPointer.h>
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
	PostZonePointSeriesDataContainer(PostSolutionInfo::Dimension dim, const QString& zoneName, const QString& pName, int pointIndex, ProjectDataItem* parent);
	const QList<double>& data() const {return m_data;}
	bool handleCurrentStepUpdate(const int /*fn*/){
		// do nothing.
		return true;
	}
	void update(const int fn){
		loadFromCgnsFile(fn);
	}
	const QString zoneName() const {return m_zoneName;}
	/// Caption is the region name in pre-processor.
	/// Currently, zone name is used instead, temporally.
	const QString caption() const {return zoneName();}
	void setPointIndex(int index){m_pointIndex = index;}
	int pointIndex() const {return m_pointIndex;}
protected:
	bool loadData(const int fn);
	void doLoadFromProjectMainFile(const QDomNode& /*node*/){}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/){}
private:
	bool setZoneId(const int fn);
protected:
	QList<double> m_data;
	QString m_zoneName;
	int m_zoneId;
	QString m_physName;
	int m_pointIndex;
	cgsize_t m_sizes[9];
};

#endif // POSTZONEPOINTSERIESDATACONTAINER_H
