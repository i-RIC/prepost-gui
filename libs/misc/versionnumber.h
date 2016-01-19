#ifndef VERSIONNUMBER_H
#define VERSIONNUMBER_H

#include "misc_global.h"

class QString;

/// This class contains version numbers of solvers
/**
 * Version number of solvers are defined using the following numbers:
 *
 * - Major version
 * - Minor version
 * - Fix version
 * - Build number
 *
 * Major version is the only required number, and the others are optional.
 *
 * This class is used for the following objectives:
 * - To make it possible to handle version numbers, that consists of several integers,
 *   easily with a single object
 * - To evaluate the compatibility easily
 * - To convert version numbers from/to string.
 *
 * @sa @ref vnum
 */
class MISCDLL_EXPORT VersionNumber
{

public:
	VersionNumber();
	VersionNumber(unsigned int major, unsigned int minor = 0, unsigned int fix = 0, unsigned int build = 0);
	VersionNumber(const QString& version);
	VersionNumber(const VersionNumber& v);
	VersionNumber(VersionNumber&& v);
	~VersionNumber();

	VersionNumber& operator=(const VersionNumber& v);
	VersionNumber& operator=(VersionNumber&& v);

	/// Major version number
	unsigned int major() const;
	/// Minor version number
	unsigned int minor() const;
	/// Fix version number
	unsigned int fix() const;
	/// Build number
	unsigned int build() const;
	/// Set major version number
	void setMajor(unsigned int major);
	/// Set minor version number
	void setMinor(unsigned int minor);
	/// Set fix version number
	void setFix(unsigned int fix);
	/// Set build number
	void setBuild(unsigned int build);
	/// Full version number as string
	QString toString() const;
	/// Version number as string (for about dialog)
	QString toAboutString() const;
	/// Return whether the target version is compatible with this version.
	/**
	 * Compatibility is defined in the following logic:
	 * - If the major version is different, it is not compatible with
	 *   the target version.
	 * - if the minor version is smaller than the target version, it is
	 *   not compatible with that.
	 */
	bool compatibleWith(const VersionNumber& v) const;
	bool operator ==(const VersionNumber& v) const;
	bool operator !=(const VersionNumber& v) const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/versionnumber_impl.h"
#endif // _DEBUG

#endif // VERSIONNUMBER_H
