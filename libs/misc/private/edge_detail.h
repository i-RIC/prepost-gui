#ifndef EDGE_DETAIL_H
#define EDGE_DETAIL_H

#include "../edge.h"

#include <QHash>

#include <vtkType.h>

inline Edge::Edge()
{
	m_vertex1 = 0;
	m_vertex2 = 0;
}

inline Edge::Edge(vtkIdType v1, vtkIdType v2)
{
	if (v1 < v2) {
		m_vertex1 = v1;
		m_vertex2 = v2;
	} else {
		m_vertex1 = v2;
		m_vertex2 = v1;
	}
}

inline vtkIdType Edge::vertex1() const
{
	return m_vertex1;
}

inline void Edge::setVertex1(vtkIdType id)
{
	m_vertex1 = id;
}

inline vtkIdType Edge::vertex2() const
{
	return m_vertex2;
}

inline void Edge::setVertex2(vtkIdType id)
{
	m_vertex2 = id;
}

inline bool Edge::operator==(const Edge& e) const
{
	return m_vertex1 == e.m_vertex1 && m_vertex2 == e.m_vertex2;
}

inline bool Edge::operator <(const Edge& e) const
{
	if (m_vertex1 != e.m_vertex1) {
		return m_vertex1 < e.m_vertex1;
	} else {
		return m_vertex2 < e.m_vertex2;
	}
}

inline QDataStream& operator<<(QDataStream& out, const Edge& edge)
{
	out << edge.vertex1() << edge.vertex2();
	return out;
}

inline QDataStream& operator>>(QDataStream& in, Edge edge)
{
	quint32 v1, v2;
	in >> v1 >> v2;
	edge.setVertex1(v1);
	edge.setVertex2(v2);
	return in;
}

inline uint qHash(const Edge& e)
{
	return qHash(e.vertex1()) / 2 + qHash(e.vertex2()) / 2;
}

#endif // EDGE_DETAIL_H
