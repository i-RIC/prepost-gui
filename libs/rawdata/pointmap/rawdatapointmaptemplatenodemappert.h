#ifndef RAWDATAPOINTMAPTEMPLATENODEMAPPERT_H
#define RAWDATAPOINTMAPTEMPLATENODEMAPPERT_H

#include <guicore/pre/rawdata/rawdatanodemappert.h>
// #include "rawdatapointmapt.h"
#include <guicore/solverdef/enumloader.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <misc/doublemappingsetting.h>

#include <vtkCell.h>
#include <vtkSmartPointer.h>
#include <vtkPointLocator.h>

#include <cmath>

#define VERYSMALL 1.0E-12

class RawDataPointmapTemplateNodeMapperSetting : public RawDataMapperSetting
{
public:
	RawDataPointmapTemplateNodeMapperSetting() : RawDataMapperSetting() {}
	~RawDataPointmapTemplateNodeMapperSetting(){}
	QList<DoubleMappingSetting> settings;
};

template <class V, class DA>
class RawDataPointmapTemplateNodeMapperT : public RawDataNodeMapperT<V, DA>
{
public:
	RawDataPointmapTemplateNodeMapperT(RawDataCreator* parent)
		: RawDataNodeMapperT<V, DA>(parent)
	{
		RawDataNodeMapperT<V, DA>::m_caption = "Pointmap template node mapper";
		init();
	}

	RawDataMapperSetting* initialize(bool* boolMap);

	void map(bool* boolMap, RawDataMapperSetting* s);

	void terminate(RawDataMapperSetting* s)
	{
		delete s;
	}

	void setAutoMode(bool a){m_autoMode = a;}
	void setStreamWiseLength(double l){m_streamWiseLength = l;}
	void setCrossStreamLength(double l){m_crossStreamLegnth = l;}
	void setNumExpansions(int num){m_numExpansions = num;}
	void setWeightPowVal(double val){m_weightPowVal = val;}

private:
	void init(){
		m_autoMode = true;
		m_streamWiseLength = 4;
		m_crossStreamLegnth = 1;
		m_numExpansions = 3;
		m_weightPowVal = 1;
	}
	bool m_autoMode;
	double m_streamWiseLength;
	double m_crossStreamLegnth;
	int m_numExpansions;
	double m_weightPowVal;

};

#include "rawdatapointmapt.h"

template <class V, class DA>
RawDataMapperSetting* RawDataPointmapTemplateNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	RawDataPointmapTemplateNodeMapperSetting* s = new RawDataPointmapTemplateNodeMapperSetting();
	RawDataPointMapT<V, DA>* pointmap = dynamic_cast<RawDataPointMapT<V, DA>* >(RawDataMapper::m_rawdata);
	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(RawDataMapper::m_grid);
	if (grid2d == nullptr){return s;}
	if (RawDataNodeMapperT<V, DA>::container()->condition()->isOption()){
		// @todo not implemented yet.

	} else {
		vtkSmartPointer<vtkPointLocator> locator = vtkSmartPointer<vtkPointLocator>::New();
		vtkPointSet* pointSet = pointmap->vtkGrid();
		locator->SetDataSet(pointSet);
		vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();

		unsigned int count = grid2d->nodeCount();
		for (unsigned int index = 0; index < count; ++index){
			if (*(boolMap + index)){continue;}
			unsigned int i, j;
			grid2d->getIJIndex(index, &i, &j);

			bool found = false;
			// get the grid node coordinates.
			QVector2D vec2d = grid2d->vertex(i, j);
			double p[3];
			QVector2D vecI;
			QVector2D vecJ;
			QList<QVector2D> ivecs;
			QList<QVector2D> jvecs;

			p[0] = vec2d.x();
			p[1] = vec2d.y();
			p[2] = 0;

			if (i != 0){
				ivecs.push_back(vec2d - grid2d->vertex(i - 1, j));
			}
			if (i != grid2d->dimensionI() - 1){
				ivecs.push_back(grid2d->vertex(i + 1, j) - vec2d);
			}
			for (int k = 0; k < ivecs.count(); ++k){
				vecI += ivecs[k];
			}
			vecI /= ivecs.count();

			if (j != 0){
				jvecs.push_back(vec2d - grid2d->vertex(i, j - 1));
			}
			if (j != grid2d->dimensionJ() - 1){
				jvecs.push_back(grid2d->vertex(i, j + 1) - vec2d);
			}
			for (int k = 0; k < jvecs.count(); ++k){
				vecJ += jvecs[k];
			}
			vecJ /= jvecs.count();

			// vecI, vecJ is setup.
			double iDist = vecI.length();
			if (! m_autoMode){
				iDist = m_streamWiseLength;
			}
			double jDist = vecJ.length();
			if (! m_autoMode){
				jDist = m_crossStreamLegnth;
			}
			vecI.normalize();
			vecJ.normalize();
			double radius = qMax(iDist, jDist);

			double rsum = 0;
			double searchRadius = 0;
			double searchIRadius = 0;
			double searchJRadius = 0;

			DoubleMappingSetting setting;
			setting.target = index;
			for (int radInc = 0; ! found && radInc <= m_numExpansions; ++radInc){
				searchRadius += radius;
				searchIRadius += iDist;
				searchJRadius += jDist;
				locator->FindPointsWithinRadius(searchRadius, p, idlist);
				int numPoints = idlist->GetNumberOfIds();
				int pointsUsed = 0;
				for (int k = 0; k < numPoints; ++k){
					double tmpPoint[3];
					pointSet->GetPoint(idlist->GetId(k), tmpPoint);
					QVector2D tmpV(tmpPoint[0], tmpPoint[1]);
					QVector2D dv = tmpV - vec2d;
					double tmpDistI = qAbs(QVector2D::dotProduct(dv, vecI));
					double tmpDistJ = qAbs(QVector2D::dotProduct(dv, vecJ));

					if (tmpDistI < searchIRadius && tmpDistJ < searchJRadius){
						// it is near.
						if (tmpDistI < VERYSMALL && tmpDistJ < VERYSMALL){
							// use this point for mapping.
							setting.indices.clear();
							setting.weights.clear();
							setting.indices.append(k);
							setting.weights.append(1);
							s->settings.append(setting);
							found = true;
						} else {
							++ pointsUsed;
							double r = ::pow(dv.length(), m_weightPowVal);
							setting.indices.append(k);
							setting.weights.append(1 / r);
							rsum += 1 / r;
						}
					}
				}
				if (pointsUsed > 0 && ! found){
					// points found.
					for (int j = 0; j < setting.weights.size(); ++j){
						setting.weights[j] /= rsum;
					}
					s->settings.append(setting);
					found = true;
				}
				if (found){
					// mapping done.
					*(boolMap + index) = true;
				}
			}
		}
	}
	return s;
}

template <class V, class DA>
void RawDataPointmapTemplateNodeMapperT<V, DA>::map(bool* boolMap, RawDataMapperSetting* s)
{
	RawDataPointmapTemplateNodeMapperSetting* s2 =
			dynamic_cast<RawDataPointmapTemplateNodeMapperSetting*> (s);
	DA* da = RawDataNodeMapperT<V, DA>::container()->dataArray();
	RawDataPointMapT<V, DA>* pointmap = dynamic_cast<RawDataPointMapT<V, DA>* >(RawDataMapper::m_rawdata);
	DA* vals = pointmap->vtkValues();
	for (int i = 0; i < s2->settings.size(); ++i){
		const DoubleMappingSetting& setting = s2->settings.at(i);
		if (*(boolMap + setting.target) == false){
			double value = 0;
			for (int j = 0; j < setting.indices.size(); ++j){
				value += setting.weights.at(j) * vals->GetValue(setting.indices.at(j));
			}
			da->SetValue(static_cast<vtkIdType>(setting.target), value);
			*(boolMap + setting.target) = true;
		}
	}
	da->Modified();
}

#endif // RAWDATAPOINTMAPTEMPLATENODEMAPPERT_H
