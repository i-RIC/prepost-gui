#include "authcredentialstore.h"

#include <QString>

#include <string>

#ifdef Q_OS_WIN

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wincred.h>

namespace {

// This name appears in the Windows Credential Manager list.
const wchar_t* TARGET_NAME = L"iRIC/id.i-ric.org/refresh_token";

} // namespace

bool iRICAuth::saveRefreshToken(const QString& token)
{
	const std::wstring value = token.toStdWString();

	CREDENTIALW cred = {};
	cred.Type = CRED_TYPE_GENERIC;
	cred.TargetName = const_cast<wchar_t*>(TARGET_NAME);
	cred.CredentialBlobSize = static_cast<DWORD>(value.size() * sizeof(wchar_t));
	cred.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(value.c_str()));
	cred.Persist = CRED_PERSIST_LOCAL_MACHINE;

	return CredWriteW(&cred, 0) == TRUE;
}

QString iRICAuth::loadRefreshToken()
{
	PCREDENTIALW cred = nullptr;
	if (CredReadW(TARGET_NAME, CRED_TYPE_GENERIC, 0, &cred) != TRUE) {
		return QString();
	}
	const QString token = QString::fromWCharArray(
				reinterpret_cast<const wchar_t*>(cred->CredentialBlob),
				cred->CredentialBlobSize / sizeof(wchar_t));
	CredFree(cred);
	return token;
}

bool iRICAuth::removeRefreshToken()
{
	return CredDeleteW(TARGET_NAME, CRED_TYPE_GENERIC, 0) == TRUE;
}

#else // Q_OS_WIN

bool iRICAuth::saveRefreshToken(const QString& /*token*/)
{
	return false;
}

QString iRICAuth::loadRefreshToken()
{
	return QString();
}

bool iRICAuth::removeRefreshToken()
{
	return false;
}

#endif // Q_OS_WIN
