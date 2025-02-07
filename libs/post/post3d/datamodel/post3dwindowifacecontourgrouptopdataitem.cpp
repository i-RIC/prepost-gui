#include "post3dwindowifacecontourgrouptopdataitem.h"

Post3dWindowIFaceContourGroupTopDataItem::Post3dWindowIFaceContourGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowFaceContourGroupTopDataItem(tr("Contours (faceI)"), v4SolutionGrid::Position::IFace, p)
{}
