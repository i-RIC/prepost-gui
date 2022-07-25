#include "startpagelabel.h"

#include "private/startpagelabel_impl.h"

#include <QFont>
#include <QPainter>

StartPageLabel::StartPageLabel(QWidget* parent) :
	QWidget(parent),
	impl {new Impl {}}
{
	setCursor(Qt::PointingHandCursor);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

StartPageLabel::~StartPageLabel()
{
	delete impl;
}

void StartPageLabel::setTitle(const QString& title)
{
	impl->m_title = title;
}

void StartPageLabel::setSubtitle(const QString& title)
{
	impl->m_subtitle = title;
}

void StartPageLabel::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	QRect r = rect();
	painter.fillRect(r, Qt::white);
	QImage img(":/images/headMark.png");
	painter.drawImage(2, 2, img);
	QFont font("Helvetica", 11, QFont::Bold);
	painter.setFont(font);
	int titleHeight = painter.fontMetrics().height();

	QRect titleRect(r.left() + IMAGEMARGIN, r.top(), r.width() - IMAGEMARGIN, titleHeight);
	painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, impl->m_title);
	font = QFont("Helvetica", 9, QFont::Normal);
	painter.setFont(font);
	painter.setPen(QPen(Qt::darkGray));
	int subTitleHeight = painter.fontMetrics().height();
	QRect subtitleRect(r.left() + IMAGEMARGIN, r.top() + titleHeight + 2, r.width() - IMAGEMARGIN, subTitleHeight);
	painter.drawText(subtitleRect, Qt::AlignLeft | Qt::AlignVCenter, impl->m_subtitle);
}

void StartPageLabel::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}

QSize StartPageLabel::sizeHint() const
{
	int h = 6;

	QPainter painter;

	QFont font("Helvetica", 11, QFont::Bold);
	painter.setFont(font);
	int titleHeight = static_cast<int> (painter.fontMetrics().height() * 1.2);
	h += titleHeight;

	font = QFont("Helvetica", 9, QFont::Normal);
	painter.setFont(font);
	int subTitleHeight = painter.fontMetrics().height();
	h += subTitleHeight;

	return QSize(300, h);
}
