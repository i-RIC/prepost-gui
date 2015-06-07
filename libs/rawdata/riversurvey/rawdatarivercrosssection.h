#ifndef RAWDATARIVERCROSSSECTION_H
#define RAWDATARIVERCROSSSECTION_H

#include "rd_riversurvey_global.h"
#include <QList>
#include <QVector>

class RawDataRiverPathPoint;
/**
 * @ingroup RiverShape
 * @brief Container for river crosssection
 */
class RD_RIVERSURVEY_EXPORT RawDataRiverCrosssection
{

public:
	/// Values to be returned as exception
	enum ErrorCodes {
		ec_NoAltitudeExists,        ///< There is no altitude on this crosssection
		ec_AltitudesMustExistTwo,   ///< There must be at least two active altitude. The operation is now allowed.
		ec_OutOfIndex,              ///< The specified index does not exist
		ec_FixDelete,               ///< Tried to delete fixed point
		ec_FixInactivate,           ///< Tried to inactivate fixed points
		ec_AltitudesBiased          ///< Crosssection exist only at left bank side or only at right bank side
	};
	/// Altitude data
	class Altitude
	{

	public:
		/// Constructor
		Altitude() {
			init();
		}
		/// Constructor
		Altitude(double pos, double height) {
			init();
			m_position = pos;
			m_height = height;
		}
		/// Constructor
		Altitude(double pos, double height, bool active) {
			init();
			m_position = pos;
			m_height = height;
			m_active = active;
		}
		/// The position of the altitude. Distance from the center. if negative, left side and if positive, right side.
		double position() const {return m_position;}
		/// Set the position of the altitude. Distance from the center. if negative, left side and if positive, right side.
		void setPosition(double p) {m_position = p;}
		/// The elevation value
		double height() const {return m_height;}
		/// Set the elevation value
		void setHeight(double h) {m_height = h;}
		/// If active true
		bool active() const {return m_active;}
		/// Set whether it is active or not
		void setActive(bool a) {m_active = a;}
		bool operator <(const Altitude& alt) const {
			return m_position < alt.m_position;
		}

	private:
		void init() {
			m_active = true;
			m_tmpSelected = false;
		}
		double m_position;
		double m_height;
		bool m_active;
		bool m_tmpSelected;
	};
	typedef QList<Altitude> AltitudeList;
	/// Constructor
	RawDataRiverCrosssection() {
		m_fixedPointLSet = false;
		m_fixedPointRSet = false;
		m_leftShift = 0;
	}
	/// Destructor
	~RawDataRiverCrosssection() {}
	/// Set the parent river path point
	void setParent(RawDataRiverPathPoint* point) {
		m_parent = point;
	}
	double leftShift() const {return m_leftShift;}
	void setLeftShift(double shift) {m_leftShift = shift;}
	void moveCenter(double offset);
	void expand(double ratio);
	void addPoint(int index, double position, double height);
	int addPoint(double position, double height);
	void removePoint(int index) /*throw (ErrorCodes)*/;
	void removePoint(const QList<int>& indices) /*throw (ErrorCodes)*/;

	void movePoint(double Hoffset, double Voffset, const QList<int>& Indices);
	void movePoint(int index, double newposition, double newheight);
	void movePosition(int index, double newposition);
	void moveHeight(int index, double newheight);

	void activate(int index, bool activate);
	void activate(const QList<int>& indices, bool a) /*throw (ErrorCodes)*/;
	QList<int> selectRegion(double position1, double position2, double height1, double height2);
	int leftBankIndex(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	/**
	 * @brief Get the left bank
	 * @param OnlyActive If true, the altitude that is at the most left side and active is returned.
	 */
	Altitude& leftBank(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	int rightBankIndex(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	/**
	 * @brief Get the right bank
	 * @param OnlyActive If true, the altitude that is at the most right side and active is returned.
	 */
	Altitude& rightBank(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	/**
	 * @brief Returns the number of altitudes on the crosssection.
	 * @param OnlyActive If true, count only the active ones.
	 */
	unsigned int numOfAltitudes(bool OnlyActive = false);
	AltitudeList& AltitudeInfo() {return m_altitudeInfo;}
	const AltitudeList& AltitudeInfo() const {return m_altitudeInfo;}
	RawDataRiverPathPoint* parent() const {return m_parent;}
	bool fixedPointLSet() const {return m_fixedPointLSet;}
	const Altitude& fixedPointL() const {
		return m_altitudeInfo.at(m_fixedPointL);
	}
	int fixedPointLIndex() const {return m_fixedPointL;}
	bool fixedPointRSet() const {return m_fixedPointRSet;}
	const Altitude& fixedPointR() const {
		return m_altitudeInfo.at(m_fixedPointR);
	}
	int fixedPointRIndex() const {return m_fixedPointR;}
	void setFixedPointL(int index);
	void unsetFixedPointL();
	void setFixedPointR(int index);
	void unsetFixedPointR();
	double fixedPointLDiv() const {return m_fixedPointLDiv;}
	double fixedPointRDiv() const {return m_fixedPointRDiv;}
	void updateFixedPointDivs();

private:
	bool enoughActivePoints();
	AltitudeList m_altitudeInfo;
	RawDataRiverPathPoint* m_parent;
	bool m_fixedPointLSet;
	int m_fixedPointL;
	bool m_fixedPointRSet;
	int m_fixedPointR;
	double m_fixedPointLDiv;
	double m_fixedPointRDiv;
	double m_leftShift;
};

#endif // RAWDATARIVERCROSSSECTION_H
