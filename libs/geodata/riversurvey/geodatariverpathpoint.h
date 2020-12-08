#ifndef GEODATARIVERPATHPOINT_H
#define GEODATARIVERPATHPOINT_H

#include "gd_riversurvey_global.h"
#include "geodatarivercrosssection.h"
#include <misc/versionnumber.h>

#include <QPointF>
#include <QVector>
#include <QString>

#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

#include <list>
#include <set>
#include <vector>

class Grid;
class Interpolator2D1;
class LinearLXSecInterpolator;
class LinearRXSecInterpolator;
class Structured2DGrid;
class GeoDataRiverSurveyBackgroundGridCreateThread;
class HydraulicDataRiverSurveyWaterElevation;

namespace iRICLib
{
	class RiverPathPoint;
}

struct CtrlPointSelectionInfo;

class GD_RIVERSURVEY_EXPORT GeoDataRiverPathPoint
{

public:
	enum Bank {
		bk_LeftBank,
		bk_RightBank
	};
	enum ErrorCodes {
		ec_PreviousPointDontExist,  ///< Failed because previous point does not exist
		ec_OutOfCrosssectionRange,  ///< In the specified value is out of crosssection range
		ec_OutOfCtrlPointRange,     ///< Specified index is out of the control point range
		ec_OutOfCtrlZoneRange,      ///< Specified index is outof the control zone range
		ec_InvalidGridSize          ///< The size of grid specified is invalid
	};
	enum CtrlPointPosition {
		pposCenterToLeft,
		pposCenterToRight,
		pposCenterLine,
		pposLeftBank,
		pposRightBank
	};
	enum CtrlZonePosition {
		zposCenterToLeft,
		zposCenterToRight,
		zposCenterLine,
		zposLeftBank,
		zposRightBank
	};
	struct CtrlPointsAddMethod {
		enum {
			am_Uniform,          ///< Uniform
			am_EqRatio_Ratio     ///< Equal ratio
		} method;
		unsigned int number; ///< Number of points to add
		double param; ///< parameter
	};

	const static QString NAME_REGEXP;

	GeoDataRiverPathPoint();
	GeoDataRiverPathPoint(double x, double y);
	GeoDataRiverPathPoint(const QString& name, double x, double y);
	virtual ~GeoDataRiverPathPoint();

	/// River center position
	const QPointF& position() const;

	void load(QDataStream& s, const VersionNumber& number);
	void save(QDataStream& s) const;
	/// Shift the river center position
	void shiftCenter(double shiftWidth);
	/// Set new river center position
	void setPosition(const QPointF& v);
	/// move the position of the river center position along the crosssection direction
	/**
	 * @param offset offset along the crosssection direction. positive value = right-bank side
	 */
	void movePosition(double offset);
	/// Returns true if this point is the first point.
	/**
	 * @note The first point is the dummy point. It does not have crosssection information.
	 */
	bool firstPoint() const;
	QPointF crosssectionPosition(double x);
	/// Add new river path point before this point.
	void insertPathPoint(GeoDataRiverPathPoint* p);
	/// Add new river path point after this point.
	void addPathPoint(GeoDataRiverPathPoint* p);
	/// Remove this point.
	void remove();
	void setCrosssectionDirection(const QPointF& v);
	/// The pointer to the previous river path point.
	GeoDataRiverPathPoint* previousPoint() const;
	/// The pointer to the next river path point.
	GeoDataRiverPathPoint* nextPoint() const;
	/// Name
	const QString& name() const;
	/// Set new name
	void setName(const QString& newname);
	GeoDataRiverCrosssection& crosssection();
	const GeoDataRiverCrosssection& crosssection() const;
	/// The direction of crosssection along left bank to right bank
	const QPointF& crosssectionDirection() const;
	/// The direction of left-bank side "wing".
	const QPointF& crosssectionDirectionL() const;
	/// The direction of right-bank side "wing".
	const QPointF& crosssectionDirectionR() const;
	/// Set the direction of crosssection
	/**
	 * @angle The angle of crosssection and the line between the center point of this crosssection and the previous crosssection
	 */
	void setCrosssectionAngle(double angle) /* throw (ErrorCodes)*/;
	/// Set the direction of crosssection along left bank to right bank
	void setCrosssectiondirection(const QPointF& v);
	/// Set the direction of left-bank side "wing".
	void setCrosssectionDirectionL(const QPointF& direction);
	/// Set the direction of right-bank side "wing".
	void setCrosssectionDirectionR(const QPointF& direction);
	QPointF GridCtrlPosition(Bank bank, unsigned int index1, double param);
	QPointF backgroundGridCtrlPosition(Bank bank, unsigned int index1, double param);
	/// Add the extention point on the left bank (that is the start point of the "wing")
	void addExtentionPointLeft(const QPointF& pos);
	/// Add the extention point on the right bank (that is the start point of the "wing")
	void addExtentionPointRight(const QPointF& pos);
	/// Move the extention point on the left bank (that is the start point of the "wing")
	void moveExtentionPointLeft(const QPointF& pos);
	/// Move the extention point on the right bank (that is the start point of the "wing")
	void moveExtentionPointRight(const QPointF& pos);
	/// Remove the extention point on the left bank (that is the start point of the "wing")
	void removeExtentionPointLeft();
	/// Remove the extention point on the right bank (that is the start point of the "wing")
	void removeExtentionPointRight();
	void updateAllXSecInterpolators();
	/// Inform that this crosssection is updated, to the upper and lower side crosssections
	void updateXSecInterpolators();
	/// Update the interpolators to calculate river shape
	void updateRiverShapeInterpolators();
	/// Update the interpolators to calculate the grid node positions
	void UpdateGridInterpolators();
	/// Returns the number of grid nodes along I-direction.
	unsigned int gridCounts(GeoDataRiverPathPoint* end) const;
	void createGrid(Structured2DGrid* grid, unsigned int initcount, bool elevmapping, bool last = false);

	/// True when this point is selected
	bool IsSelected;
	/// Disable calling updareRiverShapeInterpolators() if true
	bool InhibitInterpolatorUpdate;
	/// Return s the number of selected points including me, and the lower-side river path points
	int selectedPoints() const;

	/**
	 * @brief Set IsSelected flag true if this point is inside the specified box
	 * @param point0 base point
	 * @param v0 vector1
	 * @param v1 vector2
	 */
	void selectRegion(const QPointF& point0, const QPointF& v0, const QPointF& v1);
	void XORSelectRegion(const QPointF& point0, const QPointF& v0, const QPointF& v1);
	void SelectCtrlPointsRegion(const QPointF& point0, const QPointF& v0, const QPointF& v1, std::list<CtrlPointSelectionInfo>& info);

	void clearSelection();

	void addCtrlPoints(CtrlZonePosition position, unsigned int index, CtrlPointsAddMethod method);
	void reposCtrlPoints(CtrlPointPosition position, int minindex, int maxindex, CtrlPointsAddMethod method);
	void removeCtrlPoints(CtrlZonePosition position, std::set<int> indices);

	QPointF CtrlPointPosition2D(CtrlPointPosition pos, unsigned int index);
	QPointF CtrlPointPosition2D(CtrlPointPosition pos, double d);

	QList<QPointF> CtrlZonePoints(CtrlZonePosition position, unsigned int index, int num);
	bool gridSkip() const;
	void setGridSkip(bool skip);

	/// The interpolator to draw river center line (as spline curve)
	Interpolator2D1* riverCenter() const;
	/// Set the interpolator to draw river center line (as spline curve)
	void setRiverCenter(Interpolator2D1* interpolator);
	/// The interpolator to draw river left bank line (as spline curve)
	Interpolator2D1* leftBank() const;
	/// Set the interpolator to draw river left bank line (as spline curve)
	void setLeftBank(Interpolator2D1* interpolator);
	/// The interpolator to draw river right bank line (as spline curve)
	Interpolator2D1* rightBank() const;
	/// Set the interpolator to draw river right bank line (as spline curve)
	void setRightBank(Interpolator2D1* interpolator);
	LinearLXSecInterpolator* lXSec() const;
	LinearRXSecInterpolator* rXSec() const;
	std::vector<Interpolator2D1*>& LGridLines();
	const std::vector<Interpolator2D1*>& LGridLines() const;
	std::vector<Interpolator2D1*>& RGridLines();
	const std::vector<Interpolator2D1*>& RGridLines() const;
	std::vector<Interpolator2D1*>& backgroundLGridLines();
	const std::vector<Interpolator2D1*>& backgroundLGridLines() const;
	std::vector<Interpolator2D1*>& backgroundRGridLines();
	const std::vector<Interpolator2D1*>& backgroundRGridLines() const;
	Interpolator2D1* LGridLine(unsigned int index) const;
	Interpolator2D1* RGridLine(unsigned int index) const;
	Interpolator2D1* backgroundLGridLine(unsigned int index) const;
	Interpolator2D1* backgroundRGridLine(unsigned int index) const;
	//	void UpdateInterpolators() override;
	/// Division points between river center and left bank
	QVector<double> CenterToLeftCtrlPoints;
	/// Division points between river center and right bank
	QVector<double> CenterToRightCtrlPoints;
	/// Division points on River center line
	QVector<double> CenterLineCtrlPoints;
	/// Division points on left bank
	QVector<double> LeftBankCtrlPoints;
	/// Division points on right bank
	QVector<double> RightBankCtrlPoints;
	QVector<double>& CtrlPoints(CtrlZonePosition position);
	QVector<double>& CtrlPoints(CtrlPointPosition position);
	void centerToBanksRegion(QPointF& mins, QPointF& maxs);
	void thisToNextRegion(QPointF& mins, QPointF& maxs);
	void UpdateCtrlSections();
	vtkStructuredGrid* areaGrid() const;
	std::vector<int> getPointsToInactivateUsingWaterElevation(HydraulicDataRiverSurveyWaterElevation* we);
	void loadFromiRICLibObject(const iRICLib::RiverPathPoint* p);
	void saveToiRICLibObject(iRICLib::RiverPathPoint* p);

protected:
	void initializeInnerValues();
	/// Initialize the interpolator objects
	virtual void initializeInterpolators();

private:
	double GridCtrlParameter(Bank bank, int index1, int index2);
	QPointF myCtrlPointPosition2D(Interpolator2D1* (GeoDataRiverPathPoint::*f)() const, double d);
	QPointF myCtrlPointPosition2D(Interpolator2D1 * (GeoDataRiverPathPoint::* f)(unsigned int) const, unsigned int index, double d);
	QPointF myBgCtrlPointPosition2D(Interpolator2D1 * (GeoDataRiverPathPoint::* f)(unsigned int) const, unsigned int index, double d);
	double myHeight(GeoDataRiverPathPoint::Bank bank, double t0, double t1, double d);

	QString m_name;
	GeoDataRiverPathPoint* m_previousPoint;
	GeoDataRiverPathPoint* m_nextPoint;
	QPointF m_position;

	QPointF m_crosssectionDirection;

	QPointF m_crosssectionDirectionL;
	QPointF m_crosssectionDirectionR;

	bool m_firstPoint;

	bool m_gridSkip;
	QVector<double> m_CtrlSections;
	vtkSmartPointer<vtkStructuredGrid> m_areaGrid;

	GeoDataRiverCrosssection m_crosssection;

	Interpolator2D1* m_riverCenter;
	Interpolator2D1* m_leftBank;
	Interpolator2D1* m_rightBank;

	LinearLXSecInterpolator* m_lXSec;
	LinearRXSecInterpolator* m_rXSec;

	std::vector<Interpolator2D1*> m_LGridLines;
	std::vector<Interpolator2D1*> m_RGridLines;
	std::vector<Interpolator2D1*> m_backgroundLGridLines;
	std::vector<Interpolator2D1*> m_backgroundRGridLines;

public:
	friend class GeoDataRiverSurveyBackgroundGridCreateThread;
};

/// Selection information of grid ctrl points
struct CtrlPointSelectionInfo {
	/// The river path point that is the "parent" of the grid ctrl points
	GeoDataRiverPathPoint* Point;
	/// The position of the line on which the grid ctrl points are on
	GeoDataRiverPathPoint::CtrlPointPosition Position;
	/// Grid ctrl points index
	unsigned int Index;
	bool operator==(CtrlPointSelectionInfo& info) const {
		return (
						 Point == info.Point &&
						 Position == info.Position &&
						 Index == info.Index
					 );
	}
};

/// Selection information of grid control zone
struct CtrlZoneSelectionInfo {
	/// River path point
	GeoDataRiverPathPoint* Point;
	/// The position of the line selected
	GeoDataRiverPathPoint::CtrlZonePosition Position;
	/// Index
	int Index;
};

#endif // GEODATARIVERPATHPOINT_H
