#ifndef BASELINEPREPROCESSORCONTROLLER_IMPL_H
#define BASELINEPREPROCESSORCONTROLLER_IMPL_H

#include "../baselinepreprocessorcontroller.h"

class QAction;

class BaseLinePreProcessorController::Impl
{
public:
	enum class Mode {
		Normal,
		BeforeDefining,
		Defining,
		MovePoint,
		MovePointPrepare,
		AddPoint,
		AddPointPrepare,
		AddPointNotPossible,
		RemovePointPrepare,
		RemovePointNotPossible,
	};
	Impl();
	~Impl();

	Mode m_mode;
	int m_movingPointIndex;
	int m_addingEdgeIndex;
	int m_removingPointIndex;

	QAction* m_importAction;
	QAction* m_exportAction;

	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_editCoordinatesAction;
	QAction* m_reverseDirectionAction;
	QAction* m_deleteAction;

	QPixmap m_addPixmap;
	QPixmap m_removePixmap;

	QCursor m_addCursor;
	QCursor m_removeCursor;
};

#endif // BASELINEPREPROCESSORCONTROLLER_IMPL_H
