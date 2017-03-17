#ifndef BASELINEPREPROCESSORCONTROLLER_IMPL_H
#define BASELINEPREPROCESSORCONTROLLER_IMPL_H

#include "../baselinepreprocessorcontroller.h"

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

	Mode m_mode;
	int m_movingPointIndex;
};

#endif // BASELINEPREPROCESSORCONTROLLER_IMPL_H
