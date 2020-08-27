#include "iricmainwindow_snapshotsaver.h"

#include <guicore/base/snapshotenabledwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QString>

#include <vtkGL2PSExporter.h>
#include <vtkSmartPointer.h>

iRICMainWindow::SnapshotSaver::SnapshotSaver(iRICMainWindow* mainWindow) :
	m_mainWindow {mainWindow}
{}

void iRICMainWindow::SnapshotSaver::save(SnapshotEnabledWindowInterface* enabledWindow)
{
	QString defaultname = QDir(LastIODirectory::get()).absoluteFilePath("snapshot");
	QString fileTypes = iRICMainWindow::tr("PNG files (*.png);;JPEG file (*.jpg);;Windows BMP file (*.bmp)");
	if (enabledWindow->getVtkRenderWindow() != nullptr) {
		fileTypes.append(iRICMainWindow::tr(";;Encapsulated Post Script file (*.eps);;Portable Document Format file (*.pdf);;Scalable Vector Graphics file (*.svg)"));
	}
	QString filename = QFileDialog::getSaveFileName(m_mainWindow, iRICMainWindow::tr("Save Snapshot"), defaultname, fileTypes);
	if (filename == "") {return;}

	auto suffix = QFileInfo(filename).suffix();

	if (suffix == "jpg" || suffix == "png" || suffix == "bmp") {
		bool transparent = false;
		if (suffix == "png") {
			auto w = dynamic_cast<QWidget*> (enabledWindow);
			int ret = QMessageBox::information(w, tr("Transparency Setting"), tr("Do you want to make the background transparent?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			transparent = (ret == QMessageBox::Yes);
		}
		enabledWindow->setTransparent(transparent);
		QPixmap pixmap = enabledWindow->snapshot();
		pixmap.save(filename);
	} else if (suffix == "pdf" || suffix == "eps" || suffix == "svg") {
		vtkRenderWindow* renderWindow = enabledWindow->getVtkRenderWindow();
		if (renderWindow == nullptr) {
			QMessageBox::warning(m_mainWindow, iRICMainWindow::tr("Warning"), iRICMainWindow::tr("This window do not support snapshot with this file type."));
			return;
		}
		vtkSmartPointer<vtkGL2PSExporter> exp = vtkSmartPointer<vtkGL2PSExporter>::New();

		exp->SetRenderWindow(renderWindow);
		exp->CompressOff();
		exp->Write3DPropsAsRasterImageOff();
		exp->TextOn();
		if (suffix == "pdf") {
			exp->SetFileFormatToPDF();
		} else if (suffix == "eps") {
			exp->SetFileFormatToEPS();
		} else if (suffix == "svg") {
			exp->SetFileFormatToSVG();
		}
		QString tmppath = m_mainWindow->m_projectData->tmpFileName();
		exp->SetFilePrefix(iRIC::toStr(tmppath).c_str());
		exp->Write();
		QString tmpname = QString("%1.%2").arg(tmppath).arg(suffix);

		QFile tempFile(tmpname);
		if (! tempFile.exists()) {
			QMessageBox::critical(m_mainWindow, iRICMainWindow::tr("Error"), iRICMainWindow::tr("Saving snapshot failed."));
			return;
		}
		QFile newFile(filename);
		newFile.remove();

		bool ok = QFile::rename(tmpname, filename);
		if (! ok) {
			QMessageBox::critical(m_mainWindow, iRICMainWindow::tr("Error"), iRICMainWindow::tr("Saving snapshot failed."));
			return;
		}
	} else {
		QMessageBox::warning(m_mainWindow, tr("Warning"), tr("Wrong file name specified."));
		return;
	}
	LastIODirectory::set(QFileInfo(filename).absolutePath());
	m_mainWindow->statusBar()->showMessage(iRICMainWindow::tr("Snapshot successfully saved to %1.").arg(QDir::toNativeSeparators(filename)), iRICMainWindow::STATUSBAR_DISPLAYTIME);
}
