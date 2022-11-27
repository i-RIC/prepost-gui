#include "qfonteditwidget.h"
#include "ui_qfonteditwidget.h"

#include <QFontDialog>
#include <QPushButton>

QFontEditWidget::QFontEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QFontEditWidget)
{
	ui->setupUi(this);
	connect(ui->editButton, &QPushButton::clicked, this, &QFontEditWidget::openEditDialog);
}

QFontEditWidget::~QFontEditWidget()
{
	delete ui;
}

QFont QFontEditWidget::font() const
{
	return m_font;
}

void QFontEditWidget::setFont(const QFont& font)
{
	m_font = font;
	ui->displayLabel->setFont(m_font);
}

void QFontEditWidget::openEditDialog()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_font, this);
	if (! ok) {return;}

	m_font = newFont;
	ui->displayLabel->setFont(m_font);
}
