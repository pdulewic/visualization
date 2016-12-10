#ifndef CONTROL_PANEL
#define CONTROL_PANEL

#include <QWidget>
#include <QVector3D>
#include <QVector>
#include <QQueue>
#include <QTimer>


class QVBoxLayout;
class QLabel;
class Scene;
class QCheckBox;
class QPushButton;
class Manipulator;
class QTableView;
class QGroupBox;
class QSlider;


class ControlPanel: public QWidget{
    Q_OBJECT

    QVector<QSlider*> control;
    QVBoxLayout* mainLayout;
    QVBoxLayout* sliderLayout;
    QGroupBox* sliders;
    QLabel* tipCoordX;
    QLabel* tipCoordY;
    QLabel* tipCoordZ;
    QCheckBox* trace;
    QPushButton* eraseTrace;
    QCheckBox* manipulatorVisibility;
    QCheckBox* coordinatesVisibility;
    QPushButton* resetObserverPos;
    QPushButton* resetAng;
    QTableView* DHTable;

    QTimer animationTimer;
    QQueue<QVector<float>> animationTrace;

    void setAngles(const QVector<float> &q);
public:
    ControlPanel(QWidget* parent = nullptr);
    void connectWithManipulator(const Manipulator& man);
    void connectWithScene(const Scene* scene);
private slots:
    void resetAngles();
    void animationStep();
public slots:
    void showTipCoordinates(QVector3D position);
    void animateMovement(const QQueue<QVector<float>> &q);
signals:
    void postureChanged();
};

#endif // CONTROL_PANEL

