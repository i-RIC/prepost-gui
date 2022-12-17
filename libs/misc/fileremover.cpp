#include "fileremover.h"

#include <QFile>

FileRemover::FileRemover(const QString& fileName) :
	m_fileName {fileName}
{}

FileRemover::~FileRemover()
{
	QFile::remove(m_fileName);
}
