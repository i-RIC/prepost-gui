#ifndef VALUEPOINTI_H
#define VALUEPOINTI_H

class ValuePointI {
public:
	virtual ~ValuePointI() {}

	virtual bool getValueAt(double x, double y, double* value) = 0;
	bool getValueAt(const QPointF& pos, double* value) {
		return getValueAt(pos.x(), pos.y(), value);
	};
};

#endif // VALUEPOINTI_H
