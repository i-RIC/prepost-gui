#include "centeredcheckbox.h"

#include <QHBoxLayout>

CenteredCheckBox::CenteredCheckBox(QWidget* parent) :
	QWidget(parent)
{
	setAutoFillBackground(true);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	m_checkBox = new QCheckBox(this);
	layout->addWidget(m_checkBox);
	layout->addStretch(1);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
	setFocusProxy(m_checkBox);

	QPalette p = palette();
	p.setColor(QPalette::Window, Qt::white);
	setPalette(p);

	connect(m_checkBox, SIGNAL(clicked()), this, SIGNAL(clicked()));
	connect(m_checkBox, SIGNAL(clicked(bool)), this, SIGNAL(clicked(bool)));
	connect(m_checkBox, SIGNAL(pressed()), this, SIGNAL(pressed()));
	connect(m_checkBox, SIGNAL(released()), this, SIGNAL(released()));
	connect(m_checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
	connect(m_checkBox, SIGNAL(customContextMenuRequested(QPoint)), this, SIGNAL(customContextMenuRequested(QPoint)));
}
