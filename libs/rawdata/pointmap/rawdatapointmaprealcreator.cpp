#include "rawdatapointmapcellmappert.h"
#include "rawdatapointmapnodemappert.h"
#include "rawdatapointmaprealcreator.h"
#include "rawdatapointmaprealexporter.h"
#include "rawdatapointmaprealimporter.h"
#include "rawdatapointmapstlexporter.h"
#include "rawdatapointmapstlimporter.h"
#include "rawdatapointmapt.h"
#include "rawdatapointmapvtkexporter.h"

#include <vtkDoubleArray.h>

RawDataPointmapRealCreator::RawDataPointmapRealCreator()
	: RawDataCreator()
{
	m_caption = tr("Points");
	m_typeName = "realPointmap";

	m_importers.append(new RawDataPointmapRealImporter(this));
	m_importers.append(new RawDataPointmapSTLImporter(this));

	m_exporters.append(new RawDataPointmapRealExporter(this));
	m_exporters.append(new RawDataPointmapSTLExporter(this));
	m_exporters.append(new RawDataPointmapVTKExporter(this));

	m_nodeMappers.append(new RawDataPointmapNodeMapperT<double, vtkDoubleArray>(this));
	m_cellMappers.append(new RawDataPointmapCellMapperT<double, vtkDoubleArray>(this));
}

RawData* RawDataPointmapRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	RawDataPointmap* data = new RawDataPointMapT<double, vtkDoubleArray>(parent, this, condition);
	data->setDefaultMapper();
	return data;
}

QString RawDataPointmapRealCreator::name(unsigned int index)
{
	return QString("pointset%1").arg(index);
}

QString RawDataPointmapRealCreator::defaultCaption(unsigned int index)
{
	return QString(tr("Points%1")).arg(index);
}

bool RawDataPointmapRealCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition)
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}
	return true;
}
