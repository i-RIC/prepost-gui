#include "startpagelabel.h"

#include <QFont>
#include <QPainter>

StartPageLabel::StartPageLabel(QWidget* parent) :
	QWidget(parent)
{
	setCursor(Qt::PointingHandCursor);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

StartPageLabel::~StartPageLabel()
{}


void StartPageLabel::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	QRect r = rect();
	painter.fillRect(r, Qt::white);
	QImage img(":/images/headMark.png");
	painter.drawImage(2, 2, img);
	QFont font("Helvetica", 11, QFont::Bold);
	painter.setFont(font);
	QRect titleRect(r.left() + IMAGEMARGIN, r.top(), r.width() - IMAGEMARGIN, 20);
	painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, m_title);
	font = QFont("Helvetica", 9, QFont::Normal);
	painter.setFont(font);
	painter.setPen(QPen(Qt::darkGray));
	QRect subtitleRect(r.left() + IMAGEMARGIN, r.top() + 20, r.width() - IMAGEMARGIN, 20);
	painter.drawText(subtitleRect, Qt::AlignLeft | Qt::AlignVCenter, m_subtitle);
}

void StartPageLabel::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}

QSize StartPageLabel::sizeHint() const
{
	return QSize(300, 40);
}
