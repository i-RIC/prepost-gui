#include "geodatapointmaptemplatenodemapper.h"
#include "geodatapointmap.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <misc/mathsupport.h>

namespace {

const double VERYSMALL = 1.0E-12;

}

GeoDataPointmapTemplateNodeMapper::GeoDataPointmapTemplateNodeMapper(GeoDataCreator* parent) :
	GeoDataNodeMapperT<double, vtkDoubleArray>("Pointmap template node mapper", parent)
{
	init();
}

GeoDataMapperSettingI* GeoDataPointmapTemplateNodeMapper::initialize(bool* boolMap)
{
	GeoDataPointmapTemplateNodeMapperSetting* s = new GeoDataPointmapTemplateNodeMapperSetting();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto grid2d = dynamic_cast<Structured2DGrid*>(GeoDataMapper::grid());
	if (grid2d == nullptr) {return s;}
	if (container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.

	} else {
		vtkSmartPointer<vtkPointLocator> locator = vtkSmartPointer<vtkPointLocator>::New();
		vtkPointSet* pointSet = pointmap->points();
		locator->SetDataSet(pointSet);
		vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();

		unsigned int count = grid2d->nodeCount();
		for (unsigned int index = 0; index < count; ++index) {
			if (*(boolMap + index)) {continue;}
			unsigned int i, j;
			grid2d->getIJIndex(index, &i, &j);

			bool found = false;
			// get the grid node coordinates.
			QPointF vec2d = grid2d->vertex(i, j);
			double p[3];
			QPointF vecI;
			QPointF vecJ;
			QList<QPointF> ivecs;
			QList<QPointF> jvecs;

			p[0] = vec2d.x();
			p[1] = vec2d.y();
			p[2] = 0;

			if (i != 0) {
				ivecs.push_back(vec2d - grid2d->vertex(i - 1, j));
			}
			if (i != grid2d->dimensionI() - 1) {
				ivecs.push_back(grid2d->vertex(i + 1, j) - vec2d);
			}
			for (int k = 0; k < ivecs.count(); ++k) {
				vecI += ivecs[k];
			}
			vecI /= ivecs.count();

			if (j != 0) {
				jvecs.push_back(vec2d - grid2d->vertex(i, j - 1));
			}
			if (j != grid2d->dimensionJ() - 1) {
				jvecs.push_back(grid2d->vertex(i, j + 1) - vec2d);
			}
			for (int k = 0; k < jvecs.count(); ++k) {
				vecJ += jvecs[k];
			}
			vecJ /= jvecs.count();

			// vecI, vecJ is setup.
			double iDist = iRIC::length(vecI);
			if (! m_autoMode) {
				iDist = m_streamWiseLength;
			}
			double jDist = iRIC::length(vecJ);
			if (! m_autoMode) {
				jDist = m_crossStreamLegnth;
			}
			vecI = iRIC::normalize(vecI);
			vecJ = iRIC::normalize(vecJ);
			double radius = qMax(iDist, jDist);

			double rsum = 0;
			double searchRadius = 0;
			double searchIRadius = 0;
			double searchJRadius = 0;

			DoubleMappingSetting setting;
			setting.target = index;
			for (int radInc = 0; ! found && radInc <= m_numExpansions; ++radInc) {
				searchRadius += radius;
				searchIRadius += iDist;
				searchJRadius += jDist;
				locator->FindPointsWithinRadius(searchRadius, p, idlist);
				int numPoints = idlist->GetNumberOfIds();
				int pointsUsed = 0;
				for (int k = 0; k < numPoints; ++k) {
					double tmpPoint[3];
					auto idx = idlist->GetId(k);
					pointSet->GetPoint(idx, tmpPoint);
					QPointF tmpV(tmpPoint[0], tmpPoint[1]);
					QPointF dv = tmpV - vec2d;
					double tmpDistI = qAbs(QPointF::dotProduct(dv, vecI));
					double tmpDistJ = qAbs(QPointF::dotProduct(dv, vecJ));

					if (tmpDistI < searchIRadius && tmpDistJ < searchJRadius) {
						// it is near.
						if (tmpDistI < VERYSMALL && tmpDistJ < VERYSMALL) {
							// use this point for mapping.
							setting.indices.clear();
							setting.weights.clear();
							setting.indices.push_back(idx);
							setting.weights.push_back(1);
							s->settings.push_back(setting);
							found = true;
						} else {
							++ pointsUsed;
							double r = ::pow(iRIC::length(dv), m_weightPowVal);
							setting.indices.push_back(idx);
							setting.weights.push_back(1 / r);
							rsum += 1 / r;
						}
					}
				}
				if (pointsUsed > 0 && ! found) {
					// points found.
					for (int j = 0; j < setting.weights.size(); ++j) {
						setting.weights[j] /= rsum;
					}
					s->settings.push_back(setting);
					found = true;
				}
				if (found) {
					// mapping done.
					*(boolMap + index) = true;
				}
			}
		}
	}
	return s;
}

void GeoDataPointmapTemplateNodeMapper::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	GeoDataPointmapTemplateNodeMapperSetting* s2 =
		dynamic_cast<GeoDataPointmapTemplateNodeMapperSetting*>(s);
	vtkDoubleArray* da = container()->dataArray();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto vals = pointmap->pointsValues();
	for (int i = 0; i < s2->settings.size(); ++i) {
		const DoubleMappingSetting& setting = s2->settings.at(i);
		if (*(boolMap + setting.target) == false) {
			double value = 0;
			for (int j = 0; j < setting.indices.size(); ++j) {
				value += setting.weights.at(j) * vals->GetValue(setting.indices.at(j));
			}
			da->SetValue(static_cast<vtkIdType>(setting.target), value);
			*(boolMap + setting.target) = true;
		}
	}
	da->Modified();
}

void GeoDataPointmapTemplateNodeMapper::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

void GeoDataPointmapTemplateNodeMapper::setAutoMode(bool a)
{
	m_autoMode = a;
}

void GeoDataPointmapTemplateNodeMapper::setStreamWiseLength(double l)
{
	m_streamWiseLength = l;
}

void GeoDataPointmapTemplateNodeMapper::setCrossStreamLength(double l)
{
	m_crossStreamLegnth = l;
}

void GeoDataPointmapTemplateNodeMapper::setNumExpansions(int num)
{
	m_numExpansions = num;
}

void GeoDataPointmapTemplateNodeMapper::setWeightPowVal(double val)
{
	m_weightPowVal = val;
}

void GeoDataPointmapTemplateNodeMapper::init()
{
	m_autoMode = true;
	m_streamWiseLength = 4;
	m_crossStreamLegnth = 1;
	m_numExpansions = 3;
	m_weightPowVal = 1;
}
