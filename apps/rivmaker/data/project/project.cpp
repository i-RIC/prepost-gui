#include "project.h"
#include "../arbitraryhwm/arbitraryhwm.h"
#include "../crosssection/crosssection.h"
#include "../leftbankhwm/leftbankhwm.h"
#include "../rightbankhwm/rightbankhwm.h"
#include "../../geom/geometrypoint.h"
#include "../../misc/mathutil.h"

#include "private/project_impl.h"

#include <map>
#include <vector>

namespace {

void addToVector(const Points& points, const BaseLine& line, std::multimap<double, double>* vals)
{
	for (GeometryPoint* p : points.points()) {
		double pos = line.calcPosition(p->x(), p->y());
		vals->insert(std::make_pair(pos, p->z()));
	}
}

void addToElevMap(std::map<CrossSection*, std::vector<double> >* elevVals, CrossSection* cs, double val)
{
	auto it = elevVals->find(cs);
	if (it == elevVals->end()) {
		std::vector<double> emptyVec;
		elevVals->insert(std::make_pair(cs, emptyVec));
		it = elevVals->find(cs);
	}
	it->second.push_back(val);
}

} // namespace

Project::Impl::Impl(Project *project) :
	m_rootDataItem {project},
	m_elevationPoints {&m_rootDataItem},
	m_waterSurfaceElevationPoints {&m_rootDataItem},
	m_crossSections {&m_rootDataItem},
	m_baseLine {&m_rootDataItem},
	m_currentBuilder {& m_builderNearest}
{}

Project::Impl::~Impl()
{}

// public interfraces

Project::Project() :
	impl {new Impl {this}}
{}

Project::~Project()
{
	delete impl;
}

RootDataItem* Project::rootDataItem() const
{
	return &(impl->m_rootDataItem);
}

const ElevationPoints& Project::elevationPoints() const
{
	return impl->m_elevationPoints;
}

ElevationPoints& Project::elevationPoints()
{
	return impl->m_elevationPoints;
}

const WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints() const
{
	return impl->m_waterSurfaceElevationPoints;
}

WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints()
{
	return impl->m_waterSurfaceElevationPoints;
}

const BaseLine& Project::baseLine() const
{
	return impl->m_baseLine;
}

BaseLine& Project::baseLine()
{
	return impl->m_baseLine;
}

const CrossSections& Project::crossSections() const
{
	return impl->m_crossSections;
}

CrossSections& Project::crossSections()
{
	return impl->m_crossSections;
}

const QPointF& Project::offset() const
{
	return impl->m_offset;
}

void Project::setOffset(const QPointF& offset)
{
	impl->m_offset = offset;
}

Project::MappingMethod Project::mappingMethod() const
{
	if (impl->m_currentBuilder == &(impl->m_builderNearest)) {
		return MappingMethod::AllMapToNearestCrossSection;
	} else if (impl->m_currentBuilder == &(impl->m_builderTemplate)) {
		return MappingMethod::TIN;
	} else {
		return MappingMethod::Template;
	}
}

void Project::setMappingMethod(MappingMethod method)
{
	if (method == MappingMethod::AllMapToNearestCrossSection) {
		impl->m_currentBuilder = &(impl->m_builderNearest);
	} else if (method == MappingMethod::TIN) {
		impl->m_currentBuilder = &(impl->m_builderTin);
	} else {
		impl->m_currentBuilder = &(impl->m_builderTemplate);
	}
}

double Project::templateMappingResolution() const
{
	return impl->m_builderTemplate.mappingResolution();
}

void Project::setTemplateMappingResolution(double resolution)
{
	impl->m_builderTemplate.setMappingResolution(resolution);
}

double Project::templateMappingStreamWiseLength() const
{
	return impl->m_builderTemplate.streamWiseLength();
}

void Project::setTemplateMappingStreamWiseLength(double len)
{
	impl->m_builderTemplate.setStreamWiseLength(len);
}

double Project::templateMappingCrossStreamWidth() const
{
	return impl->m_builderTemplate.crossStreamWidth();
}

void Project::setTemplateMappingCrossStreamWidth(double width)
{
	impl->m_builderTemplate.setCrossStreamWidth(width);
}

int Project::templateMappingNumberOfExpansions() const
{
	return impl->m_builderTemplate.numberOfExpansions();
}

void Project::setTemplateMappingNumberOfExpansions(int num)
{
	impl->m_builderTemplate.setNumberOfExpansions(num);
}

double Project::templateMappingWeightExponent() const
{
	return impl->m_builderTemplate.weightExponent();
}

void Project::setTemplateMappingWeightExponent(double exp)
{
	impl->m_builderTemplate.setWeightExponent(exp);
}

void Project::calcCrossSectionElevations()
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	if (bl.polyLine().size() < 2) {return;}

	std::multimap<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (crosses) {
			posMap.insert(std::make_pair(pos, cs));
		}
	}

	std::multimap<double, double> vals;

	auto& wse = waterSurfaceElevationPoints();

	addToVector(wse.leftBankHWM(), bl, &vals);
	addToVector(wse.rightBankHWM(), bl, &vals);
	addToVector(wse.arbitraryHWM(), bl, &vals);

	std::map<CrossSection*, std::vector<double> > elevVals;

	for (auto it = posMap.begin(); it != posMap.end(); ++it) {
		auto it2 = it;
		++ it2;
		if (it2 == posMap.end()) {break;}

		double min = it->first;
		double max = it2->first;

		std::vector<double> xvec, yvec;

		auto it_b = vals.lower_bound(min);
		auto it_e = vals.upper_bound(max);
		for (auto tmp_it = it_b; tmp_it != it_e; ++ tmp_it) {
			xvec.push_back(tmp_it->first);
			yvec.push_back(tmp_it->second);
		}
		double a, b;
		MathUtil::leastSquares(xvec, yvec, &a, &b);

		addToElevMap(&elevVals, it->second,  a * min + b);
		addToElevMap(&elevVals, it2->second, a * max + b);
	}

	for (auto pair : elevVals) {
		if (pair.second.size() == 0) {continue;}

		double sum = 0;
		for (double v : pair.second) {
			sum += v;
		}
		double avg = sum / pair.second.size();
		pair.first->setWaterElevation(avg);
	}
}

void Project::mapPointsToCrossSections()
{
	if (crossSections().crossSectionVector().size() == 0) {return;}

	const auto& csVec = crossSections().crossSectionVector();
	for (auto cs : csVec) {
		cs->clearMappedPoints();
	}
	impl->m_currentBuilder->build(elevationPoints(), &(crossSections()));
}

bool Project::sortCrossSectionsIfPossible()
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	if (bl.polyLine().size() < 2) {return false;}

	std::multimap<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {return false;}

		posMap.insert(std::make_pair(pos, cs));
	}

	auto& childItems = crossSections().childItems();
	childItems.clear();
	for (auto pair : posMap) {
		childItems.push_back(pair.second);
	}
	return true;
}

void Project::emitUpdated()
{
	emit updated();
}
