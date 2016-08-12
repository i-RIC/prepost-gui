#ifndef MEASUREDDATA_IMPL_H
#define MEASUREDDATA_IMPL_H

#include "../measureddata.h"

#include <QString>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class MeasuredData::Impl
{
public:
	Impl();

	QString m_name;
	int m_index;

	bool m_noPolyData;

	vtkSmartPointer<vtkPolyData> m_pointData;
	vtkSmartPointer<vtkPolyData> m_polyData;

	std::vector<std::string> m_scalarNames;
	std::vector<std::string> m_vectorNames;
};

#endif // MEASUREDDATA_IMPL_H
