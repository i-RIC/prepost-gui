#ifndef POST3DWINDOWFACELISTSETTINGEDITWIDGET_H
#define POST3DWINDOWFACELISTSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class Post3dWindowFaceListSettingEditWidget;
}

class Post3dWindowFaceSettingContainer;

class Post3dWindowFaceListSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Post3dWindowFaceListSettingEditWidget(QWidget *parent = nullptr);
	~Post3dWindowFaceListSettingEditWidget();

	void setDimensions(int* dims);

	std::vector<Post3dWindowFaceSettingContainer> faces();
	void setFaces(const std::vector<Post3dWindowFaceSettingContainer>& faces);

private slots:
	void addFace();
	void removeFace();
	void setCurrentFace(int row);

private:
	void updateFaceList();
	void saveCurrentFace();

	std::vector<Post3dWindowFaceSettingContainer> m_faces;
	int m_currentRow;
	int m_gridDimensions[3];

	Ui::Post3dWindowFaceListSettingEditWidget *ui;
};

#endif // POST3DWINDOWFACELISTSETTINGEDITWIDGET_H
