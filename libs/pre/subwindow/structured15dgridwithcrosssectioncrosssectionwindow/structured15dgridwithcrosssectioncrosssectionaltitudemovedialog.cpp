#include "ui_structured15dgridwithcrosssectioncrosssectionaltitudemovedialog.h"

#include "../../datamodel/preprocessornormal15dgridwithcrosssectionshapedataitem.h"
#include "structured15dgridwithcrosssectioncrosssectionaltitudemovedialog.h"
#include "structured15dgridwithcrosssectioncrosssectionwindow.h"
#include "private/structured15dgridwithcrosssectioncrosssectionwindow_editcommand.h"

#include <misc/iricundostack.h>

#include <QPushButton>

Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog(v4Structured15dGridWithCrossSectionCrossSection* cross, int from, int to, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, QWidget* parent) :
	QDialog(parent),
	m_from {from},
	m_to {to},
	m_window {w},
	m_cross {cross},
	m_shapeItem {item},
	m_applyed {false},
	ui(new Ui::Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog)
{
	ui->setupUi(this);

	setLimits();
	ui->horizontalEdit->setValue(0);
	ui->verticalEdit->setValue(0);

	connect(ui->horizontalEdit, SIGNAL(valueChanged(double)), this, SLOT(hOffsetChange()));
	connect(ui->horizontalEdit, SIGNAL(returnPressed()), this, SLOT(hOffsetChange()));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::~Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog()
{
	delete ui;
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::setLimits()
{
	const auto& alist = m_cross->altitudeInfo();
	QString note;

	if (m_from == 0) {
		m_leftLimitSet = false;
	} else {
		m_leftLimitSet = true;
		m_leftLimit = alist.at(m_from - 1).position - alist.at(m_from).position;
		note.append(QString(tr("%1 < ")).arg(m_leftLimit));
	}
	note.append(tr("H"));
	if (m_to == static_cast<int> (alist.size()) - 1) {
		m_rightLimitSet = false;
	} else {
		m_rightLimitSet = true;
		m_rightLimit = alist.at(m_to + 1).position - alist.at(m_to).position;
		note.append(QString(tr(" < %1")).arg(m_rightLimit));
	}
	ui->horizontalNote->setText(note);
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	auto before = m_cross->altitudeInfo();
	updateCrossSection();
	auto after = m_cross->altitudeInfo();

	iRICUndoStack::instance().push(new Structured15DGridWithCrossSectionCrossSectionWindow::EditCommand(false, tr("Move Elevation Points"), after, before, m_window, m_shapeItem));
	QDialog::accept();
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_shapeItem->updateCrossSectionWindows();
	}
	QDialog::reject();
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::doReset()
{
	ui->horizontalEdit->setValue(0);
	ui->verticalEdit->setValue(0);
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_shapeItem->updateCrossSectionWindows();
	}
	m_applyed = false;
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	auto before = m_cross->altitudeInfo();
	updateCrossSection();
	auto after = m_cross->altitudeInfo();

	iRICUndoStack::instance().push(new Structured15DGridWithCrossSectionCrossSectionWindow::EditCommand(false, tr("Move Elevation Points"), after, before, m_window, m_shapeItem));
	m_applyed = true;
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::hOffsetChange()
{
	static bool exec = false;
	// for avoiding recursive execution.
	if (exec) { return; }
	exec = true;

	bool ok = true;
	if (m_leftLimitSet && ui->horizontalEdit->value() <= m_leftLimit) {
		ok = false;
	}
	if (m_rightLimitSet && ui->horizontalEdit->value() >= m_rightLimit) {
		ok = false;
	}
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(ok);
	exec = false;
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}

void Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog::updateCrossSection()
{
	auto& alist = m_cross->altitudeInfo();
	for (int i = m_from; i <= m_to; ++i) {
		alist[i].position += ui->horizontalEdit->value();
		alist[i].height += ui->verticalEdit->value();
	}
}
