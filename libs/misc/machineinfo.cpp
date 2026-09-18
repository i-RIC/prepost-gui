#include "machineinfo.h"

#include <QSettings>
#include <QString>

#ifdef Q_OS_WIN

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace {

// HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion
const char* CURRENT_VERSION_KEY =
	"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

// Read major.minor.build straight from the kernel (RtlGetVersion is not subject
// to the manifest-based version lie that GetVersionEx suffers from).
void kernelVersion(DWORD* major, DWORD* minor, DWORD* build)
{
	*major = 0; *minor = 0; *build = 0;

	OSVERSIONINFOEXW info = {};
	info.dwOSVersionInfoSize = sizeof(info);

	HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
	if (ntdll == nullptr) {return;}

	typedef LONG (WINAPI* RtlGetVersionFn)(LPOSVERSIONINFOEXW);
	auto fn = reinterpret_cast<RtlGetVersionFn>(GetProcAddress(ntdll, "RtlGetVersion"));
	if (fn == nullptr) {return;}

	if (fn(&info) == 0) {
		*major = info.dwMajorVersion;
		*minor = info.dwMinorVersion;
		*build = info.dwBuildNumber;
	}
}

QString osName(DWORD build)
{
	QSettings reg(QString::fromLatin1(CURRENT_VERSION_KEY), QSettings::NativeFormat);
	QString name = reg.value(QStringLiteral("ProductName")).toString().trimmed();
	if (name.isEmpty()) {
		name = QStringLiteral("Windows");
	}
	// Windows 11 still reports "Windows 10 ..." in ProductName; correct it from
	// the build number (11 starts at build 22000).
	if (build >= 22000 && name.contains(QStringLiteral("Windows 10"))) {
		name.replace(QStringLiteral("Windows 10"), QStringLiteral("Windows 11"));
	}
	if (! name.startsWith(QStringLiteral("Microsoft"), Qt::CaseInsensitive)) {
		name = QStringLiteral("Microsoft ") + name;
	}
	return name;
}

QString osVersion(DWORD major, DWORD minor, DWORD build)
{
	QString ver = QStringLiteral("%1.%2.%3").arg(major).arg(minor).arg(build);

	QSettings reg(QString::fromLatin1(CURRENT_VERSION_KEY), QSettings::NativeFormat);
	QString display = reg.value(QStringLiteral("DisplayVersion")).toString().trimmed();
	if (display.isEmpty()) {
		display = reg.value(QStringLiteral("ReleaseId")).toString().trimmed();
	}
	if (! display.isEmpty()) {
		ver += QStringLiteral(" (%1)").arg(display);
	}
	return ver;
}

QString cpuName()
{
	QSettings reg(
		QStringLiteral("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
		QSettings::NativeFormat);
	return reg.value(QStringLiteral("ProcessorNameString")).toString().trimmed();
}

QString gpuName()
{
	QString firstFound;
	DISPLAY_DEVICEW dd = {};
	dd.cb = sizeof(dd);

	for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &dd, 0) != FALSE; ++i) {
		const QString name = QString::fromWCharArray(dd.DeviceString).trimmed();
		if (name.isEmpty()) {
			dd.cb = sizeof(dd);
			continue;
		}
		if ((dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0) {
			return name;
		}
		if (firstFound.isEmpty()) {
			firstFound = name;
		}
		dd.cb = sizeof(dd);
	}
	return firstFound;
}

QString memorySize()
{
	MEMORYSTATUSEX statex = {};
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex) == FALSE) {
		return QString();
	}
	const double gib = static_cast<double>(statex.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
	return QStringLiteral("%1 GB").arg(qRound64(gib));
}

} // namespace

MachineInfo collectMachineInfo()
{
	DWORD major = 0, minor = 0, build = 0;
	kernelVersion(&major, &minor, &build);

	MachineInfo info;
	info.osName = osName(build);
	info.osVersion = osVersion(major, minor, build);
	info.cpuName = cpuName();
	info.gpuName = gpuName();
	info.memorySize = memorySize();
	return info;
}

#else // Q_OS_WIN

MachineInfo collectMachineInfo()
{
	return MachineInfo();
}

#endif // Q_OS_WIN
