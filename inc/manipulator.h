#ifndef MANIPULATOR
#define MANIPULATOR

#include "element.h"
#include <QList>
#include <QPair>
#include <QFile>
#include <QObject>

class QVBoxLayout;
class QListWidget;
class ControlPanel;
class QSlider;

class Manipulator: public QObject{
    Q_OBJECT

    QList<Element*> kinematicChain;

    void eraseKinematicChain();
public:
    Manipulator(QObject* parent = nullptr): QObject(parent) {}
    void render() const;
    void calculateZ(OctaveConnection* octave, QVector<QVector3D> &axis) const;
    void createControlSliders(QVBoxLayout* layout, QVector<QSlider *> &vec, const ControlPanel* panel) const;
    void copyKinematicChainTo(QList<Element*>& copy) const;
    void copyKinematicChainFrom(const QList<Element*>& source);
    bool loadFromXML(QFile* file);
    void saveToXML(QFile* file);
    void createNew();
    int countMovableElements() const;
    QVector<float> getAngles() const;
    QVector<bool> getAngleTypes() const;
signals:
    void kinematicChainChanged();
    void kinematicChainAboutToChange(bool enable);

};

#endif // MANIPULATOR

