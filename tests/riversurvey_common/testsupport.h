#ifndef RIVERSURVEY_TESTSUPPORT_H
#define RIVERSURVEY_TESTSUPPORT_H

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <windows.h>
	#include <crtdbg.h>
#endif

namespace RiverSurveyTestSupport {

/// Makes assertion failures and crashes end the process immediately with a message on stderr,
/// instead of showing modal dialogs that block automated test runs.
inline void disableCrashDialogs()
{
#ifdef _WIN32
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#ifdef _DEBUG
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif
#endif
}

} // namespace RiverSurveyTestSupport

#endif // RIVERSURVEY_TESTSUPPORT_H
