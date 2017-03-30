#ifndef IO_H
#define IO_H

#include <misc/lastiodirectory.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStringList>

template <class F, class DATA, class I>
DATA importDataT(QWidget* w)
{
	auto& f = F::instance();
	QStringList filters;
	QStringList extensions;
	QMap<QString, I*> extMap;

	for (auto importer : f.importers()) {
		filters << importer->fileDialogFilters();
		for (QString& ext : importer->acceptableExtensions()) {
			extensions.push_back(QString("*.").append(ext));
			extMap.insert(ext, importer);
		}
	}
	filters.push_front(QString(QObject::tr("All importable files (%1)")).arg(extensions.join(" ")));

	QString dir = LastIODirectory::get();
	QString selectedFilter;

	DATA ret;
	QString filename = QFileDialog::getOpenFileName(w, QObject::tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);

	if (filename.isNull()) {return ret;}

	I* importer = nullptr;
	for (int i = 0; i < filters.size(); ++i) {
		if (filters[i] == selectedFilter) {
			if (i == 0) {
				QFileInfo finfo(filename);
				QString extension = finfo.suffix();
				if (extMap.contains(extension)) {
					importer = extMap.value(extension);
				} else {
					QMessageBox::warning(w, QObject::tr("Warning"), QObject::tr("File type can not be recongnized from the file extension. : %1").arg(finfo.fileName()));
					return ret;
				}
			} else {
				importer = f.importers().at(i);
			}
			break;
		}
	}

	Q_ASSERT(importer != nullptr);

	ret = importer->importData(filename, selectedFilter, w);

	if (! ret.isEmpty()) {
		LastIODirectory::setFromFilename(filename);
	}

	return ret;
}

template <class F, class DATA, class I>
void exportDataT(const DATA& data, QWidget* w)
{
	auto& f = F::instance();
	QStringList filters;

	for (auto exporter : f.exporters()) {
		filters << exporter->fileDialogFilters();
	}

	QString dir = LastIODirectory::get();
	QString selectedFilter;

	QString filename = QFileDialog::getSaveFileName(w, QObject::tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);

	if (filename.isNull()) {return;}

	LastIODirectory::setFromFilename(filename);

	I* exporter = nullptr;
	for (int i = 0; i < filters.size(); ++i) {
		if (filters[i] == selectedFilter) {
			exporter = f.exporters().at(i);
			break;
		}
	}

	Q_ASSERT(exporter != nullptr);

	exporter->exportData(data, filename, selectedFilter, w);
}

#endif // IO_H
