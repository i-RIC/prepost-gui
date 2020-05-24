#ifndef GEODATAPOINT_IMPL_H
#define GEODATAPOINT_IMPL_H

#include "../geodatapoint.h"

#include <guibase/point/pointcontroller.h>

class GeoDataPoint::Impl
{
public:
	Impl(GeoDataPoint* parent);
	~Impl();

	GeoDataPoint* m_parent;
	PointController m_pointController;

	GeoDataPointColorSimpleSettingDialog::Setting m_setting;
	std::vector<QVariant> m_variantValues;

	MouseEventMode m_mouseEventMode;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	QMenu* m_rightClickingMenu;

	QAction* m_editNameAction;
	QAction* m_editValueAction;
	QAction* m_editNameAndValueAction;
	QAction* m_coordEditAction;
	QAction* m_editColorSettingAction;
};

#endif // GEODATAPOINT_IMPL_H
