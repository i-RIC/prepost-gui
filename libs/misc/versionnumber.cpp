#include "versionnumber.h"
#include "private/versionnumber_impl.h"

#include <QObject>
#include <QString>
#include <QStringList>

VersionNumber::VersionNumber() :
	impl {new Impl {}}
{}

VersionNumber::VersionNumber(unsigned int major, unsigned int minor, unsigned int fix, unsigned int build) :
	VersionNumber()
{
	impl->m_major = major;
	impl->m_minor = minor;
	impl->m_fix = fix;
	impl->m_build = build;
}

VersionNumber::VersionNumber(const QString& version) :
	VersionNumber()
{
	QStringList versions = version.split(".");
	impl->m_major = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	impl->m_minor = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	impl->m_fix = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	impl->m_build = versions.front().toUInt();
}

VersionNumber::VersionNumber(const VersionNumber& v) :
	VersionNumber()
{
	*(impl) = *(v.impl);
}

VersionNumber::VersionNumber(VersionNumber&& v) :
	impl {v.impl}
{
	v.impl = nullptr;
}

VersionNumber::~VersionNumber()
{
	delete impl;
}

VersionNumber& VersionNumber::operator=(const VersionNumber& v)
{
	*(impl) = *(v.impl);
	return *this;
}

VersionNumber& VersionNumber::operator=(VersionNumber&& v)
{
	delete impl;
	impl = v.impl;
	v.impl = nullptr;
	return *this;
}

unsigned int VersionNumber::major() const
{
	return impl->m_major;
}

unsigned int VersionNumber::minor() const
{
	return impl->m_minor;
}

unsigned int VersionNumber::fix() const
{
	return impl->m_fix;
}

unsigned int VersionNumber::build() const
{
	return impl->m_build;
}

void VersionNumber::setMajor(unsigned int major)
{
	impl->m_major = major;
}

void VersionNumber::setMinor(unsigned int minor)
{
	impl->m_minor = minor;
}

void VersionNumber::setFix(unsigned int fix)
{
	impl->m_fix = fix;
}

void VersionNumber::setBuild(unsigned int build)
{
	impl->m_build = build;
}

QString VersionNumber::toString() const
{
	QString ret;
	QString major; major.setNum(impl->m_major);
	ret.append(major);
	ret.append(".");
	QString minor; minor.setNum(impl->m_minor);
	ret.append(minor);

	if (impl->m_fix != 0 || impl->m_build != 0) {
		ret.append(".");
		QString fix;
		fix.setNum(impl->m_fix);
		ret.append(fix);
	}
	if (impl->m_build != 0) {
		ret.append(".");
		QString build;
		build.setNum(impl->m_build);
		ret.append(build);
	}
	return ret;
}

QString VersionNumber::toAboutString() const
{
	QString ret;
	QString major; major.setNum(impl->m_major);
	ret.append(major);
	ret.append(".");
	QString minor; minor.setNum(impl->m_minor);
	ret.append(minor);

	if (impl->m_fix != 0 || impl->m_build != 0) {
		ret.append(".");
		QString fix; fix.setNum(impl->m_fix);
		ret.append(fix);
	}
	if (impl->m_build != 0) {
		ret.append(QString(QObject::tr(" Revision %1")).arg(impl->m_build));
	}
	return ret;
}

bool VersionNumber::compatibleWith(const VersionNumber& v)const
{
	if (major() < v.major()) {return false;}
	if (minor() < v.minor()) {return false;}
	return true;
}

bool VersionNumber::operator ==(const VersionNumber& v) const
{
	return
		(impl->m_major == v.impl->m_major) &&
		(impl->m_minor == v.impl->m_minor) &&
		(impl->m_fix == v.impl->m_fix) &&
		(impl->m_build == v.impl->m_build);
}

bool VersionNumber::operator !=(const VersionNumber& v) const
{
	return !(operator==(v));
}
