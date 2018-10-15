#ifndef EDGE_H
#define EDGE_H

#include <vtkType.h>

class QDataStream;

/// VTK cell edge. It has two vertex as vtkIdType, i.e. indices.
class Edge
{
public:
	Edge();
	Edge(vtkIdType v1, vtkIdType v2);

	vtkIdType vertex1() const;
	void setVertex1(vtkIdType id);

	vtkIdType vertex2() const;
	void setVertex2(vtkIdType id);

	bool operator==(const Edge& e) const;
	bool operator<(const Edge& e) const;

private:
	vtkIdType m_vertex1;
	vtkIdType m_vertex2;
};

QDataStream& operator<<(QDataStream& out, const Edge& edge);
QDataStream& operator>>(QDataStream& in, Edge edge);

uint qHash(const Edge& e);

#include "private/edge_detail.h"

#endif // EDGE_H
