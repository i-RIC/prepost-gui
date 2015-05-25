#ifndef GRIDRELATEDCONDITIONDIMENSIONTIMESLIDERSELECTWIDGET_H
#define GRIDRELATEDCONDITIONDIMENSIONTIMESLIDERSELECTWIDGET_H

#include "../base/gridrelatedconditiondimensionselectwidget.h"

class QAction;
class QSlider;
class QLabel;
class QTimer;

class GridRelatedConditionDimensionTimeSliderSelectWidget : public GridRelatedConditionDimensionSelectWidget
{
	Q_OBJECT

private:
	enum RunMode {
		NotRunning,
		Running
	};

public:
	const static int SLIDERWIDTH_MAX = 100;
	explicit GridRelatedConditionDimensionTimeSliderSelectWidget(GridRelatedConditionDimensionContainer* container, QWidget* parent);
	~GridRelatedConditionDimensionTimeSliderSelectWidget();

	/// Returns the index of current step
	int currentStepIndex() const;
	/// Returns the label of current index
	const QString currentStepLabel() const;
	const QString stepLabel(int index) const;

public slots:
	/// Step to next step
	void stepForward();
	/// Step to previous step
	void stepBackward();
	/// Step to the first step
	void stepFirst();
	/// Step to the last step
	void stepLast();
	/// Start animation in slowmotion
	void startSlowmotionAnimation();
	/// Stop animation
	void stopAnimation();
	/// Show Dialog to edit slowmotion speed
	void editSlowmotionSpeed();

private slots:
	void handleSliderMove(int val);
	void handleSliderRelease();
	void handleSlideValueChange(int val);
	void handleTimerTimeout();

signals:
//	void indexChanged(unsigned int index) override;

protected:
	void doApplyValues() override;
	void doSetCurrentIndex(int newIndex) override;

private:
	void disableSteppingActions(bool disable = true);
	void animationStep();
	void updateStartButtonIcon();
	void updateStepLabel(const QString& newLabel);
	bool m_followLastStep;

public:
	/// Menu that has actions related to animation control
	class AnimationActions : public QObject
	{
	public:
		/// Constructor
		AnimationActions(QObject* parent);
		QAction* actionStepBackward;
		QAction* actionStepForward;
		QAction* actionStepFirst;
		QAction* actionStepLast;
		QAction* actionStartAnimation;
		QAction* actionStopAnimation;
		QAction* actionEditSpeed;
	};
private:
	AnimationActions* m_animationActions;
	QLabel* m_titleLabel;
	QSlider* m_slider;
	QLabel* m_currentLabel;
	QTimer* m_timer;
	RunMode m_runMode;
	double m_stepInterval;
	double m_fastInterval;
	double m_slowInterval;

	bool m_timeouted;
	bool m_rendered;

	bool m_isTime;
};

#endif // GRIDRELATEDCONDITIONDIMENSIONTIMESLIDERSELECTWIDGET_H
