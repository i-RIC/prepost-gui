#ifndef RAWDATARIVERSHAPEINTERPOLATOR_H
#define RAWDATARIVERSHAPEINTERPOLATOR_H

#include "rd_riversurvey_global.h"
#include <misc/linearinterpolator.h>
#include "rawdatarivercrosssection.h"
#include <QList>
#include <QVector>
#include <QMap>

class RawDataRiverPathPoint;

class AltitudeInterpolator : public InterpolatorBase {
public:
	AltitudeInterpolator(){}
	virtual ~AltitudeInterpolator(){}
	virtual void updateParameters(){}
	virtual RawDataRiverCrosssection::Altitude interpolate(double t) = 0;
};

class LinearAltitudeInterpolator : public AltitudeInterpolator {
public:
	LinearAltitudeInterpolator(RawDataRiverCrosssection::Altitude& v0, RawDataRiverCrosssection::Altitude& v1){
		m_value0 = v0;
		m_value1 = v1;
	}
	LinearAltitudeInterpolator(double t0, RawDataRiverCrosssection::Altitude& v0, double t1, RawDataRiverCrosssection::Altitude& v1);
	virtual ~LinearAltitudeInterpolator(){}
	void setValues(RawDataRiverCrosssection::Altitude& v0, RawDataRiverCrosssection::Altitude& v1){
		m_value0 = v0;
		m_value1 = v1;
	}
	void setValues(double t0, RawDataRiverCrosssection::Altitude& v0, double t1, RawDataRiverCrosssection::Altitude& v1);
	RawDataRiverCrosssection::Altitude interpolate(double t);
private:
	RawDataRiverCrosssection::Altitude m_value0;
	RawDataRiverCrosssection::Altitude m_value1;
};

class LinearLXSecInterpolator : public AltitudeInterpolator {
public:
	enum ErrorCodes {
		ec_OutOfInterpolationRange
	};
	/**
	 * @brief コンストラクタ
	 */
	LinearLXSecInterpolator(RawDataRiverPathPoint* parent);
	/**
	 * @brief デストラクタ
	 */
	virtual ~LinearLXSecInterpolator(){}
	void updateParameters();
	RawDataRiverCrosssection::Altitude interpolate(double t) /* throw (ErrorCodes)*/;
protected:
	std::map<double, LinearAltitudeInterpolator*> m_interpolators;
	RawDataRiverPathPoint* m_parent;
};

class LinearRXSecInterpolator : public AltitudeInterpolator {
public:
	enum ErrorCodes {
		ec_OutOfInterpolationRange
	};
	LinearRXSecInterpolator(RawDataRiverPathPoint* parent);
	virtual ~LinearRXSecInterpolator(){}
	void updateParameters();
	RawDataRiverCrosssection::Altitude interpolate(double t) /* throw (ErrorCodes)*/;
protected:
	std::map<double, LinearAltitudeInterpolator*> m_interpolators;
	RawDataRiverPathPoint* m_parent;
};

/**
 * @ingroup RiverShape
 * @brief 親点に関する情報と、何点離れた点に更新があったら、次に伝えるべきかに関する
 * 情報を保持する。
 */
class RiverInterpolator2D1 : public Interpolator2D1 {
public:
	RiverInterpolator2D1(RawDataRiverPathPoint* parent){
		m_parent = parent;
	}
	virtual ~RiverInterpolator2D1(){}
	virtual void updateParameters() = 0;
	virtual QVector2D interpolate(double t) = 0;
	virtual RiverInterpolator2D1* copy() = 0;
protected:
	RawDataRiverPathPoint* m_parent;
};


/**
 * @ingroup RiverShape
 * @brief 河川に関する三次元スプラインソルバー
 *
 * 三次元(三次曲線)のスプライン曲線を生成するためのソルバー。
 * 河川横断線の間を滑らかにつないだスプライン曲線を生成するために
 * 使われる。このソルバーで生成されるスプライン曲線は、
 * 以下の局面で使用される。
 *
 * - 河川横断線の編集モードの時に、画面上に 河川横断線の中心点同士、
 *   左岸同士、右岸同士をそれぞれ結んだ曲線を描画する。
 * - 格子の生成時に、格子生成制御点同士を滑らかにつなぐようにして
 *   格子点の XY 方向の座標を算出する。
 *
 */
class RD_RIVERSURVEY_EXPORT RiverSplineSolver {
public:
	/**
	 * @brief コンストラクタ
	 */
	RiverSplineSolver(){
		m_headPoint = nullptr;
	}
	/**
	 * @brief デストラクタ
	 */
	virtual ~RiverSplineSolver(){}
	/**
	 * @brief 計算対象の河川横断線群 (互いにつながっている) の、
	 * 先頭へのポインタを渡す
	 */
	void setHeadPoint(RawDataRiverPathPoint* head){
		m_headPoint = head;
	}
	/**
	 * @brief スプライン曲線のパラメータ計算を実行する
	 */
	void update();
	static bool linearMode(){return m_linearMode;}
	static void setLinearMode(bool linearmode, RawDataRiverPathPoint* head, bool noundo = false);
protected:
	/**
	 * @brief RawDataRiverPathPoint から、ソルバーの計算に必要な、通過点のXY座標を取得する。
	 */
	virtual QVector2D getVector(RawDataRiverPathPoint* p) = 0;
	/**
	 * @brief ユーザが直接アクセスして操作する補間用オブジェクトと、対応する
	 * 河川横断線を登録する。
	 * @param interpolator 補間オブジェクト
	 * @param p interpolator が担当する区間の両端をなす2つの河川横断線のうち、
	 * 上流側の河川横断線へのポインタ
	 */
	virtual void setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p) = 0;
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
	/**
	 * @brief Update() によって算出されたパラメータをもとに、スプライン曲線上にある
	 * 点の座標を返す。
	 * @param index 区間の index。最上流側の河川横断線と、それに隣り合う河川横断線との
	 * 間の区間が 0、以降 1, 2, 3... となる。
	 * @param d 区間上での位置。 0と1の間の値を渡す。
	 * 0 だと、上流側の河川横断線上, 1 だと下流側の河川横断線上の点になる。
	 */
	QVector2D interpolate(int index, double d);
	/**
	 * @brief 計算対象の河川横断線群 (互いにつながっている) の、先頭へのポインタ
	 */
	RawDataRiverPathPoint* m_headPoint;
	/**
	 * @brief 線形補間モード。 true の場合、線形補間が行われる
	 */
	static bool m_linearMode;
	friend class RiverSplineInterpolator;
};

/**
 * @brief 河川横断線の、中心点同士を結ぶスプライン曲線を計算するソルバー
 */
class RiverCenterLineSolver : public RiverSplineSolver {
public:
	/**
	 * @brief コンストラクタ
	 */
	RiverCenterLineSolver() : RiverSplineSolver(){}
	/**
	 * @brief デストラクタ
	 */
	~RiverCenterLineSolver(){}
protected:
	QVector2D getVector(RawDataRiverPathPoint* p);
	void setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p);
};

/**
 * @brief 河川横断線の、左岸同士を結ぶスプライン曲線を計算するソルバー
 */
class RiverLeftBankSolver : public RiverSplineSolver {
public:
	/**
	 * @brief コンストラクタ
	 */
	RiverLeftBankSolver() : RiverSplineSolver(){}
	/**
	 * @brief デストラクタ
	 */
	~RiverLeftBankSolver(){}
protected:
	QVector2D getVector(RawDataRiverPathPoint* p);
	void setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p);
};

/**
 * @brief 河川横断線の、右岸同士を結ぶスプライン曲線を計算するソルバー
 */
class RiverRightBankSolver : public RiverSplineSolver {
public:
	/**
	 * @brief コンストラクタ
	 */
	RiverRightBankSolver() : RiverSplineSolver(){}
	/**
	 * @brief デストラクタ
	 */
	~RiverRightBankSolver(){}
protected:
	QVector2D getVector(RawDataRiverPathPoint* p);
	void setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p);
};

/**
 * @brief 河川横断線上に配置された格子生成制御点同士を結ぶ
 * スプライン曲線を計算するソルバー
 */
class RD_RIVERSURVEY_EXPORT RiverGridCtrlSolver : public RiverSplineSolver {
public:
	enum Bank {
		/**
		 * @brief 左岸側
		 */
		bs_LeftBank,
		/**
		 * @brief 右岸側
		 */
		bs_RightBank
	};
	/**
	 * @brief コンストラクタ
	 */
	RiverGridCtrlSolver() : RiverSplineSolver(){
		m_BankSide = bs_LeftBank;
		m_Index = 0;
	}
	/**
	 * @brief デストラクタ
	 */
	~RiverGridCtrlSolver(){}
	void SetIndex(int index){m_Index = index;}
	int Index() const {return m_Index;}
	/**
	 * @brief 対象とする格子生成制御点が左岸側か、右岸側かを設定
	 */
	void SetBankSide(Bank side){
		m_BankSide = side;
	}
	Bank BankSide() const {return m_BankSide;}
protected:
	QVector2D getVector(RawDataRiverPathPoint* p);
	void setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p);
private:
	int m_Index;
	Bank m_BankSide;
};

/**
 * @brief 河川横断線上に配置された格子生成制御点同士を結ぶ
 * スプライン曲線を計算するソルバー
 */
class RiverBackgroundGridCtrlSolver : public RiverSplineSolver {
public:
	enum Bank {
		/**
		 * @brief 左岸側
		 */
		bs_LeftBank,
		/**
		 * @brief 右岸側
		 */
		bs_RightBank
	};
	/**
	 * @brief コンストラクタ
	 */
	RiverBackgroundGridCtrlSolver() : RiverSplineSolver(){
		m_BankSide = bs_LeftBank;
		m_Parameter = 0;
		m_Index = 0;
	}
	/**
	 * @brief デストラクタ
	 */
	~RiverBackgroundGridCtrlSolver(){}
	void SetIndex(int index){m_Index = index;}
	int Index() const {return m_Index;}
	void setParameter(double param){m_Parameter = param;}
	double parameter() const {return m_Parameter;}
	/**
	 * @brief 対象とする格子生成制御点が左岸側か、右岸側かを設定
	 */
	void SetBankSide(Bank side){
		m_BankSide = side;
	}
	Bank BankSide() const {return m_BankSide;}
protected:
	QVector2D getVector(RawDataRiverPathPoint* p);
	void setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p);
private:
	double m_Parameter;
	int m_Index;
	Bank m_BankSide;
};

/**
 * @brief 河川横断線同士を結んだスプライン曲線を用いて、補間処理を行うオブジェクト
 *
 * このオブジェクト自身はスプライン曲線のパラメータの値は持たず、コンストラクタ
 * で渡された RiverSplineSolver オブジェクトに問い合わせることによって
 * 座標を得る。
 */
class RiverSplineInterpolator : public Interpolator2D1 {
public:
	/**
	 * @brief コンストラクタ
	 * @param parent 実際のスプライン曲線のパラメータの値を保持するソルバーへのポインタ
	 * @param index parent のソルバーが保持するスプライン曲線群上で、どの区間についての
	 * 値を返すかの index。 0 の場合、最上流側の河川横断線とそれに隣り合う河川横断線
	 * との間の区間となる。
	 */
	RiverSplineInterpolator(RiverSplineSolver* parent, int index){
		m_parent = parent;
		m_Index = index;
	}
	/**
	 * @brief デストラクタ
	 */
	virtual ~RiverSplineInterpolator(){}
	/**
	 * @brief 河川形状が編集され、スプライン曲線を再計算する必要が生じたことを親ソルバーに
	 * 通知
	 */
	void updateParameters(){
		m_parent->update();
	}
	/**
	 * @brief 補間して得られる点を返す
	 */
	QVector2D interpolate(double t){
		return m_parent->interpolate(m_Index, t);
	}
	/**
	 * @brief オブジェクトのコピー
	 */
	Interpolator2D1* copy();
protected:
	RiverSplineSolver* m_parent;
	int m_Index;
};

/**
 * @brief 河川横断線同士を結んだスプライン曲線を用いて、補間処理を行うオブジェクトの、コピー
 *
 */
class RiverSplineInterpolatorCopy : public Interpolator2D1 {
public:
	/**
	 * @brief コンストラクタ
	 */
	RiverSplineInterpolatorCopy(double d, double xa, double xb, double xc, double xd, double ya, double yb, double yc, double yd){
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
	/**
	 * @brief デストラクタ
	 */
	virtual ~RiverSplineInterpolatorCopy(){}
	void updateParameters(){}
	/**
	 * @brief 補間して得られる点を返す
	 */
	QVector2D interpolate(double t){
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
	/**
	 * @brief オブジェクトのコピー
	 */
	virtual Interpolator2D1* copy(){
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

/**
 * @ingroup RiverShape
 * @brief コンストラクタで指定された2点を線形補間するオブジェクト。
 * updateParameters() が呼ばれると、親ソルバに Interpolator の再生成を依頼する。
 * RiverShapeSolver::m_LinearMode = true のときのみ使用される。
 */
class RiverLinearInterpolator : public Interpolator2D1 {
public:
	RiverLinearInterpolator(RiverSplineSolver* parent, const QVector2D& v0, const QVector2D& v1) : Interpolator2D1(){
		m_Parent = parent;
		m_Interpolator.setValues(v0, v1);
	}
	virtual ~RiverLinearInterpolator(){}
	virtual void updateParameters(){
		m_Parent->update();
	}
	virtual QVector2D interpolate(double t){
		return m_Interpolator.interpolate(t);
	}
	virtual Interpolator2D1* copy(){
		return new RiverLinearInterpolator(m_Parent, m_Interpolator.interpolate(0), m_Interpolator.interpolate(1));
	}
private:
	LinearInterpolator2D1 m_Interpolator;
	RiverSplineSolver* m_Parent;
};

/**
 * @brief 親点に関する情報と、何点離れた点に更新があったら、次に伝えるべきかに関する
 * 情報を保持する。
 */
/*
	 class RiverGridInterpolator : public RiverInterpolator2D1{
	 public:
		enum Bank{
				bk_LeftBank,
				bk_RightBank
		};
		RiverGridInterpolator(RawDataRiverPathPoint* parent, Bank bank, int index) : RiverInterpolator2D1(parent){
				m_Bank = bank;
				m_Index = index;
		}
		void SetBankIndex(Bank bank, int index){
				m_Bank = bank;
				m_Index = index;
		}
		virtual ~RiverGridInterpolator(){}
		virtual void updateParameters() = 0;
		virtual QVector2D interpolate(double t) = 0;
		virtual RiverInterpolator2D1* copy() = 0;
	 protected:
		Bank m_Bank;
		int m_Index;
	 };
	 class LinearRiverGridInterpolator : public RiverGridInterpolator{
	 public:
		LinearRiverGridInterpolator(RawDataRiverPathPoint* parent, Bank bank, int index)
				: RiverGridInterpolator(parent, bank, index){}
		~LinearRiverGridInterpolator(){}
		void updateParameters() = 0;
		QVector2D interpolate(double t) = 0;
	 //	bool InformUp(int count){return count < 2;}
	 //	bool InformDown(int count){return count < 1;}
		virtual RiverInterpolator2D1* copy() = 0;
	 private:
		LinearInterpolator2D1 m_Interpolator;
	 };
 */

#endif // __RIVERSHAPEINTERPOLATOR_H__

