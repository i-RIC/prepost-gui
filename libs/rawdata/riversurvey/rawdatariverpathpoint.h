#ifndef RAWDATARIVERPATHPOINT_H
#define RAWDATARIVERPATHPOINT_H

#include "rd_riversurvey_global.h"
#include "rawdatarivercrosssection.h"
#include <misc/versionnumber.h>
#include <QVector2D>
#include <QVector>
#include <set>

#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>

class Grid;
class Interpolator2D1;
class LinearLXSecInterpolator;
class LinearRXSecInterpolator;
class Structured2DGrid;
class RawDataRiverSurveyBackgroundGridCreateThread;
//class RiverGridInterpolator;
//class BStream;
namespace iRICLib
{
	class RiverPathPoint;
}

struct CtrlPointSelectionInfo;

class RD_RIVERSURVEY_EXPORT RawDataRiverPathPoint
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
	RawDataRiverPathPoint() {
		initializeInnerValues();
		initializeInterpolators();
	}
	RawDataRiverPathPoint(double x, double y) {
		initializeInnerValues();
		initializeInterpolators();
		m_position = QVector2D(x, y);
	}
	RawDataRiverPathPoint(const QString& name, double x, double y) {
		initializeInnerValues();
		initializeInterpolators();
		m_name = name;
		m_position = QVector2D(x, y);
	}
	virtual ~RawDataRiverPathPoint(){}
	/// River center position
	const QVector2D& position() const {return m_position;}
	void load(QDataStream& s, const VersionNumber& number);
	void save(QDataStream& s);
	/// Shift the river center position
	void shiftCenter(double shiftWidth);
	/// Set new river center position
	void setPosition(const QVector2D& v);
	/// move the position of the river center position along the crosssection direction
	/**
	 * @param offset offset along the crosssection direction. positive value = right-bank side
	 */
	void movePosition(double offset);
	/// Returns true if this point is the first point.
	/**
	 * @note The first point is the dummy point. It does not have crosssection information.
	 */
	bool firstPoint() const {return m_firstPoint;}
	QVector2D crosssectionPosition(double x);
	/// Add new river path point before this point.
	void insertPathPoint(RawDataRiverPathPoint* p);
	/// Add new river path point after this point.
	void addPathPoint(RawDataRiverPathPoint* p);
	/// Remove this point.
	void remove();
	void setCrosssectionDirection(const QVector2D& v);
	/// The pointer to the previous river path point.
	RawDataRiverPathPoint* previousPoint() {return m_previousPoint;}
	/// The pointer to the next river path point.
	RawDataRiverPathPoint* nextPoint() {return m_nextPoint;}
	/// Name
	const QString& name() const {return m_name;}
	/// Set new name
	void setName(const QString& newname);
	RawDataRiverCrosssection& crosssection() {return m_crosssection;}
	/// The direction of crosssection along left bank to right bank
	const QVector2D& crosssectionDirection() const {return m_crosssectionDirection;}
	/// The direction of left-bank side "wing".
	const QVector2D& crosssectionDirectionL() const {return m_crosssectionDirectionL;}
	/// The direction of right-bank side "wing".
	const QVector2D& crosssectionDirectionR() const {return m_crosssectionDirectionR;}
	/// Set the direction of crosssection
	/**
	 * @angle The angle of crosssection and the line between the center point of this crosssection and the previous crosssection
	 */
	void setCrosssectionAngle(double angle) /* throw (ErrorCodes)*/;
	/// Set the direction of crosssection along left bank to right bank
	void setCrosssectiondirection(const QVector2D& v);
	/// Set the direction of left-bank side "wing".
	void setCrosssectionDirectionL(const QVector2D& direction);
	/// Set the direction of right-bank side "wing".
	void setCrosssectionDirectionR(const QVector2D& direction);
	QVector2D GridCtrlPosition(Bank bank, unsigned int index1, double param);
	QVector2D backgroundGridCtrlPosition(Bank bank, unsigned int index1, double param);
	/// Add the extention point on the left bank (that is the start point of the "wing")
	void addExtentionPointLeft(const QVector2D& pos);
	/// Add the extention point on the right bank (that is the start point of the "wing")
	void addExtentionPointRight(const QVector2D& pos);
	/// Move the extention point on the left bank (that is the start point of the "wing")
	void moveExtentionPointLeft(const QVector2D& pos);
	/// Move the extention point on the right bank (that is the start point of the "wing")
	void moveExtentionPointRight(const QVector2D& pos);
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
	unsigned int gridCounts(RawDataRiverPathPoint* end) {
		if (this == end) {
			return 1;
		} else {
			if (! m_gridSkip) {
				return 1 + static_cast<unsigned int>(CenterLineCtrlPoints.size()) + m_nextPoint->gridCounts(end);
			} else {
				return m_nextPoint->gridCounts(end);
			}
		}
	}
	void createGrid(Structured2DGrid* grid, unsigned int initcount, bool elevmapping, bool last = false);

	/// True when this point is selected
	bool IsSelected;
	/// Disable calling updareRiverShapeInterpolators() if true
	bool InhibitInterpolatorUpdate;
	/// Return s the number of selected points including me, and the lower-side river path points
	int selectedPoints() {
		int selectedpoints = 0;
		if (IsSelected) {
			++ selectedpoints;
		}
		if (m_nextPoint != 0) {
			return selectedpoints + m_nextPoint->selectedPoints();
		} else {
			return selectedpoints;
		}
	}
	/**
	 * @brief Set IsSelected flag true if this point is inside the specified box
	 * @param point0 base point
	 * @param v0 vector1
	 * @param v1 vector2
	 */
	void selectRegion(const QVector2D& point0, const QVector2D& v0, const QVector2D& v1);
	void XORSelectRegion(const QVector2D& point0, const QVector2D& v0, const QVector2D& v1);
	void SelectCtrlPointsRegion(const QVector2D& point0, const QVector2D& v0, const QVector2D& v1, std::list<CtrlPointSelectionInfo>& info);

	/// Clear selection.
	void clearSelection() {
		IsSelected = false;
		if (m_nextPoint != 0) {
			m_nextPoint->clearSelection();
		}
	}
	void addCtrlPoints(CtrlZonePosition position, unsigned int index, CtrlPointsAddMethod method);
	void reposCtrlPoints(CtrlPointPosition position, int minindex, int maxindex, CtrlPointsAddMethod method);
	void removeCtrlPoints(CtrlZonePosition position, std::set<int> indices);

	QVector2D CtrlPointPosition2D(CtrlPointPosition pos, unsigned int index);
	QVector2D CtrlPointPosition2D(CtrlPointPosition pos, double d);
	void clearWaterSurfaceElevation();
	void setWaterSurfaceElevation(double value);
	bool waterSurfaceElevationSpecified() const;
	double waterSurfaceElevationValue() const;

	QList<QVector2D> CtrlZonePoints(CtrlZonePosition position, unsigned int index, int num);
	bool gridSkip() const {return m_gridSkip;}
	void setGridSkip(bool skip) {
		if (m_nextPoint == 0) {return;}
		if (firstPoint()) {return;}
		if (m_previousPoint->firstPoint()) {return;}
		m_gridSkip = skip;
	}
	/// The interpolator to draw river center line (as spline curve)
	Interpolator2D1* riverCenter() const {return m_riverCenter;}
	/// Set the interpolator to draw river center line (as spline curve)
	void setRiverCenter(Interpolator2D1* interpolator) {m_riverCenter = interpolator;}
	/// The interpolator to draw river left bank line (as spline curve)
	Interpolator2D1* leftBank() const {return m_leftBank;}
	/// Set the interpolator to draw river left bank line (as spline curve)
	void setLeftBank(Interpolator2D1* interpolator) {m_leftBank = interpolator;}
	/// The interpolator to draw river right bank line (as spline curve)
	Interpolator2D1* rightBank() const {return m_rightBank;}
	/// Set the interpolator to draw river right bank line (as spline curve)
	void setRightBank(Interpolator2D1* interpolator) {m_rightBank = interpolator;}
	LinearLXSecInterpolator* lXSec() const {return m_lXSec;}
	LinearRXSecInterpolator* rXSec() const {return m_rXSec;}
	QVector<Interpolator2D1*>& LGridLines() {return m_LGridLines;}
	const QVector<Interpolator2D1*>& LGridLines() const {return m_LGridLines;}
	QVector<Interpolator2D1*>& RGridLines() {return m_RGridLines;}
	const QVector<Interpolator2D1*>& RGridLines() const {return m_RGridLines;}
	QVector<Interpolator2D1*>& backgroundLGridLines() {return m_backgroundLGridLines;}
	const QVector<Interpolator2D1*>& backgroundLGridLines() const {return m_backgroundLGridLines;}
	QVector<Interpolator2D1*>& backgroundRGridLines() {return m_backgroundRGridLines;}
	const QVector<Interpolator2D1*>& backgroundRGridLines() const {return m_backgroundRGridLines;}
	Interpolator2D1* LGridLine(unsigned int index) const {return m_LGridLines[index];}
	Interpolator2D1* RGridLine(unsigned int index) const {return m_RGridLines[index];}
	Interpolator2D1* backgroundLGridLine(unsigned int index) const {return m_backgroundLGridLines[index];}
	Interpolator2D1* backgroundRGridLine(unsigned int index) const {return m_backgroundRGridLines[index];}
	//	void UpdateInterpolators();
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
	QVector<double>& CtrlPoints(CtrlZonePosition position) {
		switch (position) {
		case zposCenterToLeft:
			return CenterToLeftCtrlPoints;
			break;
		case zposCenterToRight:
			return CenterToRightCtrlPoints;
			break;
		case zposCenterLine:
			return CenterLineCtrlPoints;
			break;
		case zposLeftBank:
			return LeftBankCtrlPoints;
			break;
		case zposRightBank:
			return RightBankCtrlPoints;
			break;
			// Never used. this is written to avoid warning in compiling.
		default:
			return RightBankCtrlPoints;
		}
	}
	QVector<double>& CtrlPoints(CtrlPointPosition position) {
		switch (position) {
		case pposCenterToLeft:
			return CenterToLeftCtrlPoints;
			break;
		case pposCenterToRight:
			return CenterToRightCtrlPoints;
			break;
		case pposCenterLine:
			return CenterLineCtrlPoints;
			break;
		case pposLeftBank:
			return LeftBankCtrlPoints;
			break;
		case pposRightBank:
			return RightBankCtrlPoints;
			break;
			// Never used. this is written to avoid warning in compiling.
		default:
			return RightBankCtrlPoints;
		}
	}
	void centerToBanksRegion(QVector2D& mins, QVector2D& maxs);
	void thisToNextRegion(QVector2D& mins, QVector2D& maxs);
	void UpdateCtrlSections();
	vtkStructuredGrid* areaGrid() {return m_areaGrid;}
	QList<int> getPointsToInactivateUsingWaterElevation();
	void loadFromiRICLibObject(const iRICLib::RiverPathPoint* p);
	void saveToiRICLibObject(iRICLib::RiverPathPoint* p);

protected:
	void initializeInnerValues();
	/// Initialize the interpolator objects
	virtual void initializeInterpolators();

private:
	double GridCtrlParameter(Bank bank, int index1, int index2);
	QVector2D myCtrlPointPosition2D(Interpolator2D1* (RawDataRiverPathPoint::*f)() const, double d);
	QVector2D myCtrlPointPosition2D(Interpolator2D1 * (RawDataRiverPathPoint::* f)(unsigned int) const, unsigned int index, double d);
	QVector2D myBgCtrlPointPosition2D(Interpolator2D1 * (RawDataRiverPathPoint::* f)(unsigned int) const, unsigned int index, double d);
	double myHeight(RawDataRiverPathPoint::Bank bank, double t0, double t1, double d);

	QString m_name;
	RawDataRiverPathPoint* m_previousPoint;
	RawDataRiverPathPoint* m_nextPoint;
	QVector2D m_position;
	bool m_waterSurfaceElevationSpecified;
	double m_waterSurfaceElevationValue;

	QVector2D m_crosssectionDirection;

	QVector2D m_crosssectionDirectionL;
	QVector2D m_crosssectionDirectionR;

	bool m_firstPoint;

	bool m_gridSkip;
	QVector<double> m_CtrlSections;
	vtkSmartPointer<vtkStructuredGrid> m_areaGrid;

	RawDataRiverCrosssection m_crosssection;

	Interpolator2D1* m_riverCenter;

	Interpolator2D1* m_leftBank;

	Interpolator2D1* m_rightBank;

	LinearLXSecInterpolator* m_lXSec;

	LinearRXSecInterpolator* m_rXSec;

	QVector<Interpolator2D1*> m_LGridLines;

	QVector<Interpolator2D1*> m_RGridLines;

	QVector<Interpolator2D1*> m_backgroundLGridLines;

	QVector<Interpolator2D1*> m_backgroundRGridLines;
public:
	friend class RawDataRiverSurveyBackgroundGridCreateThread;
};

/// Selection information of grid ctrl points
struct CtrlPointSelectionInfo {
	/// The river path point that is the "parent" of the grid ctrl points
	RawDataRiverPathPoint* Point;
	/// The position of the line on which the grid ctrl points are on
	RawDataRiverPathPoint::CtrlPointPosition Position;
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
	RawDataRiverPathPoint* Point;
	/// The position of the line selected
	RawDataRiverPathPoint::CtrlZonePosition Position;
	/// Index
	int Index;
};

#endif // RAWDATARIVERPATHPOINT_H
