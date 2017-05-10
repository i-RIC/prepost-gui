#ifndef POINTSPREPROCESSORCONTROLLER_IMPL_H
#define POINTSPREPROCESSORCONTROLLER_IMPL_H

#include "../pointspreprocessorcontroller.h"

class QAction;

class PointsPreProcessorController::Impl
{
public:
	Impl();
	~Impl();

	QAction* m_importAction;
	QAction* m_deleteAction;
};

#endif // POINTSPREPROCESSORCONTROLLER_IMPL_H
