#ifndef SCENE
#define SCENE

#include <QGLWidget>
#include <QTimer>
#include <QVector3D>
#include <QList>
#include "constants.h"
#include "dh_parameters.h"

class Manipulator;
class OctaveConnection;

class Scene: public QGLWidget{
    Q_OBJECT

    float posX;
    float posY;
    float rotX;
    float rotY;
    float size;

    bool leftButtonIsClicked;
    bool rightButtonIsClicked;
    QPoint referencePoint;
    QTimer timer;

    const Manipulator* man;
    OctaveConnection* octave;
    QList<QVector3D> tipTrace;
    QVector3D token;
    QVector<float> angles;
    DHParameters* parameters;
    bool isTraceVisible;
    bool isManipulatorVisible;
    bool areCoordinatesVisible;
    bool postureChanged;
    bool isPaintingEnabled;
    bool isTokenVisible;
public:
    Scene(Manipulator* m = nullptr, OctaveConnection* o = nullptr, QWidget* parent = nullptr);
    DHParameters* getParameters() const {return parameters; }
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *e);
    QSize sizeHint() const {return QSize(SCENE_DEFAULT_WIDTH,SCENE_DEFAULT_HEIGHT); }
private slots:
    void periodicEvent();
public slots:
    void addTipPosition(QVector3D pos);
    void setTraceVisible(bool visible = true){ isTraceVisible = visible; }
    void setCoordinatesVisible(bool visible = true){ areCoordinatesVisible = visible; }
    void setManipulatorVisible(bool visible = true){ isManipulatorVisible = visible; }
    void startDisplayingToken(){ isTokenVisible = true; }
    void stopDisplayingToken(){ isTokenVisible = false; }
    void setTokenPosition(const QVector3D &pos){ token = pos;}
    void clearTrace(){ tipTrace.clear(); }
    void resetObserverPosition();
    void setPostureChanged(){ postureChanged = true; }
    void setPaintingEnabled(bool enabled = true){ isPaintingEnabled = enabled; }
    void calculateDH();
signals:
    void tipPositionChanged(QVector3D position);
};

#endif // SCENE

