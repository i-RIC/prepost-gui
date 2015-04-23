#ifndef RAWDATARIVERCROSSSECTION_H
#define RAWDATARIVERCROSSSECTION_H

#include "rd_riversurvey_global.h"
#include <QList>
#include <QVector>

class RawDataRiverPathPoint;
/**
 * @ingroup RiverShape
 * @brief 河川の断面の情報を保持するクラス
 */
class RD_RIVERSURVEY_EXPORT RawDataRiverCrosssection {
public:
	/**
	 * @brief 例外処理で返す値
	 */
	enum ErrorCodes {
		ec_NoAltitudeExists,        ///< @brief この断面には、標高データがまったく存在しない
		ec_AltitudesMustExistTwo,   ///< @brief 最低2つ以上の有効なAltitude が存在しないといけないので、この操作はできません。
		ec_OutOfIndex,              ///< @brief 指定されたインデックスが存在しない
		ec_FixDelete,               ///< @brief 固定点が設定されているときに固定点を削除もしくは端点を削除しようとした　→できない
		ec_FixInactivate,           ///< Tried to inactivate fixed points.
		ec_AltitudesBiased          ///< @brief 断面が左岸側もしくは右岸側にのみ存在
	};
	/**
	 * @brief 標高データ
	 */
	class Altitude {
public:
		/// Constructor
		Altitude(){
			init();
		}
		/// Constructor
		Altitude(double pos, double height){
			init();
			m_position = pos;
			m_height = height;
		}
		/// Constructor
		Altitude(double pos, double height, bool active){
			init();
			m_position = pos;
			m_height = height;
			m_active = active;
		}
		double position() const {return m_position;}
		void setPosition(double p){m_position = p;}
		double height() const {return m_height;}
		void setHeight(double h){m_height = h;}
		bool active() const {return m_active;}
		void setActive(bool a){m_active = a;}
		bool operator <(const Altitude& alt) const{
			return m_position < alt.m_position;
		}
private:
		void init(){
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
	RawDataRiverCrosssection(){
		m_fixedPointLSet = false;
		m_fixedPointRSet = false;
		m_leftShift = 0;
	}
	/// Destructor
	~RawDataRiverCrosssection(){}
	/**
	 * @brief 親河川横断線を設定する
	 */
	void setParent(RawDataRiverPathPoint* point){
		m_parent = point;
	}
	double leftShift() const {return m_leftShift;}
	void setLeftShift(double shift){m_leftShift = shift;}
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
	 * @brief 左岸の情報を取得する
	 * @param OnlyActive true に設定すると、active なものの中でもっとも左岸よりのものを返す。
	 */
	Altitude& leftBank(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	int rightBankIndex(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	/**
	 * @brief 右岸の情報を取得する
	 * @param OnlyActive true に設定すると、active なものの中でもっとも右岸よりのものを返す。
	 */
	Altitude& rightBank(bool OnlyActive = false) /*throw (ErrorCodes)*/;
	/**
	 * @brief 断面の上に存在する点の数を数える。
	 */
	unsigned int numOfAltitudes(bool OnlyActive = false);
	AltitudeList& AltitudeInfo(){return m_altitudeInfo;}
	RawDataRiverPathPoint* parent(){return m_parent;}
	bool fixedPointLSet(){return m_fixedPointLSet;}
	const Altitude& fixedPointL(){
		return m_altitudeInfo.at(m_fixedPointL);
	}
	int fixedPointLIndex(){return m_fixedPointL;}
	bool fixedPointRSet(){return m_fixedPointRSet;}
	const Altitude& fixedPointR(){
		return m_altitudeInfo.at(m_fixedPointR);
	}
	int fixedPointRIndex(){return m_fixedPointR;}
	void setFixedPointL(int index);
	void unsetFixedPointL();
	void setFixedPointR(int index);
	void unsetFixedPointR();
	double fixedPointLDiv(){return m_fixedPointLDiv;}
	double fixedPointRDiv(){return m_fixedPointRDiv;}
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
