#include "versionnumber.h"

#include <QObject>
#include <QStringList>

class VersionNumber::Impl
{
public:
	unsigned int m_major {};
	unsigned int m_minor {};
	unsigned int m_fix {};
	unsigned int m_build {};
};

VersionNumber::VersionNumber() :
	m_impl {new Impl {}}
{}


VersionNumber::VersionNumber(unsigned int major, unsigned int minor, unsigned int fix, unsigned int build) :
	VersionNumber()
{
	m_impl->m_major = major;
	m_impl->m_minor = minor;
	m_impl->m_fix = fix;
	m_impl->m_build = build;
}

VersionNumber::VersionNumber(const QString& version) :
	VersionNumber()
{
	QStringList versions = version.split(".");
	m_impl->m_major = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	m_impl->m_minor = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	m_impl->m_fix = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	m_impl->m_build = versions.front().toUInt();
}

VersionNumber::VersionNumber(const VersionNumber& v) :
	VersionNumber()
{
	*(m_impl) = *(v.m_impl);
}

VersionNumber::VersionNumber(VersionNumber&& v) :
	m_impl {v.m_impl}
{
	v.m_impl = nullptr;
}

VersionNumber::~VersionNumber()
{
	delete m_impl;
}

VersionNumber& VersionNumber::operator=(const VersionNumber& v)
{
	*(m_impl) = *(v.m_impl);
	return *this;
}

VersionNumber& VersionNumber::operator=(VersionNumber&& v)
{
	delete m_impl;
	m_impl = v.m_impl;
	v.m_impl = nullptr;
	return *this;
}

unsigned int VersionNumber::major() const
{
	return m_impl->m_major;
}

unsigned int VersionNumber::minor() const
{
	return m_impl->m_minor;
}

unsigned int VersionNumber::fix() const
{
	return m_impl->m_fix;
}

unsigned int VersionNumber::build() const
{
	return m_impl->m_build;
}

void VersionNumber::setMajor(unsigned int major)
{
	m_impl->m_major = major;
}

void VersionNumber::setMinor(unsigned int minor)
{
	m_impl->m_minor = minor;
}

void VersionNumber::setFix(unsigned int fix)
{
	m_impl->m_fix = fix;
}

void VersionNumber::setBuild(unsigned int build)
{
	m_impl->m_build = build;
}

QString VersionNumber::toString() const
{
	QString ret;
	QString major; major.setNum(m_impl->m_major);
	ret.append(major);
	ret.append(".");
	QString minor; minor.setNum(m_impl->m_minor);
	ret.append(minor);

	if (m_impl->m_fix != 0 || m_impl->m_build != 0) {
		ret.append(".");
		QString fix;
		fix.setNum(m_impl->m_fix);
		ret.append(fix);
	}
	if (m_impl->m_build != 0) {
		ret.append(".");
		QString build;
		build.setNum(m_impl->m_build);
		ret.append(build);
	}
	return ret;
}

QString VersionNumber::toAboutString() const
{
	QString ret;
	QString major; major.setNum(m_impl->m_major);
	ret.append(major);
	ret.append(".");
	QString minor; minor.setNum(m_impl->m_minor);
	ret.append(minor);

	if (m_impl->m_fix != 0 || m_impl->m_build != 0) {
		ret.append(".");
		QString fix; fix.setNum(m_impl->m_fix);
		ret.append(fix);
	}
	if (m_impl->m_build != 0) {
		ret.append(QString(QObject::tr(" Revision %1")).arg(m_impl->m_build));
	}
	return ret;
}

bool VersionNumber::compatibleWith(const VersionNumber& v)const
{
	if (major() != v.major()) {return false;}
	if (minor() < v.minor()) {return false;}
	return true;
}

bool VersionNumber::operator ==(const VersionNumber& v) const
{
	return
		(m_impl->m_major == v.m_impl->m_major) &&
		(m_impl->m_minor == v.m_impl->m_minor) &&
		(m_impl->m_fix == v.m_impl->m_fix) &&
		(m_impl->m_build == v.m_impl->m_build);
}
