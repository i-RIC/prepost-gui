#include "rivexporter.h"
#include "../data/baseline/baseline.h"
#include "../data/crosssections/crosssections.h"
#include "../data/project/project.h"
#include "../data/crosssection/crosssection.h"
#include "../data/crosssections/crosssections.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QVector2D>

#include <string>

RivExporter::RivExporter() :
	m_rivFileName {},
	m_csvFileName {}
{}

bool RivExporter::exportData(const Project& project, QWidget* w)
{
	QString fname = QFileDialog::getSaveFileName(w, tr("Input File name to export"), m_rivFileName, tr("River Survey Data(*.riv)"));
	if (fname.isNull()) {return false;}

	m_rivFileName = fname;
	QFileInfo finfo(fname);
	m_csvFileName = QString("%1/%2_wse.csv").arg(finfo.absolutePath()).arg(finfo.baseName());

	QFile file(fname);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(fname)));
		return false;
	}

	QTextStream ts(&file);
	QPointF offset = project.offset();
	const auto& cs = project.crossSections();

	ts.setRealNumberPrecision(12);

	ts << "#survey" << endl;
	for (CrossSection* s : cs.crossSectionVector()) {
		ts
				<< s->name() << " "
				<< s->point1().x() + offset.x() << " "
				<< s->point1().y() + offset.y() << " "
				<< s->point2().x() + offset.x() << " "
				<< s->point2().y() + offset.y() << endl;
	}
	ts << endl;

	ts.setRealNumberPrecision(6);

	ts << "#x-section" << endl;
	for (CrossSection* s : cs.crossSectionVector()) {
		auto points = s->mappedPoints();
		ts << s->name() << " " << points.size() << endl;

		int index = 0;
		for (auto p : points) {
			ts << " " << p.x() << " " << p.y();
			if (index % 4 == 3) {ts << endl;}
			++ index;
		}

		ts << endl;
	}
	file.close();

	return exportCsvData(project, w);
}

bool RivExporter::exportCsvData(const Project &project, QWidget *w)
{
	bool update = true;;
	if (m_csvFileName.isNull()) {
		QString fname = QFileDialog::getSaveFileName(w, tr("CSV File name to export"), "", tr("CSV file(*.csv)"));
		if (fname.isNull()) {return false;}

		m_csvFileName = fname;
		update = false;
	}
	QFile file(m_csvFileName);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(file.fileName())));
		return false;
	}

	const auto& bl = project.baseLine();

	const auto& cs = project.crossSections();
	QTextStream ts(&file);
	for (CrossSection* s : cs.crossSectionVector()) {
		bool crosses;
		double x, y, pos;
		bl.getCrossingPoint(s, &crosses, &x, &y, &pos);
		ts << pos << "," << s->waterElevation() << endl;
	}
	file.close();

	if (update) {
		QMessageBox::information(w, tr("Information"), tr("%1 is updated.").arg(QDir::toNativeSeparators(m_csvFileName)));
	}
	return true;
}
