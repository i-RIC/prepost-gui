#ifndef VERSIONNUMBER_H
#define VERSIONNUMBER_H

#include "misc_global.h"
#include <QString>

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
	/// Constructor
	VersionNumber() {
		init();
	}
	/// Constructor (with initialization)
	VersionNumber(unsigned int major, unsigned int minor = 0, unsigned int fix = 0, unsigned int build = 0) {
		init();
		m_major = major;
		m_minor = minor;
		m_fix = fix;
		m_build = build;
	}
	/// Constructor (with initialization with string version number)
	VersionNumber(const QString& version) {
		fromString(version);
	}
	/// Major version number
	unsigned int major()const {return m_major;}
	/// Minor version number
	unsigned int minor()const {return m_minor;}
	/// Fix version number
	unsigned int fix()const {return m_fix;}
	/// Build number
	unsigned int build()const {return m_build;}
	/// Set major version number
	void setMajor(unsigned int major) {m_major = major;}
	/// Set minor version number
	void setMinor(unsigned int minor) {m_minor = minor;}
	/// Set fix version number
	void setFix(unsigned int fix) {m_fix = fix;}
	/// Set build number
	void setBuild(unsigned int build) {m_build = build;}
	/// Full version number as string
	const QString toString()const;
	/// Version number as string (for about dialog)
	const QString toAboutString() const;
	/// Set version number as string
	void fromString(const QString& version);
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
	bool operator !=(const VersionNumber& v) const {
		return !(operator==(v));
	}

private:
	void init();
	unsigned int m_major;
	unsigned int m_minor;
	unsigned int m_fix;
	unsigned int m_build;
};

#endif // VERSIONNUMBER_H
