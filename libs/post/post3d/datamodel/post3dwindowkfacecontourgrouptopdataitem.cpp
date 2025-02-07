#include "post3dwindowkfacecontourgrouptopdataitem.h"

Post3dWindowKFaceContourGroupTopDataItem::Post3dWindowKFaceContourGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowFaceContourGroupTopDataItem(tr("Contours (faceK)"), v4SolutionGrid::Position::KFace, p)
{}
