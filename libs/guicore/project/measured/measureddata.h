#ifndef MEASUREDDATA_H
#define MEASUREDDATA_H

#include "../../guicore_global.h"
#include "../projectdataitem.h"

#include <string>
#include <vector>

class QString;

class vtkPolyData;

/// This class stores the information about the specified workfolder.
class GUICOREDLL_EXPORT MeasuredData : public ProjectDataItem
{
	Q_OBJECT

public:
	MeasuredData(ProjectDataItem* parent);
	~MeasuredData();

	const QString& name() const;
	void setName(const QString& name);

	vtkPolyData* pointData() const;
	vtkPolyData* polyData() const;

	const std::vector<std::string>& scalarNames() const;
	std::vector<std::string>& scalarNames();

	const std::vector<std::string>& vectorNames() const;
	std::vector<std::string>& vectorNames();

	int index() const;
	void setIndex(int index);

	void setupPolyData();
	bool noPolyData() const;
	void applyOffset(double x, double y);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	void doApplyOffset(double x, double y) override;

	class Impl;
	Impl* impl;
};

#endif // MEASUREDDATA_H
