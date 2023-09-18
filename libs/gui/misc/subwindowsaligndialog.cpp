#include "subwindowsaligndialog.h"
#include "ui_subwindowsaligndialog.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include <guibase/objectbrowser.h>
#include <guicore/base/qmainwindowwithsnapshot.h>
#include <guicore/base/windowwithobjectbrowseri.h>

#include <map>

SubwindowsAlignDialog::SubwindowsAlignDialog(QMdiArea* area, QWidget *parent) :
	QDialog(parent),
	m_mdiArea {area},
	ui(new Ui::SubwindowsAlignDialog)
{
	ui->setupUi(this);
	ui->sizeWidget->setSize(area->size());

	connect(ui->horizontalRadioButton, &QRadioButton::clicked, this, &SubwindowsAlignDialog::updateMarginWidgetStatus);
	connect(ui->verticalRadioButton, &QRadioButton::clicked, this, &SubwindowsAlignDialog::updateMarginWidgetStatus);
	connect(ui->gridRadioButton, &QRadioButton::clicked, this, &SubwindowsAlignDialog::updateMarginWidgetStatus);

	updateMarginWidgetStatus();
}

SubwindowsAlignDialog::~SubwindowsAlignDialog()
{
	delete ui;
}

void SubwindowsAlignDialog::accept()
{
	alignSubWindows();

	QDialog::accept();
}

void SubwindowsAlignDialog::updateMarginWidgetStatus()
{
	ui->horizontalMarginSpinBox->setEnabled(ui->horizontalRadioButton->isChecked() || ui->gridRadioButton->isChecked());
	ui->verticalMarginSpinBox->setEnabled(ui->verticalRadioButton->isChecked() || ui->gridRadioButton->isChecked());
}

void SubwindowsAlignDialog::alignSubWindows()
{
	std::vector<WindowWithObjectBrowserI*> windowsToShowObjectBrowser;
	std::vector<QMdiSubWindow*> activeSubWindows;

	bool hideOb = ui->hideObjectBrowsersCheckBox->isChecked();
	for (auto subWindow : m_mdiArea->subWindowList()) {
		if (subWindow->isHidden()) {continue;}
		if (subWindow->isMinimized()) {continue;}

		activeSubWindows.push_back(subWindow);
		auto wob = dynamic_cast<WindowWithObjectBrowserI*> (subWindow->widget());
		if (wob != nullptr) {
			auto ob = wob->objectBrowser();
			if (! ob->isHidden()) {
				ob->hide();
				if (! hideOb) {
					windowsToShowObjectBrowser.push_back(wob);
				}
			}
		}
	}
	qApp->processEvents();

	if (activeSubWindows.size() > 0) {
		if (ui->horizontalRadioButton->isChecked()) {
			alignSubWindowsHorizontal(activeSubWindows);
		} else if (ui->verticalRadioButton->isChecked()) {
			alignSubWindowsVertical(activeSubWindows);
		} else if (ui->gridRadioButton->isChecked()) {
			alignSubWindowsGrid(activeSubWindows);
		}
	}
	for (auto wob : windowsToShowObjectBrowser) {
		wob->objectBrowser()->show();
	}
}

void SubwindowsAlignDialog::alignSubWindowsHorizontal(const std::vector<QMdiSubWindow*>& subWindows)
{
	std::vector<QMdiSubWindow*> orderedWindows = subWindows;
	struct compareX {
		bool operator()(const QMdiSubWindow* a, const QMdiSubWindow* b) {
			return a->pos().x() < b->pos().x();
		}
	};

	std::sort(orderedWindows.begin(), orderedWindows.end(), compareX {});

	auto windowCount = static_cast<int> (subWindows.size());
	auto totalSize = ui->sizeWidget->size();

	auto hMargin = ui->horizontalMarginSpinBox->value();
	auto marginCount = windowCount - 1;
	auto stdWidth = (totalSize.width() - hMargin * marginCount) / windowCount;
	auto lastWidth = totalSize.width() - (stdWidth + hMargin) * marginCount;
	auto height = totalSize.height();

	int i = 0;
	for (auto w : orderedWindows) {
		int width = 0;
		if (i == windowCount - 1) {
			width = lastWidth;
		} else {
			width = stdWidth;
		}
		w->move(QPoint(i * (stdWidth + hMargin), 0));
		auto w2 = dynamic_cast<QMainWindowWithSnapshot*> (w->widget());
		w2->resizeSnapshotAreaTo(QSize(width, height), w);

		++i;
	}
}

void SubwindowsAlignDialog::alignSubWindowsVertical(const std::vector<QMdiSubWindow*>& subWindows)
{
	std::vector<QMdiSubWindow*> orderedWindows = subWindows;
	struct compareY {
		bool operator()(const QMdiSubWindow* a, const QMdiSubWindow* b) {
			return a->pos().y() < b->pos().y();
		}
	};

	std::sort(orderedWindows.begin(), orderedWindows.end(), compareY {});

	auto windowCount = static_cast<int> (subWindows.size());
	auto totalSize = ui->sizeWidget->size();

	auto vMargin = ui->verticalMarginSpinBox->value();
	auto marginCount = windowCount - 1;
	auto stdHeight = (totalSize.height() - vMargin * marginCount) / windowCount;
	auto lastHeight = totalSize.height() - (stdHeight + vMargin) * marginCount;
	auto width = totalSize.width();

	int i = 0;
	for (auto w : orderedWindows) {
		int height = 0;
		if (i == windowCount - 1) {
			height = lastHeight;
		} else {
			height = stdHeight;
		}
		w->move(QPoint(0, i * (stdHeight + vMargin)));
		auto w2 = dynamic_cast<QMainWindowWithSnapshot*> (w->widget());
		w2->resizeSnapshotAreaTo(QSize(width, height), w);

		++i;
	}
}

void SubwindowsAlignDialog::alignSubWindowsGrid(const std::vector<QMdiSubWindow*>& subWindows)
{
	if (ui->columnsRadioButton->isChecked()) {
		alignSubWindowsGridForColumns(subWindows);
	} else if (ui->rowsRadioButton->isChecked()) {
		alignSubWindowsGridForRows(subWindows);
	}
}

void SubwindowsAlignDialog::alignSubWindowsGridForColumns(const std::vector<QMdiSubWindow*>& subWindows)
{
	std::vector<QMdiSubWindow*> orderedWindows = subWindows;

	struct compareY {
		bool operator()(const QMdiSubWindow* a, const QMdiSubWindow* b) {
			return a->pos().y() < b->pos().y();
		}
	};
	std::sort(orderedWindows.begin(), orderedWindows.end(), compareY {});

	auto columns = ui->columnsSpinBox->value();
	auto rows = static_cast<int> (std::ceil(subWindows.size() / static_cast<double> (columns)));

	auto totalSize = ui->sizeWidget->size();

	auto hMargin = ui->horizontalMarginSpinBox->value();
	auto hMarginCount = columns - 1;
	auto vMargin = ui->verticalMarginSpinBox->value();
	auto vMarginCount = rows - 1;
	auto stdWidth = (totalSize.width() - hMargin * hMarginCount) / columns;
	auto lastWidth = totalSize.width() - (stdWidth + hMargin) * hMarginCount;
	auto stdHeight = (totalSize.height() - vMargin * vMarginCount) / rows;
	auto lastHeight = totalSize.height() - (stdHeight + vMargin) * vMarginCount;

	struct compareX {
		bool operator()(const QMdiSubWindow* a, const QMdiSubWindow* b) {
			return a->pos().x() < b->pos().x();
		}
	};

	for (int row = 0; row < rows; ++row) {
		std::vector<QMdiSubWindow*> orderedWindow2;
		for (int index = row * columns; index < (row + 1) * columns && index < orderedWindows.size(); ++index) {
			orderedWindow2.push_back(orderedWindows.at(index));
		}
		std::sort(orderedWindow2.begin(), orderedWindow2.end(), compareX {});
		for (int column = 0; column < columns; ++column) {
			if (column >= static_cast<int> (orderedWindow2.size())) {break;}
			int width = 0, height = 0;
			if (row == rows - 1) {
				height = lastHeight;
			} else {
				height = stdHeight;
			}
			if (column == columns - 1) {
				width = lastWidth;
			} else {
				width = stdWidth;
			}
			auto w = orderedWindow2.at(column);
			w->move(QPoint(column * (stdWidth + hMargin), row * (stdHeight + vMargin)));
			auto w2 = dynamic_cast<QMainWindowWithSnapshot*> (w->widget());
			w2->resizeSnapshotAreaTo(QSize(width, height), w);
		}
	}
}

void SubwindowsAlignDialog::alignSubWindowsGridForRows(const std::vector<QMdiSubWindow*>& subWindows)
{
	std::vector<QMdiSubWindow*> orderedWindows = subWindows;

	struct compareX {
		bool operator()(const QMdiSubWindow* a, const QMdiSubWindow* b) {
			return a->pos().x() < b->pos().x();
		}
	};
	std::sort(orderedWindows.begin(), orderedWindows.end(), compareX {});

	auto rows = ui->rowsSpinBox->value();
	auto columns = static_cast<int> (std::ceil(subWindows.size() / static_cast<double>(rows)));

	auto totalSize = ui->sizeWidget->size();

	auto vMargin = ui->verticalMarginSpinBox->value();
	auto vMarginCount = rows - 1;
	auto hMargin = ui->horizontalMarginSpinBox->value();
	auto hMarginCount = columns - 1;
	auto stdHeight = (totalSize.height() - vMargin * vMarginCount) / rows;
	auto lastHeight = totalSize.height() - (stdHeight + vMargin) * vMarginCount;
	auto stdWidth = (totalSize.width() - hMargin * hMarginCount) / columns;
	auto lastWidth = totalSize.width() - (stdWidth + hMargin) * hMarginCount;

	struct compareY {
		bool operator()(const QMdiSubWindow* a, const QMdiSubWindow* b) {
			return a->pos().y() < b->pos().y();
		}
	};

	for (int column = 0; column < columns; ++column) {
		std::vector<QMdiSubWindow*> orderedWindow2;
		for (int index = column * rows; index < (column + 1) * rows && index < orderedWindows.size(); ++index) {
			orderedWindow2.push_back(orderedWindows.at(index));
		}
		std::sort(orderedWindow2.begin(), orderedWindow2.end(), compareY {});
		for (int row = 0; row < rows; ++row) {
			if (row >= static_cast<int> (orderedWindow2.size())) {break;}
			int width = 0, height = 0;
			if (column == columns - 1) {
				width = lastWidth;
			} else {
				width = stdWidth;
			}
			if (row == rows - 1) {
				height = lastHeight;
			} else {
				height = stdHeight;
			}
			auto w = orderedWindow2.at(row);
			w->move(QPoint(column * (stdWidth + hMargin), row * (stdHeight + vMargin)));
			auto w2 = dynamic_cast<QMainWindowWithSnapshot*> (w->widget());
			w2->resizeSnapshotAreaTo(QSize(width, height), w);
		}
	}
}
