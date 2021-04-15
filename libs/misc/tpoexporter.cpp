#include "tpoexporter.h"

#include <QFile>
#include <QTextStream>

TpoExporter::TpoExporter(QWidget* w) :
	m_file {nullptr},
	m_stream {nullptr},
	m_widget{w}
{}

TpoExporter::~TpoExporter()
{}

bool TpoExporter::open(const QString& fileName)
{
	m_file = new QFile(fileName);
	bool ok = m_file->open(QFile::WriteOnly | QFile::Text);
	if (! ok) {
		delete m_file;
		m_file = nullptr;
		return false;
	}

	m_stream = new QTextStream(m_file);
	m_stream->setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);
	m_stream->setRealNumberPrecision(3);

	return true;
}

void TpoExporter::close()
{
	if (m_file == nullptr) {return;}

	m_file->close();
}

void TpoExporter::setOffset(double x, double y)
{
	setOffset(QPointF(x, y));
}

void TpoExporter::setOffset(const QPointF& offset)
{
	m_offset = offset;
}
