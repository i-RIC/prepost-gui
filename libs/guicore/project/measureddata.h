#ifndef MEASUREDDATA_H
#define MEASUREDDATA_H

#include "../guicore_global.h"
#include "projectdataitem.h"

#include <QObject>
#include <QString>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <string>
#include <vector>

/// This class stores the information about the specified workfolder.
class GUICOREDLL_EXPORT MeasuredData : public ProjectDataItem
{
	Q_OBJECT

public:
	MeasuredData(ProjectDataItem* parent);

	const QString& name() const;

	vtkPolyData* pointData() const;
	vtkPolyData* polyData() const;

	const std::vector<std::string>& scalarNames() const;
	const std::vector<std::string>& vectorNames() const;

	int index() const;
	void setIndex(int index);

	void setupPolyData();
	bool noPolyData() const;
	void applyOffset(double x, double y);

	void importFromFile(const QString& filename);
	void exportToFile(const QString& filename);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;

	void doApplyOffset(double x, double y) override;

	QString m_name;
	int m_index;

	bool m_noPolyData;

	vtkSmartPointer<vtkPolyData> m_pointData;
	vtkSmartPointer<vtkPolyData> m_polyData;

	std::vector<std::string> m_scalarNames;
	std::vector<std::string> m_vectorNames;
};

#endif // MEASUREDDATA_H
