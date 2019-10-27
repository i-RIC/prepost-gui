#include "geodatariversurvey_impl.h"

GeoDataRiverSurvey::Impl::Impl(GeoDataRiverSurvey* rs) :
	m_mouseEventMode {meNormal},
	m_rs {rs}
{}

GeoDataRiverSurvey::Impl::~Impl()
{}

void GeoDataRiverSurvey::Impl::setupCursors()
{
	m_pixmapMove = QPixmap(":/libs/guibase/images/cursorItemMove.png");
	m_cursorMove = QCursor(m_pixmapMove, 7, 2);

	m_pixmapRotate = QPixmap(":/libs/geodata/riversurvey/images/cursorCrosssectionRotate.png");
	m_cursorRotate = QCursor(m_pixmapRotate, 7, 2);

	m_pixmapExpand = QPixmap(":/libs/geodata/riversurvey/images/cursorExpand.png");
	m_cursorExpand = QCursor(m_pixmapExpand, 7, 2);

	m_pixmapShift = QPixmap(":/libs/geodata/riversurvey/images/cursorShiftCenter.png");
	m_cursorShift = QCursor(m_pixmapShift, 7, 2);
}
