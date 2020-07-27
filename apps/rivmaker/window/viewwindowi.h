#ifndef VIEWWINDOWI_H
#define VIEWWINDOWI_H

class ViewWindowI
{
public:
	virtual void fit() = 0;
	virtual void resetRotation() = 0;
	virtual void rotate90() = 0;

	virtual void zoomIn() = 0;
	virtual void zoomInX() = 0;
	virtual void zoomInY() = 0;
	virtual void zoomOut() = 0;
	virtual void zoomOutX() = 0;
	virtual void zoomOutY() = 0;
};

#endif // VIEWWINDOWI_H
