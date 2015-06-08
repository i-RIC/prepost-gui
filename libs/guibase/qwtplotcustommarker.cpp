/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwtplotcustommarker.h"
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_symbol.h>
#include <qwt_text.h>
#include <qwt_math.h>
#include <qpainter.h>

class QwtPlotCustomMarker::PrivateData
{
public:
	PrivateData():
		labelAlignment(Qt::AlignCenter),
		labelOrientation(Qt::Horizontal),
		spacing(2),
		symbol(NULL),
		style(QwtPlotCustomMarker::NoLine),
		xValue(0.0),
		yValue(0.0) {
	}

	~PrivateData() {
		delete symbol;
	}

	QwtText label;
	Qt::Alignment labelAlignment;
	Qt::Orientation labelOrientation;
	int spacing;

	QPen pen;
	QPen labelPen;
	const QwtSymbol* symbol;
	LineStyle style;

	double xValue;
	double yValue;
};

//! Sets alignment to Qt::AlignCenter, and style to QwtPlotCustomMarker::NoLine
QwtPlotCustomMarker::QwtPlotCustomMarker(const QString& title):
	QwtPlotItem(QwtText(title))
{
	d_data = new PrivateData;
	setZ(30.0);
}

//! Sets alignment to Qt::AlignCenter, and style to QwtPlotCustomMarker::NoLine
QwtPlotCustomMarker::QwtPlotCustomMarker(const QwtText& title):
	QwtPlotItem(title)
{
	d_data = new PrivateData;
	setZ(30.0);
}

//! Destructor
QwtPlotCustomMarker::~QwtPlotCustomMarker()
{
	delete d_data;
}

//! \return QwtPlotItem::Rtti_PlotMarker
int QwtPlotCustomMarker::rtti() const
{
	return QwtPlotItem::Rtti_PlotMarker;
}

//! Return Value
QPointF QwtPlotCustomMarker::value() const
{
	return QPointF(d_data->xValue, d_data->yValue);
}

//! Return x Value
double QwtPlotCustomMarker::xValue() const
{
	return d_data->xValue;
}

//! Return y Value
double QwtPlotCustomMarker::yValue() const
{
	return d_data->yValue;
}

//! Set Value
void QwtPlotCustomMarker::setValue(const QPointF& pos)
{
	setValue(pos.x(), pos.y());
}

//! Set Value
void QwtPlotCustomMarker::setValue(double x, double y)
{
	if (x != d_data->xValue || y != d_data->yValue) {
		d_data->xValue = x;
		d_data->yValue = y;
		itemChanged();
	}
}

//! Set X Value
void QwtPlotCustomMarker::setXValue(double x)
{
	setValue(x, d_data->yValue);
}

//! Set Y Value
void QwtPlotCustomMarker::setYValue(double y)
{
	setValue(d_data->xValue, y);
}

/*!
  Draw the marker

  \param painter Painter
  \param xMap x Scale Map
  \param yMap y Scale Map
  \param canvasRect Contents rectangle of the canvas in painter coordinates
*/
void QwtPlotCustomMarker::draw(QPainter* painter,
															 const QwtScaleMap& xMap, const QwtScaleMap& yMap,
															 const QRectF& canvasRect) const
{
	const QPointF pos(xMap.transform(d_data->xValue),
										yMap.transform(d_data->yValue));

	// draw lines

	drawLines(painter, canvasRect, pos);

	// draw symbol
	if (d_data->symbol &&
			(d_data->symbol->style() != QwtSymbol::NoSymbol)) {
		const QSizeF sz = d_data->symbol->size();

		const QRectF clipRect = canvasRect.adjusted(
															-sz.width(), -sz.height(), sz.width(), sz.height());

		if (clipRect.contains(pos)) {
			d_data->symbol->drawSymbol(painter, pos);
		}
	}

	painter->setPen(d_data->labelPen);
	drawLabel(painter, canvasRect, pos);
}

/*!
  Draw the lines marker

  \param painter Painter
  \param canvasRect Contents rectangle of the canvas in painter coordinates
  \param pos Position of the marker, translated into widget coordinates

  \sa drawLabel(), QwtSymbol::drawSymbol()
*/
void QwtPlotCustomMarker::drawLines(QPainter* painter,
																		const QRectF& canvasRect, const QPointF& pos) const
{
	if (d_data->style == NoLine) {
		return;
	}

	const bool doAlign = true;

	painter->setPen(d_data->pen);
	if (d_data->style == QwtPlotCustomMarker::HLine ||
			d_data->style == QwtPlotCustomMarker::Cross) {
		double y = pos.y();
		if (doAlign) {
			y = qRound(y);
		}

		QwtPainter::drawLine(painter, canvasRect.left(),
												 y, canvasRect.right() - 1.0, y);
	}
	if (d_data->style == QwtPlotCustomMarker::VLine ||
			d_data->style == QwtPlotCustomMarker::Cross) {
		double x = pos.x();
		if (doAlign) {
			x = qRound(x);
		}

		QwtPainter::drawLine(painter, x,
												 canvasRect.top(), x, canvasRect.bottom() - 1.0);
	}
}

/*!
  Align and draw the text label of the marker

  \param painter Painter
  \param canvasRect Contents rectangle of the canvas in painter coordinates
  \param pos Position of the marker, translated into widget coordinates

  \sa drawLabel(), QwtSymbol::drawSymbol()
*/
void QwtPlotCustomMarker::drawLabel(QPainter* painter,
																		const QRectF& canvasRect, const QPointF& pos) const
{
	if (d_data->label.isEmpty()) {
		return;
	}

	Qt::Alignment align = d_data->labelAlignment;
	QPointF alignPos = pos;

	QSizeF symbolOff(0, 0);

	switch (d_data->style) {
	case QwtPlotCustomMarker::VLine: {
			// In VLine-style the y-position is pointless and
			// the alignment flags are relative to the canvas

			if (d_data->labelAlignment & Qt::AlignTop) {
				alignPos.setY(canvasRect.top());
				align &= ~Qt::AlignTop;
				align |= Qt::AlignBottom;
			} else if (d_data->labelAlignment & Qt::AlignBottom) {
				// In HLine-style the x-position is pointless and
				// the alignment flags are relative to the canvas

				alignPos.setY(canvasRect.bottom() - 1);
				align &= ~Qt::AlignBottom;
				align |= Qt::AlignTop;
			} else {
				alignPos.setY(canvasRect.center().y());
			}
			break;
		}
	case QwtPlotCustomMarker::HLine: {
			if (d_data->labelAlignment & Qt::AlignLeft) {
				alignPos.setX(canvasRect.left());
				align &= ~Qt::AlignLeft;
				align |= Qt::AlignRight;
			} else if (d_data->labelAlignment & Qt::AlignRight) {
				alignPos.setX(canvasRect.right() - 1);
				align &= ~Qt::AlignRight;
				align |= Qt::AlignLeft;
			} else {
				alignPos.setX(canvasRect.center().x());
			}
			break;
		}
	default: {
			if (d_data->symbol &&
					(d_data->symbol->style() != QwtSymbol::NoSymbol)) {
				symbolOff = d_data->symbol->size() + QSizeF(1, 1);
				symbolOff /= 2;
			}
		}
	}

	qreal pw2 = d_data->pen.widthF() / 2.0;
	if (pw2 == 0.0) {
		pw2 = 0.5;
	}

	const int spacing = d_data->spacing;

	const qreal xOff = qMax(pw2, symbolOff.width());
	const qreal yOff = qMax(pw2, symbolOff.height());

	const QSizeF textSize = d_data->label.textSize(painter->font());

	if (align & Qt::AlignLeft) {
		alignPos.rx() -= xOff + spacing;
		if (d_data->labelOrientation == Qt::Vertical) {
			alignPos.rx() -= textSize.height();
		} else {
			alignPos.rx() -= textSize.width();
		}
	} else if (align & Qt::AlignRight) {
		alignPos.rx() += xOff + spacing;
	} else {
		if (d_data->labelOrientation == Qt::Vertical) {
			alignPos.rx() -= textSize.height() / 2;
		} else {
			alignPos.rx() -= textSize.width() / 2;
		}
	}

	if (align & Qt::AlignTop) {
		alignPos.ry() -= yOff + spacing;
		if (d_data->labelOrientation != Qt::Vertical) {
			alignPos.ry() -= textSize.height();
		}
	} else if (align & Qt::AlignBottom) {
		alignPos.ry() += yOff + spacing;
		if (d_data->labelOrientation == Qt::Vertical) {
			alignPos.ry() += textSize.width();
		}
	} else {
		if (d_data->labelOrientation == Qt::Vertical) {
			alignPos.ry() += textSize.width() / 2;
		} else {
			alignPos.ry() -= textSize.height() / 2;
		}
	}

	painter->translate(alignPos.x(), alignPos.y());
	if (d_data->labelOrientation == Qt::Vertical) {
		painter->rotate(-90.0);
	}

	const QRectF textRect(0, 0, textSize.width(), textSize.height());
	d_data->label.draw(painter, textRect);
}

/*!
  \brief Set the line style
  \param style Line style.
  \sa lineStyle()
*/
void QwtPlotCustomMarker::setLineStyle(LineStyle style)
{
	if (style != d_data->style) {
		d_data->style = style;

		legendChanged();
		itemChanged();
	}
}

/*!
  \return the line style
  \sa setLineStyle()
*/
QwtPlotCustomMarker::LineStyle QwtPlotCustomMarker::lineStyle() const
{
	return d_data->style;
}

/*!
  \brief Assign a symbol
  \param symbol New symbol
  \sa symbol()
*/
void QwtPlotCustomMarker::setSymbol(const QwtSymbol* symbol)
{
	if (symbol != d_data->symbol) {
		delete d_data->symbol;
		d_data->symbol = symbol;

		if (symbol) {
			setLegendIconSize(symbol->boundingRect().size());
		}

		legendChanged();
		itemChanged();
	}
}

/*!
  \return the symbol
  \sa setSymbol(), QwtSymbol
*/
const QwtSymbol* QwtPlotCustomMarker::symbol() const
{
	return d_data->symbol;
}

/*!
  \brief Set the label
  \param label Label text
  \sa label()
*/
void QwtPlotCustomMarker::setLabel(const QwtText& label)
{
	if (label != d_data->label) {
		d_data->label = label;
		itemChanged();
	}
}

/*!
  \return the label
  \sa setLabel()
*/
QwtText QwtPlotCustomMarker::label() const
{
	return d_data->label;
}

/*!
  \brief Set the alignment of the label

  In case of QwtPlotCustomMarker::HLine the alignment is relative to the
  y position of the marker, but the horizontal flags correspond to the
  canvas rectangle. In case of QwtPlotCustomMarker::VLine the alignment is
  relative to the x position of the marker, but the vertical flags
  correspond to the canvas rectangle.

  In all other styles the alignment is relative to the marker's position.

  \param align Alignment.
  \sa labelAlignment(), labelOrientation()
*/
void QwtPlotCustomMarker::setLabelAlignment(Qt::Alignment align)
{
	if (align != d_data->labelAlignment) {
		d_data->labelAlignment = align;
		itemChanged();
	}
}

/*!
  \return the label alignment
  \sa setLabelAlignment(), setLabelOrientation()
*/
Qt::Alignment QwtPlotCustomMarker::labelAlignment() const
{
	return d_data->labelAlignment;
}

/*!
  \brief Set the orientation of the label

  When orientation is Qt::Vertical the label is rotated by 90.0 degrees
  ( from bottom to top ).

  \param orientation Orientation of the label

  \sa labelOrientation(), setLabelAlignment()
*/
void QwtPlotCustomMarker::setLabelOrientation(Qt::Orientation orientation)
{
	if (orientation != d_data->labelOrientation) {
		d_data->labelOrientation = orientation;
		itemChanged();
	}
}

/*!
  \return the label orientation
  \sa setLabelOrientation(), labelAlignment()
*/
Qt::Orientation QwtPlotCustomMarker::labelOrientation() const
{
	return d_data->labelOrientation;
}

/*!
  \brief Set the spacing

  When the label is not centered on the marker position, the spacing
  is the distance between the position and the label.

  \param spacing Spacing
  \sa spacing(), setLabelAlignment()
*/
void QwtPlotCustomMarker::setSpacing(int spacing)
{
	if (spacing < 0) {
		spacing = 0;
	}

	if (spacing == d_data->spacing) {
		return;
	}

	d_data->spacing = spacing;
	itemChanged();
}

/*!
  \return the spacing
  \sa setSpacing()
*/
int QwtPlotCustomMarker::spacing() const
{
	return d_data->spacing;
}

/*!
  Build and assign a line pen

  In Qt5 the default pen width is 1.0 ( 0.0 in Qt4 ) what makes it
  non cosmetic ( see QPen::isCosmetic() ). This method has been introduced
  to hide this incompatibility.

  \param color Pen color
  \param width Pen width
  \param style Pen style

  \sa pen(), brush()
 */
void QwtPlotCustomMarker::setLinePen(const QColor& color, qreal width, Qt::PenStyle style)
{
	setLinePen(QPen(color, width, style));
}

/*!
  Specify a pen for the line.

  \param pen New pen
  \sa linePen()
*/
void QwtPlotCustomMarker::setLinePen(const QPen& pen)
{
	if (pen != d_data->pen) {
		d_data->pen = pen;

		legendChanged();
		itemChanged();
	}
}

/*!
  \return the line pen
  \sa setLinePen()
*/
const QPen& QwtPlotCustomMarker::linePen() const
{
	return d_data->pen;
}

QRectF QwtPlotCustomMarker::boundingRect() const
{
	return QRectF(d_data->xValue, d_data->yValue, 0.0, 0.0);
}

/*!
   \return Icon representing the marker on the legend

   \param index Index of the legend entry
                ( usually there is only one )
   \param size Icon size

   \sa setLegendIconSize(), legendData()
*/
QwtGraphic QwtPlotCustomMarker::legendIcon(int index,
		const QSizeF& size) const
{
	Q_UNUSED(index);

	if (size.isEmpty()) {
		return QwtGraphic();
	}

	QwtGraphic icon;
	icon.setDefaultSize(size);
	icon.setRenderHint(QwtGraphic::RenderPensUnscaled, true);

	QPainter painter(&icon);
	painter.setRenderHint(QPainter::Antialiasing,
												testRenderHint(QwtPlotItem::RenderAntialiased));

	if (d_data->style != QwtPlotCustomMarker::NoLine) {
		painter.setPen(d_data->pen);

		if (d_data->style == QwtPlotCustomMarker::HLine ||
				d_data->style == QwtPlotCustomMarker::Cross) {
			const double y = 0.5 * size.height();

			QwtPainter::drawLine(&painter,
													 0.0, y, size.width(), y);
		}

		if (d_data->style == QwtPlotCustomMarker::VLine ||
				d_data->style == QwtPlotCustomMarker::Cross) {
			const double x = 0.5 * size.width();

			QwtPainter::drawLine(&painter,
													 x, 0.0, x, size.height());
		}
	}

	if (d_data->symbol) {
		const QRect r(0.0, 0.0, size.width(), size.height());
		d_data->symbol->drawSymbol(&painter, r);
	}

	return icon;
}

void QwtPlotCustomMarker::setLabelPen(const QPen& pen)
{
	if (pen != d_data->labelPen) {
		d_data->labelPen = pen;
		itemChanged();
	}
}

const QPen& QwtPlotCustomMarker::labelPen() const
{
	return d_data->labelPen;
}
