#ifndef BACKGROUNDIMAGEINFO_SETACTORPROPERTYCOMMAND_H
#define BACKGROUNDIMAGEINFO_SETACTORPROPERTYCOMMAND_H

#include "../backgroundimageinfo.h"

#include <QUndoCommand>

class BackgroundImageInfo::SetActorPropertyCommand : public QUndoCommand
{
public:
	SetActorPropertyCommand(double posx, double posy, double scale, double theta, BackgroundImageInfo* info);

	void redo() override;
	void undo() override;

private:
	void applySetting(double posx, double posy, double scale, double theta);

	double m_newTranslateX;
	double m_newTranslateY;
	double m_newScale;
	double m_newTheta;

	double m_oldTranslateX;
	double m_oldTranslateY;
	double m_oldScale;
	double m_oldTheta;

	BackgroundImageInfo* m_info;
};

#endif // BACKGROUNDIMAGEINFO_SETACTORPROPERTYCOMMAND_H
