#ifndef GEODATAPOLYLINE_IMPL_H
#define GEODATAPOLYLINE_IMPL_H

#include "../geodatapolyline.h"

#include <misc/zdepthrange.h>

class GeoDataPolyLine::Impl
{
public:
	Impl(GeoDataPolyLine* parent);
	~Impl();

	GeoDataPolyLine* m_parent;

	GeoDataPolyLineImplPolyLine* m_polyLine;

	GeoDataPolyLineColorSimpleSettingDialog::Setting m_setting;
	std::vector<QVariant> m_variantValues;

	ZDepthRange m_depthRange;

	bool m_inhibitSelect;

	MouseEventMode m_mouseEventMode;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	QMenu* m_rightClickingMenu;

	QAction* m_editValueAction;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_editColorSettingAction;

	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QPixmap m_movePointPixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;
	QCursor m_movePointCursor;
};

#endif // GEODATAPOLYLINE_IMPL_H
