#ifndef RECT_H
#define RECT_H

#include "../guibase_global.h"

class GUIBASEDLL_EXPORT Rect
{
public:
	Rect();
	Rect(double xmin, double xmax, double ymin, double ymax);
	double xMin() const {return m_xMin;}
	double xMax() const {return m_xMax;}
	double yMin() const {return m_yMin;}
	double yMax() const {return m_yMax;}

	bool operator<(const Rect& rect);
	bool operator<=(const Rect& rect);

private:
	double m_xMin;
	double m_xMax;
	double m_yMin;
	double m_yMax;
};

#endif // RECT_H
