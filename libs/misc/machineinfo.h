#ifndef MACHINEINFO_H
#define MACHINEINFO_H

#include "misc_global.h"

#include <QString>

/// A snapshot of the machine iRIC is running on, for best-effort telemetry.
///
/// Every field is a free-form display string (never normalized to a number or
/// unit). An empty field means "could not read it" and is treated by the hub
/// as "not sent". No value here identifies the machine or its user: host name,
/// Windows user name, serial numbers, MAC / IP addresses and file paths are
/// deliberately excluded.
struct MISCDLL_EXPORT MachineInfo
{
	QString osName;      ///< e.g. "Microsoft Windows 11 Pro"
	QString osVersion;   ///< e.g. "10.0.26100 (24H2)"
	QString cpuName;     ///< e.g. "13th Gen Intel(R) Core(TM) i7-13700"
	QString gpuName;     ///< primary adapter, e.g. "NVIDIA GeForce RTX 4060"
	QString memorySize;  ///< total physical RAM, e.g. "32 GB"
};

/// Collect the current machine description. Returns empty fields for anything
/// that cannot be read; never throws. On non-Windows builds every field is
/// empty.
MISCDLL_EXPORT MachineInfo collectMachineInfo();

#endif // MACHINEINFO_H
