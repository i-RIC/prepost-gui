#include "tpoexporter.h"
#include "private/tpoexporter_impl.h"
#include "private/tpoexporter_settingdialog.h"

#include <QFile>
#include <QTextStream>

TpoExporter::Impl::Impl() :
	m_file {nullptr},
	m_stream {nullptr},
	m_xPrecision {6},
	m_yPrecision {6},
	m_zPrecision {8}
{}

TpoExporter::Impl::~Impl()
{
	delete m_file;
	delete m_stream;
}

TpoExporter::TpoExporter(QWidget* w) :
	impl {new Impl()}
{
	impl->m_widget = w;
}

TpoExporter::~TpoExporter()
{
	delete impl;
}

bool TpoExporter::open(const QString& fileName)
{
	impl->m_file = new QFile(fileName);
	bool ok = impl->m_file->open(QFile::WriteOnly | QFile::Text);
	if (! ok) {
		delete impl->m_file;
		impl->m_file = nullptr;
		return false;
	}

	impl->m_stream = new QTextStream(impl->m_file);
	impl->m_stream->setRealNumberNotation(QTextStream::RealNumberNotation::FixedNotation);

	return true;
}

bool TpoExporter::setup()
{
	SettingDialog dialog(impl->m_widget);
	int ret = dialog.exec();
	if (ret != QDialog::Accepted) {
		return false;
	}

	impl->m_xPrecision = dialog.xPrecision();
	impl->m_yPrecision = dialog.yPrecision();
	impl->m_zPrecision = dialog.zPrecision();

	return true;
}

void TpoExporter::close()
{
	if (impl->m_file == nullptr) {return;}

	impl->m_file->close();
}

void TpoExporter::addPoint(double x, double y, double z)
{
	if (! impl->m_stream) {return;}

	impl->m_stream->setRealNumberPrecision(impl->m_xPrecision);
	*(impl->m_stream) << x + impl->m_offset.x() << " ";

	impl->m_stream->setRealNumberPrecision(impl->m_yPrecision);
	*(impl->m_stream) << y + impl->m_offset.y() << " ";

	impl->m_stream->setRealNumberPrecision(impl->m_zPrecision);
	*(impl->m_stream) << z << endl;
}

void TpoExporter::setOffset(double x, double y)
{
	setOffset(QPointF(x, y));
}

void TpoExporter::setOffset(const QPointF& offset)
{
	impl->m_offset = offset;
}
