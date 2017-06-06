#ifndef WATERSURFACEELEVATIONPOINTS_IMPL
#define WATERSURFACEELEVATIONPOINTS_IMPL

#include "../watersurfaceelevationpoints.h"
#include "../../arbitraryhwm/arbitraryhwm.h"
#include "../../benchmark/benchmark.h"
#include "../../hub/hub.h"
#include "../../leftbankhwm/leftbankhwm.h"
#include "../../referencemark/referencemark.h"
#include "../../rightbankhwm/rightbankhwm.h"
#include "../../streamgage/streamgage.h"

class WaterSurfaceElevationPoints::Impl
{
public:
	Impl(WaterSurfaceElevationPoints* parent);

	ArbitraryHWM m_arbitraryHWM;
	Benchmark m_benchmark;
	ReferenceMark m_referenceMark;
	Hub m_hub;
	LeftBankHWM m_leftBankHWM;
	RightBankHWM m_rightBankHWM;
	StreamGage m_streamGage;
};

#endif // WATERSURFACEELEVATIONPOINTS_IMPL
