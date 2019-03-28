#include "ui_propertybrowserview.h"

#include "attributebrowsertargetdataitem.h"
#include "propertybrowserview.h"

#include <QVector2D>

PropertyBrowserView::PropertyBrowserView(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::PropertyBrowserView)
{
	ui->setupUi(this);
	m_targetDataItem = 0;
}

PropertyBrowserView::~PropertyBrowserView()
{
	delete ui;
}

void PropertyBrowserView::setTargetDataItem(AttributeBrowserTargetDataItem* item)
{
	m_targetDataItem = item;
}

void PropertyBrowserView::resetForVertex(bool structured)
{
	ui->indicesGroupBox->show();
	ui->coordinatesGroupBox->show();
	ui->attLabel->show();
	ui->attributesTable->show();
	resetBase(structured);
	updateGeometry();
	resetAttributes();
}

void PropertyBrowserView::resetForCell(bool structured)
{
	ui->indicesGroupBox->show();
	ui->coordinatesGroupBox->show();
	ui->attLabel->show();
	ui->attributesTable->show();
	resetBase(structured);
	updateGeometry();
	resetAttributes();
}

void PropertyBrowserView::resetForParticle()
{
	ui->indicesGroupBox->show();
	ui->coordinatesGroupBox->show();
	ui->attLabel->show();
	ui->attributesTable->show();
	resetBase(false);
	updateGeometry();
	resetAttributes();
}

void PropertyBrowserView::resetForPolyData()
{
	ui->indicesGroupBox->show();
	ui->coordinatesGroupBox->hide();
	ui->attLabel->show();
	ui->attributesTable->show();
	resetBase(false);
	updateGeometry();
	resetAttributes();
}

void PropertyBrowserView::hideAll()
{
	ui->indicesGroupBox->hide();
	ui->coordinatesGroupBox->hide();
	ui->attLabel->hide();
	ui->attributesTable->hide();
	updateGeometry();
	if (m_targetDataItem) {
		m_targetDataItem->clear();
	}
}

void PropertyBrowserView::resetBase(bool structured)
{
	ui->iLabel->setVisible(structured);
	ui->jLabel->setVisible(structured);
	ui->indexLabel->setVisible(! structured);

	ui->iValueLabel->setVisible(structured);
	ui->jValueLabel->setVisible(structured);
	ui->indexValueLabel->setVisible(! structured);
}

void PropertyBrowserView::resetAttributes(bool internal)
{
	QString nullStr = "---";
	ui->indexValueLabel->setText(nullStr);
	ui->iValueLabel->setText(nullStr);
	ui->jValueLabel->setText(nullStr);
	ui->xValueLabel->setText(nullStr);
	ui->yValueLabel->setText(nullStr);
	QTableWidget* table = ui->attributesTable;
	int rows = table->rowCount();
	for (int i = rows - 1; i >= 0; --i) {
		table->removeRow(i);
	}
	if (m_targetDataItem && ! internal) {
		m_targetDataItem->clear();
	}
}

void PropertyBrowserView::setVertexAttributes(vtkIdType index, double x, double y, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIndex(index);
	updateCoords(x, y);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPoint(QPointF(x, y));
	}
}

void PropertyBrowserView::setVertexAttributes(unsigned int i, unsigned int j, double x, double y, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIJ(i, j);
	updateCoords(x, y);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPoint(QPointF(x, y));
	}
}

void PropertyBrowserView::setCellAttributes(vtkIdType index, const QPolygonF& polygon, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIndex(index);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPolygon(polygon);
	}
}

void PropertyBrowserView::setCellAttributes(unsigned int i, unsigned int j, const QPolygonF& polygon, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIJ(i, j);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPolygon(polygon);
	}
}

void PropertyBrowserView::setParticleAttributes(vtkIdType index, double x, double y, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIndex(index);
	updateCoords(x, y);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPoint(QPointF(x, y));
	}
}

void PropertyBrowserView::setPolyDataAttributes(const std::vector<QPolygonF>& polygons, vtkIdType index, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIndex(index);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPolygons(polygons);
	}
}

void PropertyBrowserView::setPolyDataAttributes(const std::vector<QPointF>& polyline, vtkIdType index, const QList<PropertyBrowserAttribute>& attr)
{
	resetAttributes(true);
	updateIndex(index);
	updateAttributes(attr);
	if (m_targetDataItem) {
		m_targetDataItem->setPolyline(polyline);
	}
}

void PropertyBrowserView::updateIndex(vtkIdType index)
{
	ui->indexValueLabel->setText(QString::number(index + 1));
}

void PropertyBrowserView::updateIJ(unsigned int i, unsigned int j)
{
	ui->iValueLabel->setText(QString::number(i + 1));
	ui->jValueLabel->setText(QString::number(j + 1));
}

void PropertyBrowserView::updateCoords(double x, double y)
{
	ui->xValueLabel->setText(QString("%1").arg(x));
	ui->yValueLabel->setText(QString("%1").arg(y));
}

void PropertyBrowserView::updateAttributes(const QList<PropertyBrowserAttribute>& attr)
{
	QTableWidget* table = ui->attributesTable;
	int rows = table->rowCount();
	for (int i = rows - 1; i >= 0; --i) {
		table->removeRow(i);
	}
	int index = 0;
	for (auto it = attr.begin(); it != attr.end(); ++it) {
		PropertyBrowserAttribute a = *it;
		table->insertRow(index);
		QTableWidgetItem* labelItem = new QTableWidgetItem(a.name);
		table->setItem(index, 0, labelItem);
		QTableWidgetItem* valueItem = new QTableWidgetItem(a.value.toString());
		table->setItem(index, 1, valueItem);
		table->setRowHeight(index, ROWHEIGHT);
		++ index;
	}
}
