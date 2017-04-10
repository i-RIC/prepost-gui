#ifndef COORDINATESI_H
#define COORDINATESI_H

class CoordinatesI
{
public:
	virtual ~CoordinatesI() {}

	virtual std::vector<QPointF> coordinates() const = 0;
	virtual void setCoordinates(const std::vector<QPointF>& coords) = 0;
};

#endif // COORDINATESI_H
