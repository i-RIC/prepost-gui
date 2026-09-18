#ifndef AUTHCREDENTIALSTORE_H
#define AUTHCREDENTIALSTORE_H

#include "misc_global.h"

class QString;

/// Persists the iRIC ID OAuth2 refresh token.
///
/// On Windows the token is stored in the Windows Credential Manager
/// (target name "iRIC/id.i-ric.org/refresh_token"), DPAPI-encrypted and
/// readable only by the same Windows user. On other platforms the functions
/// are no-op stubs so the library keeps building.
namespace iRICAuth
{

/// Save the refresh token. Returns true on success.
bool MISCDLL_EXPORT saveRefreshToken(const QString& token);

/// Load the stored refresh token. Returns an empty string when nothing is stored.
QString MISCDLL_EXPORT loadRefreshToken();

/// Delete the stored refresh token (called on logout). Returns true on success.
bool MISCDLL_EXPORT removeRefreshToken();

} // namespace iRICAuth

#endif // AUTHCREDENTIALSTORE_H
