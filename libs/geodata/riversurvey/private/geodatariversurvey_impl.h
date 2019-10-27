#ifndef GEODATARIVERSURVEY_IMPL_H
#define GEODATARIVERSURVEY_IMPL_H

#include "../geodatariversurvey.h"

#include <QCursor>
#include <QPixmap>

class GeoDataRiverSurvey::Impl
{
public:
	enum MouseEventMode {
		meNormal,
		meTranslate,
		meTranslatePrepare,
		meRotateRight,
		meRotatePrepareRight,
		meRotateLeft,
		meRotatePrepareLeft,
		meShift,
		meShiftPrepare,
		meMoveExtentionEndPointLeft,
		meMoveExtensionEndPointPrepareLeft,
		meMoveExtentionEndPointRight,
		meMoveExtensionEndPointPrepareRight,
		meExpansionRight,
		meExpansionPrepareRight,
		meExpansionLeft,
		meExpansionPrepareLeft,
		meAddingExtension,
		meInserting,

		meTranslateDialog,
		meRotateDialog,
		meShiftDialog,
		meExpansionDialog
	};

	Impl(GeoDataRiverSurvey* rs);
	~Impl();

	void setupCursors();

	MouseEventMode m_mouseEventMode;

	QPixmap m_pixmapMove;
	QPixmap m_pixmapRotate;
	QPixmap m_pixmapExpand;
	QPixmap m_pixmapShift;

	QCursor m_cursorMove;
	QCursor m_cursorRotate;
	QCursor m_cursorExpand;
	QCursor m_cursorShift;

	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_IMPL_H
