#include "pointring.h"
#include "rect.h"

#include <vtkCell.h>
#include <vtkPoints.h>

#include <algorithm>
#include <cmath>

PointRing::PointRing(const std::vector<vtkIdType>& points, vtkPoints* vtkPs) :
	m_points (points),
	m_vtkPoints {vtkPs}
{
	updateSortedPoints();
}

PointRing::PointRing(vtkCell* cell, vtkPoints* vtkPs) :
	m_vtkPoints {vtkPs}
{
	vtkIdType numPoints = cell->GetNumberOfPoints();
	m_points.reserve(numPoints);
	for (vtkIdType i = 0; i < numPoints; ++i) {
		m_points.push_back(cell->GetPointId(i));
	}
	updateSortedPoints();
}

Rect PointRing::boundingRect() const
{
	double xmin, xmax, ymin, ymax;
	double v[3];
	for (int i = 0; i < m_points.size(); ++i) {
		m_vtkPoints->GetPoint(i, v);
		if (i == 0 || v[0] < xmin) {xmin = v[0];}
		if (i == 0 || v[0] > xmax) {xmax = v[0];}
		if (i == 0 || v[1] < ymin) {ymin = v[1];}
		if (i == 0 || v[1] > ymax) {ymax = v[1];}
	}
	return Rect(xmin, xmax, ymin, ymax);
}

const std::vector<vtkIdType>& PointRing::points() const
{
	return m_points;
}

vtkPoints* PointRing::getVtkPoints() const
{
	return m_vtkPoints;
}

bool PointRing::merge(PointRing* otherRing, PointRing** another, int rotateCount)
{
	int otherSize = otherRing->points().size();
	for (int i = 0; i < otherSize; ++i) {
		vtkIdType p1 = otherRing->points().at(i);
		if (! contains(p1)) {continue;}
		vtkIdType p2 = otherRing->points().at((i + 1) % otherSize);
		if (! contains(p2)) {continue;}
		auto p1Id = std::find(m_points.begin(), m_points.end(), p1) - m_points.begin();
		auto prevId = (p1Id - 1 + m_points.size()) % m_points.size();
		auto nextId = (p1Id + 1) % m_points.size();
		if (m_points[prevId] != p2 && m_points[nextId] != p2) {continue;}

		if (i == 0 && rotateCount < otherRing->m_points.size()) {
			int myFirst = otherRing->m_points.front();
			otherRing->m_points.erase(otherRing->m_points.begin());
			otherRing->m_points.push_back(myFirst);
			return merge(otherRing, another, rotateCount + 1);
		}

		reOrder(p1, p2);
		otherRing->reOrder(p1, p2);
		// now, p1 is first, p2 is second in both this and otherRing
		int last = 1;
		while (
			last < m_points.size() - 1 &&
			last < otherSize - 1 &&
			m_points.at(last + 1) == otherRing->points().at(last + 1))
		{
			++ last;
		}
		bool aFound = false;
		int aMyFirst, aMyLast, aOtherFirst, aOtherLast;
		// the first point of another loop on the otherRing
		aOtherFirst = last + 1;
		for (; aOtherFirst < otherSize; ++aOtherFirst) {
			vtkIdType ap1 = otherRing->points().at(aOtherFirst);
			if (! contains(ap1)) {continue;}
			// the first point of another loop on this ring
			aMyFirst = std::find(m_points.begin(), m_points.end(), ap1) - m_points.begin();
			aOtherLast = aOtherFirst;
			aMyLast = aMyFirst;
			while (
				aMyLast < m_points.size() - 1 &&
				aOtherLast < otherSize - 1 &&
				m_points.at(aMyLast + 1) == otherRing->points().at(aOtherLast + 1))
			{
				++ aMyLast;
				++ aOtherLast;
			}
			if (aMyFirst == aMyLast) {
				// no loop found.
				continue;
			}
			aFound = true;
			break;
		}
		if (aFound == false) {
			// another loop is not found. simply connect.
			std::vector<vtkIdType> newPoints;
			if (last == m_points.size() - 1 && last == otherRing->points().size() - 1) {
				// empty ring.
				m_points = newPoints;
				updateSortedPoints();
				return true;
			}
			newPoints.reserve(m_points.size() + otherRing->points().size());
			for (int id = last; id < m_points.size(); ++id) {
				newPoints.push_back(m_points.at(id));
			}
			newPoints.push_back(m_points.at(0));
			for (int id = otherRing->points().size() - 1; id > last; --id) {
				newPoints.push_back(otherRing->points().at(id));
			}
			m_points = newPoints;
			updateSortedPoints();
			return true;
		} else {
			std::vector<vtkIdType> newPoints;
			for (int id = aMyLast; id < m_points.size(); ++id) {
				newPoints.push_back(m_points.at(id));
			}
			newPoints.push_back(m_points.at(0));
			for (int id = otherRing->points().size() - 1; id > aOtherLast; --id) {
				newPoints.push_back(otherRing->points().at(id));
			}

			std::vector<vtkIdType> anotherPoints;
			for (int id = last; id <= aMyFirst; ++id) {
				anotherPoints.push_back(m_points.at(id));
			}
			for (int id = aOtherFirst - 1; id > last; --id) {
				anotherPoints.push_back(otherRing->points().at(id));
			}

			m_points = newPoints;
			updateSortedPoints();

			*another = new PointRing(anotherPoints, m_vtkPoints);
			return true;
		}
	}
	return false;
}

void PointRing::reOrder(vtkIdType first, vtkIdType second)
{
	auto size = m_points.size();
	auto it = std::find(m_points.begin(), m_points.end(), first);
	int index = it - m_points.begin();

	int inc = -1; // increment. -1 for reverse.
	if (m_points[(index + 1) % size] == second) {inc = 1;} // forward order, so increment = 1.

	std::vector<vtkIdType> newPoints(size);
	for (int i = 0; i < size; ++i) {
		newPoints[i] = m_points[(index + size + i * inc) % size];
	}
	m_points = newPoints;
	updateSortedPoints();
}

void PointRing::clean(std::unordered_set<vtkIdType> finished)
{
	for (int i = 0; i < m_points.size(); ++i) {
		vtkIdType id = m_points.at(i);
		if (finished.find(id) != finished.end()) {continue;}
		finished.insert(id);
		auto it = std::find(m_points.begin() + i + 1, m_points.end(), id);
		if (it == m_points.end()) {continue;}
		int other = static_cast<int> (it - m_points.begin());
		if ((other - i) % 2 == 1) {continue;}

		// try right side
		bool rightOk = true;
		for (int k = 1; k < (other - i) / 2; ++k) {
			if (m_points.at(i + k) != m_points.at(other - k)) {
				rightOk = false;
				break;
			}
		}
		if (rightOk) {
			// Erase the items between i + 1 to other
			m_points.erase(m_points.begin() + i, m_points.begin() + other);
			continue;
		}
		// try left side
		bool leftOk = true;
		auto size = m_points.size();
		for (int k = 1; k < (size + i - other) / 2; ++k) {
			if (m_points.at((other + k) % size) != m_points.at((i - k + size) % size)) {
				leftOk = false;
				break;
			}
		}
		if (leftOk) {
			m_points.erase(m_points.begin() + other, m_points.end());
			m_points.erase(m_points.begin(), m_points.begin() + i);
			return clean(finished);
		}
	}
	updateSortedPoints();
}

void PointRing::exportHoles(std::vector<PointRing*>* rings, std::unordered_set<vtkIdType> finished)
{
	for (int i = 0; i < m_points.size() - 3; ++i) {
		vtkIdType p1 = m_points.at(i);
		if (finished.find(p1) != finished.end()) {continue;}

		auto p1It = std::find(m_points.begin() + i + 1, m_points.end(), p1);
		if (p1It == m_points.end()) {
			finished.insert(p1);
			continue;
		}

		if (i == 0) {
			bool matched = false;
			vtkIdType p2 = m_points.at(1);
			if (*(p1It - 1) == p2) {
				matched = true;
			}
			p2 = m_points.at(m_points.size() - 1);
			if (p1It + 1 != m_points.end() && *(p1It + 1) == p2) {
				matched = true;
			}
			if (matched) {
				int myFirst = m_points.front();
				m_points.erase(m_points.begin());
				m_points.push_back(myFirst);
				exportHoles(rings, finished);
				return;
			}
		}

		vtkIdType p2 = m_points.at(i + 1);
		if (*(p1It - 1) != p2) {
			finished.insert(p1);
			continue;
		}

		int otherP1Id = p1It - m_points.begin();
		int edgeLength = 1;
		while (
			edgeLength + 1 < (otherP1Id - i) / 2 &&
			m_points.at(i + edgeLength + 1) == m_points.at(otherP1Id - edgeLength - 1))
		{
			++ edgeLength;
		}
		std::vector<vtkIdType> v1, v2;
		for (int j = i + edgeLength; j < otherP1Id - edgeLength; ++j) {
			v1.push_back(m_points.at(j));
		}
		for (int j = otherP1Id; j < m_points.size(); ++j) {
			v2.push_back(m_points.at(j));
		}
		for (int j = 0; j < i; ++j) {
			v2.push_back(m_points.at(j));
		}
		m_points = v1;
		rings->push_back(new PointRing(v2, m_vtkPoints));
		return exportHoles(rings, finished);
	}
}

std::vector<PointRing*> PointRing::clean()
{
	std::unordered_set<vtkIdType> emptySet;
	clean(emptySet);
	std::vector<PointRing*> holes;
	exportHoles(&holes, emptySet);
	return holes;
}

bool PointRing::contains(vtkIdType point)
{
	return std::binary_search(m_sortedPoints.begin(), m_sortedPoints.end(), point);
}

void PointRing::updateSortedPoints()
{
	m_sortedPoints = m_points;
	std::sort(m_sortedPoints.begin(), m_sortedPoints.end());
}
