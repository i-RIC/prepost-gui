#ifndef GEODATARIVERSHAPEINTERPOLATOR_H
#define GEODATARIVERSHAPEINTERPOLATOR_H

#include "gd_riversurvey_global.h"
#include <misc/linearinterpolator.h>
#include "geodatarivercrosssection.h"

#include <map>
#include <vector>

class GeoDataRiverPathPoint;

/// Interpolator for altitude with one parameter
class AltitudeInterpolator : public InterpolatorBase
{
public:
	AltitudeInterpolator();
	virtual ~AltitudeInterpolator() override;
	virtual void updateParameters() override;
	/// Get value by interpolation
	virtual GeoDataRiverCrosssection::Altitude interpolate(double t) const = 0;
};

/// Linear interpolator for altitude with one parameter
class LinearAltitudeInterpolator : public AltitudeInterpolator
{
public:
	LinearAltitudeInterpolator(GeoDataRiverCrosssection::Altitude& v0, GeoDataRiverCrosssection::Altitude& v1);
	LinearAltitudeInterpolator(double t0, GeoDataRiverCrosssection::Altitude& v0, double t1, GeoDataRiverCrosssection::Altitude& v1);
	virtual ~LinearAltitudeInterpolator();
	void setValues(GeoDataRiverCrosssection::Altitude& v0, GeoDataRiverCrosssection::Altitude& v1);
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
	virtual ~LinearLXSecInterpolator() override;
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
	virtual ~LinearRXSecInterpolator() override;
	void updateParameters() override;
	GeoDataRiverCrosssection::Altitude interpolate(double t) const /* throw (ErrorCodes)*/ override;

protected:
	std::map<double, LinearAltitudeInterpolator*> m_interpolators;
	GeoDataRiverPathPoint* m_parent;
};

class RiverInterpolator2D1 : public Interpolator2D1
{
public:
	RiverInterpolator2D1(GeoDataRiverPathPoint* parent);
	virtual ~RiverInterpolator2D1();

	virtual void updateParameters() = 0;
	virtual QPointF interpolate(double t) const = 0;
	virtual RiverInterpolator2D1* copy() const = 0;

protected:
	GeoDataRiverPathPoint* m_parent;
};


class GD_RIVERSURVEY_EXPORT RiverSplineSolver
{
public:
	RiverSplineSolver();
	virtual ~RiverSplineSolver();

	void setHeadPoint(GeoDataRiverPathPoint* head);
	void update();

	static bool linearMode();
	static void setLinearMode(bool linearmode, GeoDataRiverPathPoint* head, bool noundo = false);

private:
	virtual QPointF getVector(GeoDataRiverPathPoint* p) = 0;
	virtual void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) = 0;

	std::vector<double> m_XA;
	std::vector<double> m_XB;
	std::vector<double> m_XC;
	std::vector<double> m_XD;
	std::vector<double> m_YA;
	std::vector<double> m_YB;
	std::vector<double> m_YC;
	std::vector<double> m_YD;
	std::vector<double> m_Dist;
	QPointF interpolate(int index, double d) const;
	GeoDataRiverPathPoint* m_headPoint;
	/// If true, linear interpolation is used.
	static bool m_linearMode;
	friend class RiverSplineInterpolator;
};

class RiverCenterLineSolver : public RiverSplineSolver
{
public:
	RiverCenterLineSolver();
	~RiverCenterLineSolver() override;

private:
	QPointF getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;
};

class RiverLeftBankSolver : public RiverSplineSolver
{
public:
	RiverLeftBankSolver();
	~RiverLeftBankSolver() override;

private:
	QPointF getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;
};

class RiverRightBankSolver : public RiverSplineSolver
{
public:
	RiverRightBankSolver();
	~RiverRightBankSolver() override;

private:
	QPointF getVector(GeoDataRiverPathPoint* p) override;
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
	RiverGridCtrlSolver();
	~RiverGridCtrlSolver() override;
	int index() const;
	void setIndex(int index);
	/// The bank side: left or right
	Bank bankSide() const;
	/// Set the bank side: left or right
	void setBankSide(Bank side);

private:
	QPointF getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;

private:
	int m_index;
	Bank m_bankSide;
};

class RiverBackgroundGridCtrlSolver : public RiverSplineSolver
{
public:
	enum Bank {
		bs_LeftBank,
		bs_RightBank
	};
	RiverBackgroundGridCtrlSolver();
	~RiverBackgroundGridCtrlSolver();

	int index() const;
	void setIndex(int index);

	double parameter() const;
	void setParameter(double param);

	Bank bankSide() const;
	void setBankSide(Bank side);

private:
	QPointF getVector(GeoDataRiverPathPoint* p) override;
	void setInterpolator(Interpolator2D1* interpolator, GeoDataRiverPathPoint* p) override;

private:
	double m_parameter;
	int m_index;
	Bank m_bankSide;
};

class RiverSplineInterpolator : public Interpolator2D1
{

public:
	RiverSplineInterpolator(RiverSplineSolver* parent, int index);
	~RiverSplineInterpolator() override;

	void updateParameters() override;
	QPointF interpolate(double t) const override;
	Interpolator2D1* copy() const override;

protected:
	RiverSplineSolver* m_parent;
	int m_Index;
};

class RiverSplineInterpolatorCopy : public Interpolator2D1
{

public:
	RiverSplineInterpolatorCopy(double d, double xa, double xb, double xc, double xd, double ya, double yb, double yc, double yd);
	~RiverSplineInterpolatorCopy() override;

	void updateParameters() override;
	QPointF interpolate(double t) const override;
	Interpolator2D1* copy() const override;

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
	RiverLinearInterpolator(RiverSplineSolver* parent, const QPointF& v0, const QPointF& v1);
	~RiverLinearInterpolator() override;

	void updateParameters() override;
	QPointF interpolate(double t) const override;
	Interpolator2D1* copy() const override;

private:
	LinearInterpolator2D1 m_Interpolator;
	RiverSplineSolver* m_Parent;
};

#endif // __RIVERSHAPEINTERPOLATOR_H__

