#include "rawdatariverpathpoint.h"
#include "rawdatarivershapeinterpolator.h"

bool RiverSplineSolver::m_linearMode = false;

LinearAltitudeInterpolator::LinearAltitudeInterpolator(double t0, RawDataRiverCrosssection::Altitude& v0, double t1, RawDataRiverCrosssection::Altitude& v1){
	m_value0.setPosition((t1 * v0.position() - t0 * v1.position()) / (t1 - t0));
	m_value0.setHeight((t1 * v0.height() - t0 * v1.height()) / (t1 - t0));
	m_value1.setPosition(((1 - t0) * v1.position() + (t1 - 1) * v0.position()) / (t1 - t0));
	m_value1.setHeight(((1 - t0) * v1.height() + (t1 - 1) * v0.height()) / (t1 - t0));
}

void LinearAltitudeInterpolator::setValues(double t0, RawDataRiverCrosssection::Altitude& v0, double t1, RawDataRiverCrosssection::Altitude& v1){
	m_value0.setPosition((t1 * v0.position() - t0 * v1.position()) / (t1 - t0));
	m_value0.setHeight((t1 * v0.height() - t0 * v1.height()) / (t1 - t0));
	m_value1.setPosition(((1 - t0) * v1.position() + (t1 - 1) * v0.position()) / (t1 - t0));
	m_value1.setHeight(((1 - t0) * v1.height() + (t1 - 1) * v0.height()) / (t1 - t0));
}

RawDataRiverCrosssection::Altitude LinearAltitudeInterpolator::interpolate(double t){
	RawDataRiverCrosssection::Altitude alt;
	alt.setPosition(m_value1.position() * t + m_value0.position() * (1. - t));
	alt.setHeight(m_value1.height() * t + m_value0.height() * (1. - t));
	return alt;
}

LinearLXSecInterpolator::LinearLXSecInterpolator(RawDataRiverPathPoint* parent) : AltitudeInterpolator(){
	m_parent = parent;
	updateParameters();
}

void LinearLXSecInterpolator::updateParameters(){
	// まず、左岸の座標を取得
	RawDataRiverCrosssection::Altitude leftbank;
	try{
		leftbank = m_parent->crosssection().leftBank(true);
	}catch (RawDataRiverCrosssection::ErrorCodes /* codes */){}
	// まず、オブジェクトを削除
	for (auto l_it = m_interpolators.begin(); l_it != m_interpolators.end(); ++l_it){
		delete l_it->second;
	}
	m_interpolators.clear();
	RawDataRiverCrosssection::AltitudeList& AList = m_parent->crosssection().AltitudeInfo();
	auto it = AList.begin();
	if (it == AList.end()) return;
	while (it != AList.end() - 1 && it->position() <= 0){
		while (it != AList.end() - 1 && ! it->active()){
			++it;
		}
		auto it2 = it;
		if (it2 != AList.end() - 1){++it2;}
		while (it2 != AList.end() - 1 && ! it2->active()){
			++it2;
		}
		if (it != it2 && it->position() != it2->position()){
			LinearAltitudeInterpolator* interpolator = new LinearAltitudeInterpolator(
				it2->position() / leftbank.position(), *it2,
				it->position() / leftbank.position(), *it
			    );
			std::pair<double, LinearAltitudeInterpolator*> dl_pair =
			    std::pair<double, LinearAltitudeInterpolator*>(it->position() / leftbank.position(), interpolator);
			m_interpolators.insert(dl_pair);
		}
		if (it != AList.end() - 1){++it;}
	}
}

RawDataRiverCrosssection::Altitude LinearLXSecInterpolator::interpolate(double t) /*throw (ErrorCodes)*/ {
	if (t < 0 || t > 1){
		throw ec_OutOfInterpolationRange;
	}
	auto it = m_interpolators.lower_bound(t);
	return (it->second)->interpolate(t);
}

LinearRXSecInterpolator::LinearRXSecInterpolator(RawDataRiverPathPoint* parent) : AltitudeInterpolator(){
	m_parent = parent;
	updateParameters();
}

void LinearRXSecInterpolator::updateParameters(){
	// まず、右岸の座標を取得
	RawDataRiverCrosssection::Altitude rightbank;
	try{
		rightbank = m_parent->crosssection().rightBank(true);
	}catch (RawDataRiverCrosssection::ErrorCodes /* codes */){}
	// まず、オブジェクトを削除
	for (auto l_it = m_interpolators.begin(); l_it != m_interpolators.end(); ++l_it){
		delete l_it->second;
	}
	m_interpolators.clear();
	RawDataRiverCrosssection::AltitudeList& AList = m_parent->crosssection().AltitudeInfo();
	auto it = AList.end();
	if (it == AList.begin()) return;
	--it;
	while (it != AList.begin() && it->position() >= 0){
		while (it != AList.begin() && ! it->active()){
			--it;
		}
		auto it2 = it;
		if (it2 != AList.begin()){--it2;}
		while (it2 != AList.begin() && ! it2->active()){
			--it2;
		}
		if (it != it2 && it->position() != it2->position()){
			LinearAltitudeInterpolator* interpolator = new LinearAltitudeInterpolator(
				it2->position() / rightbank.position(), *it2,
				it->position() / rightbank.position(), *it
			    );
			std::pair<double, LinearAltitudeInterpolator*> dl_pair =
			    std::pair<double, LinearAltitudeInterpolator*>(it->position() / rightbank.position(), interpolator);
			m_interpolators.insert(dl_pair);
		}
		if (it != AList.begin()){--it;}
	}
}

RawDataRiverCrosssection::Altitude LinearRXSecInterpolator::interpolate(double t) /*throw (ErrorCodes)*/ {
	if (t < 0 || t > 1){
		throw ec_OutOfInterpolationRange;
	}
	auto it = m_interpolators.lower_bound(t);
	return (it->second)->interpolate(t);
}

void RiverSplineSolver::update(){
	if (m_linearMode){
		// Linear Interpolator を作って終わり
		RawDataRiverPathPoint* tmpp = m_headPoint;
		// 最初のひとつは、適当でもいいから interpolator を作る
		RiverLinearInterpolator* interpolator =
			new RiverLinearInterpolator(this, getVector(tmpp->nextPoint()), getVector(tmpp->nextPoint()));
		setInterpolator(interpolator, tmpp);
		tmpp = tmpp->nextPoint();
		while (tmpp != 0){
			RawDataRiverPathPoint* nextp = tmpp->nextPoint();
			if (nextp == 0){nextp = tmpp;}
			RiverLinearInterpolator* interpolator =
			    new RiverLinearInterpolator(this, getVector(tmpp), getVector(nextp));
			setInterpolator(interpolator, tmpp);
			tmpp = tmpp->nextPoint();
		}
	}else{
		// まず、区間の数と、各区間の長さを調べる。
		int num = 0;
		RawDataRiverPathPoint* tmpp = m_headPoint;
		if (tmpp != 0){tmpp = tmpp->nextPoint();}
		while (tmpp != 0 && tmpp->nextPoint() != 0){
			++num;
			tmpp = tmpp->nextPoint();
		}
		// 区間の数が、num に格納された。
		m_XA.insert(0, num + 1, 0);
		m_XB.insert(0, num, 0);
		m_XC.insert(0, num + 1, 0);
		m_XD.insert(0, num, 0);
		m_YA.insert(0, num + 1, 0);
		m_YB.insert(0, num, 0);
		m_YC.insert(0, num + 1, 0);
		m_YD.insert(0, num, 0);
		m_Dist.insert(0, num, 0);
		tmpp = m_headPoint;
		if (tmpp != 0){tmpp = tmpp->nextPoint();}
		int i = 0;
		while (tmpp != 0 && tmpp->nextPoint() != 0){
			// Distvectorに、区間の長さを入れる。
			m_Dist[i] = (getVector(tmpp->nextPoint()) - getVector(tmpp)).length();
			// m_XA に 座標の x 座標をコピー
			m_XA[i] = getVector(tmpp).x();
			// m_YA に 座標の y 座標をコピー
			m_YA[i] = getVector(tmpp).y();
			++i;
			tmpp = tmpp->nextPoint();
		}
		if (tmpp != 0){
			// m_XA に 座標の x 座標をコピー
			m_XA[i] = getVector(tmpp).x();
			// m_YA に 座標の y 座標をコピー
			m_YA[i] = getVector(tmpp).y();
		}
		std::vector<double> alfax, alfay, lx, ly, mx, my, zx, zy;
		alfax.assign(num, 0);
		alfay.assign(num, 0);
		lx.assign(num + 1, 0);
		ly.assign(num + 1, 0);
		mx.assign(num, 0);
		my.assign(num, 0);
		zx.assign(num + 1, 0);
		zy.assign(num + 1, 0);
		for (i = 1; i < num; ++i){
			alfax[i] =
			    3. / m_Dist[i]   * (m_XA[i + 1] - m_XA[i]) -
			    3. / m_Dist[i - 1] * (m_XA[i] - m_XA[i - 1]);
			alfay[i] =
			    3. / m_Dist[i]   * (m_YA[i + 1] - m_YA[i]) -
			    3. / m_Dist[i - 1] * (m_YA[i] - m_YA[i - 1]);
		}
		if (num > 0){
			lx[0] = 1;
			ly[0] = 1;
			mx[0] = 0;
			my[0] = 0;
			zx[0] = 0;
			zy[0] = 0;
		}
		for (i = 1; i < num; ++i){
			lx[i] = 2 * (m_Dist[i - 1] + m_Dist[i]) - m_Dist[i - 1] * mx[i - 1];
			ly[i] = 2 * (m_Dist[i - 1] + m_Dist[i]) - m_Dist[i - 1] * my[i - 1];
			mx[i] = m_Dist[i] / lx[i];
			my[i] = m_Dist[i] / ly[i];
			zx[i] = (alfax[i] - m_Dist[i - 1] * zx[i - 1]) / lx[i];
			zy[i] = (alfay[i] - m_Dist[i - 1] * zy[i - 1]) / ly[i];
		}
		if (num >= 0){
			lx[num] = 1;
			ly[num] = 1;
			zx[num] = 0;
			zy[num] = 0;
			m_XC[num] = 0;
			m_YC[num] = 0;
		}
		for (i = num - 1; i >= 0; --i){
			m_XC[i] = zx[i] - mx[i] * m_XC[i + 1];
			m_YC[i] = zy[i] - my[i] * m_YC[i + 1];
			m_XB[i] =
			    (m_XA[i + 1] - m_XA[i]) / m_Dist[i] -
			    m_Dist[i] * (m_XC[i + 1] + 2 * m_XC[i]) / 3.;
			m_YB[i] =
			    (m_YA[i + 1] - m_YA[i]) / m_Dist[i] -
			    m_Dist[i] * (m_YC[i + 1] + 2 * m_YC[i]) / 3.;
			m_XD[i] = (m_XC[i + 1] - m_XC[i]) / 3. / m_Dist[i];
			m_YD[i] = (m_YC[i + 1] - m_YC[i]) / 3. / m_Dist[i];
		}
		// できたinterpolator をセットする。
		tmpp = m_headPoint;
		i = - 1;
		while (tmpp != 0){
			RiverSplineInterpolator* interpolator = new RiverSplineInterpolator(this, i);
			setInterpolator(interpolator, tmpp);
			++i;
			tmpp = tmpp->nextPoint();
		}

	}
}

QVector2D RiverSplineSolver::interpolate(int index, double d){
	d *= m_Dist[index];
	double x;
	double y;
	x = m_XA[index] +
	    m_XB[index] * d +
	    m_XC[index] * d * d +
	    m_XD[index] * d * d * d;
	y = m_YA[index] +
	    m_YB[index] * d +
	    m_YC[index] * d * d +
	    m_YD[index] * d * d * d;
	return QVector2D(x, y);
}

QVector2D RiverCenterLineSolver::getVector(RawDataRiverPathPoint* p){
	return p->position();
}

void RiverCenterLineSolver::setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p){
	if (p->riverCenter() != 0){
		delete p->riverCenter();
	}
	p->setRiverCenter(interpolator);
}

QVector2D RiverLeftBankSolver::getVector(RawDataRiverPathPoint* p){
	try{
		return p->crosssectionPosition(p->crosssection().leftBank(true).position());
	}catch (RawDataRiverCrosssection::ErrorCodes /* code */){
		return p->position();
	}
}

void RiverLeftBankSolver::setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p){
	if (p->leftBank() != 0){
		delete p->leftBank();
	}
	p->setLeftBank(interpolator);
}

QVector2D RiverRightBankSolver::getVector(RawDataRiverPathPoint* p){
	try{
		return p->crosssectionPosition(p->crosssection().rightBank(true).position());
	}catch (RawDataRiverCrosssection::ErrorCodes /* code */){
		return p->position();
	}
}

void RiverRightBankSolver::setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p){
	if (p->rightBank() != 0){
		delete p->rightBank();
	}
	p->setRightBank(interpolator);
}

QVector2D RiverGridCtrlSolver::getVector(RawDataRiverPathPoint* p){
	if (m_BankSide == bs_LeftBank){
		if (p->CenterToLeftCtrlPoints.size() == 0){return p->position();}
		double tmpdbl = p->lXSec()->interpolate(p->CenterToLeftCtrlPoints[m_Index]).position();
		return p->crosssectionPosition(tmpdbl);
	}else{
		if (p->CenterToRightCtrlPoints.size() == 0){return p->position();}
		double tmpdbl = p->rXSec()->interpolate(p->CenterToRightCtrlPoints[m_Index]).position();
		return p->crosssectionPosition(tmpdbl);
	}
}

void RiverGridCtrlSolver::setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p){
	if (m_BankSide == bs_LeftBank){
		if (p->LGridLines()[m_Index] != 0){
			delete p->LGridLines()[m_Index];
		}
		p->LGridLines()[m_Index] = interpolator;
	}else{
		if (p->RGridLines()[m_Index] != 0){
			delete p->RGridLines()[m_Index];
		}
		p->RGridLines()[m_Index] = interpolator;
	}
}

QVector2D RiverBackgroundGridCtrlSolver::getVector(RawDataRiverPathPoint* p){
	if (m_BankSide == bs_LeftBank){
		double tmpdbl = p->lXSec()->interpolate(m_Parameter).position();
		return p->crosssectionPosition(tmpdbl);
	}else{
		double tmpdbl = p->rXSec()->interpolate(m_Parameter).position();
		return p->crosssectionPosition(tmpdbl);
	}
}

void RiverBackgroundGridCtrlSolver::setInterpolator(Interpolator2D1* interpolator, RawDataRiverPathPoint* p){
	if (m_BankSide == bs_LeftBank){
		if (p->backgroundLGridLines()[m_Index] != 0){
			delete p->backgroundLGridLines()[m_Index];
		}
		p->backgroundLGridLines()[m_Index] = interpolator;
	}else{
		if (p->backgroundRGridLines()[m_Index] != 0){
			delete p->backgroundRGridLines()[m_Index];
		}
		p->backgroundRGridLines()[m_Index] = interpolator;
	}
}

Interpolator2D1* RiverSplineInterpolator::copy(){
	Interpolator2D1* copy = new RiverSplineInterpolatorCopy(
		m_parent->m_Dist[m_Index],
		m_parent->m_XA[m_Index],
		m_parent->m_XB[m_Index],
		m_parent->m_XC[m_Index],
		m_parent->m_XD[m_Index],
		m_parent->m_YA[m_Index],
		m_parent->m_YB[m_Index],
		m_parent->m_YC[m_Index],
		m_parent->m_YD[m_Index]);
	return copy;
}

void RiverSplineSolver::setLinearMode(bool linearmode, RawDataRiverPathPoint* /*head*/, bool /*noundo*/){
	m_linearMode = linearmode;
}
