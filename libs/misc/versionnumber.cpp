#include "versionnumber.h"

#include <QStringList>

const QString VersionNumber::toString()const
{
	QString ret;
	QString major; major.setNum(m_major);
	ret.append(major);
	ret.append(".");
	QString minor; minor.setNum(m_minor);
	ret.append(minor);

	if (m_fix != 0 || m_build != 0) {
		ret.append(".");
		QString fix;
		fix.setNum(m_fix);
		ret.append(fix);
	}
	if (m_build != 0) {
		ret.append(".");
		QString build;
		build.setNum(m_build);
		ret.append(build);
	}
	return ret;
}

const QString VersionNumber::toAboutString() const
{
	QString ret;
	QString major; major.setNum(m_major);
	ret.append(major);
	ret.append(".");
	QString minor; minor.setNum(m_minor);
	ret.append(minor);

	if (m_fix != 0 || m_build != 0) {
		ret.append(".");
		QString fix; fix.setNum(m_fix);
		ret.append(fix);
	}
	if (m_build != 0) {
		ret.append(QString(QObject::tr(" Revision %1")).arg(m_build));
	}
	return ret;
}

void VersionNumber::fromString(const QString& version)
{
	init();
	QStringList versions = version.split(".");
	m_major = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	m_minor = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	m_fix = versions.front().toUInt();
	versions.pop_front();
	if (versions.empty()) {return;}

	m_build = versions.front().toUInt();
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
		(m_major == v.m_major) &&
		(m_minor == v.m_minor) &&
		(m_fix == v.m_fix) &&
		(m_build == v.m_build);
}
