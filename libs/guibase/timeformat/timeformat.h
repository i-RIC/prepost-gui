#ifndef TIMEFORMAT_H
#define TIMEFORMAT_H

enum class TimeFormat {
	elapsed_SS_sec      = 1,
	elapsed_MM_SS       = 2,
	elapsed_HH_MM_SS    = 3,
	elapsed_DD_HH_MM_SS = 4,

	actual_yyyy_mm_dd_HH_MM_SS = 101,
	actual_yyyy_mm_dd_HH_MM    = 102,
	actual_mm_dd_HH_MM_SS      = 103,
	actual_mm_dd_HH_MM         = 104,
	actual_HH_MM_SS            = 105,
	actual_HH_MM               = 106,
	actual_Custom              = 107
};

#endif // TIMEFORMAT_H
