#ifndef WATERSURFACEELEVATIONPOINTS_IMPL
#define WATERSURFACEELEVATIONPOINTS_IMPL

#include "../watersurfaceelevationpoints.h"
#include "../../arbitraryhwm/arbitraryhwm.h"
#include "../../leftbankhwm/leftbankhwm.h"
#include "../../rightbankhwm/rightbankhwm.h"

class WaterSurfaceElevationPoints::Impl
{
public:
	Impl(WaterSurfaceElevationPoints* parent);

	ArbitraryHWM m_arbitraryHWM;
	LeftBankHWM m_leftBankHWM;
	RightBankHWM m_rightBankHWM;
};

#endif // WATERSURFACEELEVATIONPOINTS_IMPL
