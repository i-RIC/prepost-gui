#ifndef GEOREFERENCEVIEW_H
#define GEOREFERENCEVIEW_H

#include "backgroundimageinfogeoreferencedialog.h"

#include <misc/primitiveview.h>

class AddibleGcpTableModel;
class BackgroundImageInfo;
class GcpTableRow;

class GeoreferenceView : public PrimitiveView
{
	Q_OBJECT
public:
	explicit GeoreferenceView(QWidget *parent = 0);
	~GeoreferenceView();

	void setInfo(BackgroundImageInfo* info, AddibleGcpTableModel* gcpTableModel);
	void setDialog(BackgroundImageInfoGeoreferenceDialog* dialog);

	void keyPressEvent(QKeyEvent* event) override; 
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	void resizeEvent(QResizeEvent*) override;

	std::vector<GcpTableRow>* gcpTable();

private:
	class ImageInfo;

	void setupTransform();
	QPixmap drawModel() override;

	ImageInfo* m_info;
};

#endif // GEOREFERENCEVIEW_H
