#ifndef EDGE_H
#define EDGE_H

#include <QDataStream>
#include <vtkObject.h>

class Edge {
public:
	Edge (){
		m_vertex1 = 0;
		m_vertex2 = 0;
	}

	Edge (vtkIdType v1, vtkIdType v2){
		if (v1 < v2){
			m_vertex1 = v1;
			m_vertex2 = v2;
		} else {
			m_vertex1 = v2;
			m_vertex2 = v1;
		}
	}
	vtkIdType vertex1() const {return m_vertex1;}
	vtkIdType vertex2() const {return m_vertex2;}
	void setVertex1(vtkIdType id){m_vertex1 = id;}
	void setVertex2(vtkIdType id){m_vertex2 = id;}

	bool operator==(const Edge& e) const {
		return m_vertex1 == e.m_vertex1 && m_vertex2 == e.m_vertex2;
	}

	bool operator <(const Edge& e) const {
		if (m_vertex1 != e.m_vertex1){
			return m_vertex1 < e.m_vertex1;
		} else {
			return m_vertex2 < e.m_vertex2;
		}
	}

private:
	vtkIdType m_vertex1;
	vtkIdType m_vertex2;
};

inline QDataStream &operator<<(QDataStream &out, const Edge &edge){
	out << edge.vertex1() << edge.vertex2();
	return out;
}

inline QDataStream &operator>>(QDataStream &in, Edge edge){
	quint32 v1, v2;
	in >> v1 >> v2;
	edge.setVertex1(v1);
	edge.setVertex2(v2);
	return in;
}

inline uint qHash(const Edge& e){
	return qHash(e.vertex1()) / 2 + qHash(e.vertex2()) / 2;
}

#endif // EDGE_H
