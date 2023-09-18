#include "geodatapointmap_impl.h"
#include "geodatapointmap_mappingsetting.h"
#include "geodatapointmap_templatenodemapper.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/doublemappingsetting.h>
#include <misc/mathsupport.h>

namespace {

const double VERYSMALL = 1.0E-12;

class Setting : public GeoDataMapperSettingI
{
public:
	std::vector<DoubleMappingSetting> settings;
};

}

GeoDataPointmap::TemplateNodeMapper::TemplateNodeMapper(QObject* parent) :
	GeoDataNodeMapperT<double, vtkDoubleArray> ("Pointmap template node mapper", parent)
{}

GeoDataMapperSettingI* GeoDataPointmap::TemplateNodeMapper::initialize(bool* boolMap)
{
	auto s = new ::Setting();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto grid2d = dynamic_cast<v4Structured2dGrid*>(GeoDataMapper::grid()->grid());
	if (grid2d == nullptr) {return s;}
	if (container()->gridAttribute()->isOption()) {
		// @todo not implemented yet.

	} else {
		vtkSmartPointer<vtkPointLocator> locator = vtkSmartPointer<vtkPointLocator>::New();
		vtkPointSet* pointSet = pointmap->points();
		locator->SetDataSet(pointSet);
		vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();

		auto ms = pointmap->impl->m_mappingSetting;

		unsigned int count = grid2d->nodeCount();
		for (unsigned int index = 0; index < count; ++index) {
			if (*(boolMap + index)) {continue;}
			vtkIdType i, j;
			grid2d->getPointIJIndex(index, &i, &j);

			bool found = false;
			// get the grid node coordinates.
			QPointF vec2d = grid2d->point2d(i, j);

			int iCount = 0;
			int jCount = 0;
			QPointF iVec(0, 0);
			QPointF jVec(0, 0);

			if (i != 0) {
				iVec += vec2d - grid2d->point2d(i - 1, j);
				++ iCount;
			}
			if (i != grid2d->dimensionI() - 1) {
				iVec += grid2d->point2d(i + 1, j) - vec2d;
				++ iCount;
			}
			iVec /= iCount;

			if (j != 0) {
				jVec += vec2d - grid2d->point2d(i, j - 1);
				++ jCount;
			}
			if (j != grid2d->dimensionJ() - 1) {
				jVec += grid2d->point2d(i, j + 1) - vec2d;
				++ jCount;
			}
			jVec /= jCount;

			double iDist = iRIC::length(iVec);
			double jDist = iRIC::length(jVec);

			if (! ms.templateAutoMode) {
				iDist = ms.templateStreamWiseLength;
			}
			if (! ms.templateAutoMode) {
				jDist = ms.templateCrossStreamLength;
			}
			iVec = iRIC::normalize(iVec);
			jVec = iRIC::normalize(jVec);
			double radius = qMax(iDist, jDist);

			double rsum = 0;
			double searchRadius = 0;
			double searchIRadius = 0;
			double searchJRadius = 0;

			DoubleMappingSetting setting;
			setting.target = index;
			double p[3];
			p[0] = vec2d.x();
			p[1] = vec2d.y();
			p[2] = 0;

			for (int radInc = 0; ! found && radInc <= ms.templateNumberOfExpansions; ++radInc) {
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
					double tmpDistI = qAbs(QPointF::dotProduct(dv, iVec));
					double tmpDistJ = qAbs(QPointF::dotProduct(dv, jVec));

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
							double r = ::pow(iRIC::length(dv), ms.templateWeightExponent);
							setting.indices.push_back(idx);
							setting.weights.push_back(1 / r);
							rsum += 1 / r;
						}
					}
				}
				if (pointsUsed > 0 && ! found) {
					// points found.
					for (unsigned int j = 0; j < setting.weights.size(); ++j) {
						setting.weights[j] /= rsum;
					}
					s->settings.push_back(setting);
					found = true;
				}
				if (found) {
					// mapping done.
					*(boolMap + index) = true;
					break;
				}
			}
		}
	}
	return s;
}

void GeoDataPointmap::TemplateNodeMapper::map(bool* boolMap, GeoDataMapperSettingI* s)
{
	auto s2 = dynamic_cast<::Setting*>(s);
	vtkDoubleArray* da = container()->dataArray();
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto vals = pointmap->pointsValues();
	for (unsigned int i = 0; i < s2->settings.size(); ++i) {
		const DoubleMappingSetting& setting = s2->settings.at(i);
		if (*(boolMap + setting.target) == false) {
			double value = 0;
			for (unsigned int j = 0; j < setting.indices.size(); ++j) {
				value += setting.weights.at(j) * vals->GetValue(setting.indices.at(j));
			}
			da->SetValue(static_cast<vtkIdType>(setting.target), value);
			*(boolMap + setting.target) = true;
		}
	}
	da->Modified();
}

void GeoDataPointmap::TemplateNodeMapper::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

