#ifndef DISTANCEMEASUREDATAITEM_IMPL_H
#define DISTANCEMEASUREDATAITEM_IMPL_H

#include "../distancemeasuredataitem.h"

#include <guibase/vtktool/vtklabel2dactor.h>
#include <guibase/vtktool/vtklineactor.h>

#include <QCursor>
#include <QPixmap>

class QAction;

class DistanceMeasureDataItem::Impl
{
public:
	enum MouseEventMode {
		meNormal,
		meBeforeDefining,
		meDefining,
		meTranslate,
		meTranslatePrepare,
		meMoveVertex,
		meMoveVertexPrepare
	};

	Impl(DistanceMeasureDataItem* parent);
	~Impl();

	int m_dragPointTarget;
	QPoint m_dragStartPoint;
	QPointF m_dragStartPoint1;
	QPointF m_dragStartPoint2;

	MouseEventMode m_mouseEventMode;
	bool m_shiftPressed;

	QPixmap m_movePointPixmap;
	QCursor m_movePointCursor;

	QMenu* m_rightClickingMenu;
	QAction* m_redefineAction;
	QAction* m_propertyAction;

	vtkLabel2DActor m_labelActor;
	vtkLineActor m_lineActor;

	DistanceMeasureSetting m_setting;
};

#endif // DISTANCEMEASUREDATAITEM_IMPL_H
