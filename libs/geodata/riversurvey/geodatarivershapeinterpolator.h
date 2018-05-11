#ifndef GEODATARIVERSHAPEINTERPOLATOR_H
#define GEODATARIVERSHAPEINTERPOLATOR_H

#include "gd_riversurvey_global.h"
#include <misc/linearinterpolator.h>
#include "geodatarivercrosssection.h"

#include <QVector>

class GeoDataRiverPathPoint;

/// Interpolator for altitude with one parameter
class AltitudeInterpolator : public InterpolatorBase
{

public:
	AltitudeInterpolator() {}
	virtual ~AltitudeInterpolator() {}
	virtual void updateParameters() override {}
	/// Get value by interpolation
	virtual GeoDataRiverCrosssection::Altitude interpolate(double t) const = 0;
};

/// Linear interpolator for altitude with one parameter
class LinearAltitudeInterpolator : public AltitudeInterpolator
{

public:
	LinearAltitudeInterpolator(GeoDataRiverCrosssection::Altitude& v0, GeoDataRiverCrosssection::Altitude& v1) {
		m_value0 = v0;
		m_value1 = v1;
	}
	LinearAltitudeInterpolator(double t0, GeoDataRiverCrosssection::Altitude& v0, double t1, GeoDataRiverCrosssection::Altitude& v1);
	virtual ~LinearAltitudeInterpolator() {}
	void setValues(GeoDataRiverCrosssection::Altitude& v0, GeoDataRiverCrosssection::Altitude& v1) {
		m_value0 = v0;
		m_value1 = v1;
	}
	void setValues(double t0, GeoDataRiverCrosssection::Altitude& v0, double t1, GeoDataRiverCrosssection::Altitude& v1);
	GeoDataRiverCrosssection::Altitude interpolate(double t) const override;

private:
	GeoDataRiverCrosssection::Altitude m_value0;
	GeoDataRiverCrosssection::Altitude m_value1;
};

/// Linear interpolator for left side crosssection
class LinearLXSecInterpolator : public AltitudeInterpolator
{

public:
	enum ErrorCodes {
		ec_OutOfInterpolationRange
	};
	LinearLXSecInterpolator(GeoDataRiverPathPoint* parent);
	virtual ~LinearLXSecInterpolator() {}
	void updateParameters() override;
	GeoDataRiverCrosssection::Altitude interpolate(double t) const /* throw (ErrorCodes)*/ override;

protected:
	std::map<double, LinearAltitudeInterpolator*> m_interpolators;
	GeoDataRiverPathPoint* m_parent;
};

/// Linear interpolator for right side crosssection
class LinearRXSecInterpolator : public AltitudeInterpolator
{

public:
	enum ErrorCodes {
		ec_OutOfInterpolationRange
	};
	LinearRXSecInterpolator(GeoDataRiverPathPoint* parent);
	virtual ~LinearRXSecInterpolator() {}
	void updateParameters() override;
	GeoDataRiverCrosssection::Altitude interpolate(double t) const /* throw (ErrorCodes)*/ override;

protected:
	std::map<double, LinearAltitudeInterpolator*> m_interpolators;
	GeoDataRiverPathPoint* m_parent;
};

class RiverInterpolator2D1 : public Interpolator2D1
{

public:
	RiverInterpolator2D1(GeoDataRiverPathPoint* parent) {
		m_parent = parent;
	}
	virtual ~RiverInterpolator2D1() {}
	virtual void updateParameters() = 0;
	virtual QVector2D interpolate(double t) const = 0;
	virtual RiverInterpolator2D1* copy() const = 0;

protected:
	GeoDataRiverPathPoint* m_parent;
};


class GD_RIVERSURVEY_EXPORT RiverSplineSolver
{

public:
	RiverSplineSolver() {
		m_headPoint = nullptr;
	}
	virtual ~RiverSplineSolver() {}
	void setHeadPoint(GeoDataRiverPathPoint* head) {
		m_headPoint = head;
	}
	void update();
	static bool linearMode() {return m_linearMode;}
	static void setLinearMode(bool linearmode, GeoDataRiverPathPoint* head, bool noundo = false);

protected:
	virtual QVector2D getVector(GeoDataRiverPathPoint* p) = 0;
	virtual void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) = 0;

private:
	QVector<double> m_XA;
	QVector<double> m_XB;
	QVector<double> m_XC;
	QVector<double> m_XD;
	QVector<double> m_YA;
	QVector<double> m_YB;
	QVector<double> m_YC;
	QVector<double> m_YD;
	QVector<double> m_Dist;
	QVector2D interpolate(int index, double d) const;
	GeoDataRiverPathPoint* m_headPoint;
	/// If true, linear interpolation is used.
	static bool m_linearMode;
	friend class RiverSplineInterpolator;
};

class RiverCenterLineSolver : public RiverSplineSolver
{

public:
	RiverCenterLineSolver() : RiverSplineSolver() {}
	~RiverCenterLineSolver() {}

protected:
	QVector2D getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;
};

class RiverLeftBankSolver : public RiverSplineSolver
{

public:
	RiverLeftBankSolver() : RiverSplineSolver() {}
	~RiverLeftBankSolver() {}

protected:
	QVector2D getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;
};

class RiverRightBankSolver : public RiverSplineSolver
{

public:
	RiverRightBankSolver() : RiverSplineSolver() {}
	~RiverRightBankSolver() {}

protected:
	QVector2D getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;
};

/**
 * @brief Spline curve solver for grid control points on river crosssection lines
 */
class GD_RIVERSURVEY_EXPORT RiverGridCtrlSolver : public RiverSplineSolver
{

public:
	enum Bank {
		bs_LeftBank, ///< Left bank side
		bs_RightBank ///< Right bank side
	};
	RiverGridCtrlSolver() : RiverSplineSolver() {
		m_BankSide = bs_LeftBank;
		m_Index = 0;
	}
	~RiverGridCtrlSolver() {}
	void SetIndex(int index) {m_Index = index;}
	int Index() const {return m_Index;}
	/// Set the bank side: left or right
	void SetBankSide(Bank side) {
		m_BankSide = side;
	}
	/// The bank side: left or right
	Bank BankSide() const {return m_BankSide;}

protected:
	QVector2D getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;

private:
	int m_Index;
	Bank m_BankSide;
};

class RiverBackgroundGridCtrlSolver : public RiverSplineSolver
{

public:
	enum Bank {
		bs_LeftBank,
		bs_RightBank
	};
	RiverBackgroundGridCtrlSolver() : RiverSplineSolver() {
		m_BankSide = bs_LeftBank;
		m_Parameter = 0;
		m_Index = 0;
	}
	~RiverBackgroundGridCtrlSolver() {}
	void SetIndex(int index) {m_Index = index;}
	int Index() const {return m_Index;}
	void setParameter(double param) {m_Parameter = param;}
	double parameter() const {return m_Parameter;}
	void SetBankSide(Bank side) {
		m_BankSide = side;
	}
	Bank BankSide() const {return m_BankSide;}

protected:
	QVector2D getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;

private:
	double m_Parameter;
	int m_Index;
	Bank m_BankSide;
};

class RiverSplineInterpolator : public Interpolator2D1
{

public:
	RiverSplineInterpolator(RiverSplineSolver* parent, int index) {
		m_parent = parent;
		m_Index = index;
	}
	virtual ~RiverSplineInterpolator() {}
	void updateParameters() override {
		m_parent->update();
	}
	QVector2D interpolate(double t) const override {
		return m_parent->interpolate(m_Index, t);
	}
	Interpolator2D1* copy() const override;

protected:
	RiverSplineSolver* m_parent;
	int m_Index;
};

class RiverSplineInterpolatorCopy : public Interpolator2D1
{

public:
	RiverSplineInterpolatorCopy(double d, double xa, double xb, double xc, double xd, double ya, double yb, double yc, double yd) {
		m_D = d;
		m_XA = xa;
		m_XB = xb;
		m_XC = xc;
		m_XD = xd;
		m_YA = ya;
		m_YB = yb;
		m_YC = yc;
		m_YD = yd;
	}
	virtual ~RiverSplineInterpolatorCopy() {}
	void updateParameters() override {}
	QVector2D interpolate(double t) const override {
		t *= m_D;
		double x =
			m_XA +
			m_XB * t +
			m_XC * t * t +
			m_XD * t * t * t;
		double y =
			m_YA +
			m_YB * t +
			m_YC * t * t +
			m_YD * t * t * t;
		return QVector2D(x, y);
	}
	virtual Interpolator2D1* copy() const override {
		Interpolator2D1* copy = new RiverSplineInterpolatorCopy(m_D, m_XA, m_XB, m_XC, m_XD, m_YA, m_YB, m_YC, m_YD);
		return copy;
	}

private:
	double m_D;
	double m_XA;
	double m_XB;
	double m_XC;
	double m_XD;
	double m_YA;
	double m_YB;
	double m_YC;
	double m_YD;
};

class RiverLinearInterpolator : public Interpolator2D1
{

public:
	RiverLinearInterpolator(RiverSplineSolver* parent, const QVector2D& v0, const QVector2D& v1) : Interpolator2D1() {
		m_Parent = parent;
		m_Interpolator.setValues(v0, v1);
	}
	virtual ~RiverLinearInterpolator() {}
	virtual void updateParameters() override {
		m_Parent->update();
	}
	virtual QVector2D interpolate(double t) const override {
		return m_Interpolator.interpolate(t);
	}
	virtual Interpolator2D1* copy() const override {
		return new RiverLinearInterpolator(m_Parent, m_Interpolator.interpolate(0), m_Interpolator.interpolate(1));
	}

private:
	LinearInterpolator2D1 m_Interpolator;
	RiverSplineSolver* m_Parent;
};

#endif // __RIVERSHAPEINTERPOLATOR_H__

