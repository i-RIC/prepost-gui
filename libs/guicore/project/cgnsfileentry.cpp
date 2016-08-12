#include "cgnsfileentry.h"
#include "private/cgnsfileentry_impl.h"

#include <QDomElement>
#include <QDomNode>
#include <QXmlStreamWriter>

CgnsFileEntry::Impl::Impl(const QString& filename) :
	m_filename {filename}
{}

// public interface

CgnsFileEntry::CgnsFileEntry(ProjectDataItem* parent) :
	CgnsFileEntry("", parent)
{}

CgnsFileEntry::CgnsFileEntry(const QString& filename, ProjectDataItem* parent) :
	ProjectDataItem {parent},
	impl {new Impl(filename)}
{}

CgnsFileEntry::~CgnsFileEntry()
{
	delete impl;
}

QString CgnsFileEntry::filename() const
{
	return impl->m_filename;
}

QString CgnsFileEntry::comment() const
{
	return impl->m_comment;
}

void CgnsFileEntry::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement element = node.toElement();
	impl->m_filename = element.attribute("filename");
	impl->m_comment = element.attribute("comment");
}

void CgnsFileEntry::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("filename", impl->m_filename);
	writer.writeAttribute("comment", impl->m_comment);
}
