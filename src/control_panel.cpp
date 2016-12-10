#include "inc/control_panel.h"
#include "inc/manipulator.h"
#include "inc/constants.h"
#include <inc/scene.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QGroupBox>
#include <QFrame>
#include <QSlider>

#include <QDebug>

ControlPanel::ControlPanel(QWidget *parent): QWidget(parent), sliders(nullptr) {
    QGroupBox* DHGroup = new QGroupBox(tr("Denavit–Hartenberg parameters"));
    DHTable = new QTableView(DHGroup);
    QFont font;
    font.setFamily("Computer Modern Roman");
    font.setItalic(true);
    DHTable->horizontalHeader()->setFont(font);
    DHTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVBoxLayout* DHLayout = new QVBoxLayout;
    DHLayout->addWidget(DHTable);
    DHGroup->setLayout(DHLayout);

    QGroupBox* viewGroup = new QGroupBox(tr("View"));
    trace = new QCheckBox(tr("Show tip trace"),viewGroup);
    eraseTrace = new QPushButton(tr("Clean tip trace"),viewGroup);
    QHBoxLayout* traceLayout = new QHBoxLayout;
    traceLayout->addWidget(trace);
    traceLayout->addWidget(eraseTrace);

    manipulatorVisibility = new QCheckBox(tr("Show manipulator model"),viewGroup);
    coordinatesVisibility = new QCheckBox(tr("Show coordinate systems"),viewGroup);
    resetObserverPos = new QPushButton(tr("Reset observer position"),viewGroup);
    resetAng = new QPushButton(tr("Reset angles"));
    connect(resetAng,SIGNAL(clicked(bool)),this,SLOT(resetAngles()));
    tipCoordX = new QLabel("x: 0.0");
    tipCoordX->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    tipCoordY = new QLabel("y: 0.0");
    tipCoordY->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    tipCoordZ = new QLabel("z: 0.0");
    tipCoordZ->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QFrame* tipFrame = new QFrame;
    tipFrame->setFrameShape(QFrame::Box);
    QHBoxLayout* frameLayout = new QHBoxLayout;
    frameLayout->addWidget(tipCoordX);
    frameLayout->addWidget(tipCoordY);
    frameLayout->addWidget(tipCoordZ);
    tipFrame->setLayout(frameLayout);
    QHBoxLayout* tipLayout = new QHBoxLayout;
    tipLayout->addWidget(new QLabel(tr("Tip coordinates:")));
    tipLayout->addWidget(tipFrame);
    QHBoxLayout* resLayout = new QHBoxLayout;
    resLayout->addWidget(resetObserverPos);
    resLayout->addWidget(resetAng);

    QVBoxLayout* viewLayout = new QVBoxLayout;
    viewLayout->addLayout(traceLayout);
    viewLayout->addWidget(manipulatorVisibility);
    viewLayout->addWidget(coordinatesVisibility);
    viewLayout->addLayout(resLayout);
    viewLayout->addLayout(tipLayout);
    viewGroup->setLayout(viewLayout);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(DHGroup);
    mainLayout->addWidget(viewGroup);
    setLayout(mainLayout);

    animationTimer.setInterval(SCENE_TIMER_INTERVAL);
    connect(&animationTimer,SIGNAL(timeout()),this,SLOT(animationStep()));
}

void ControlPanel::connectWithManipulator(const Manipulator &man){
    delete sliders;
    control.clear();
    sliderLayout = new QVBoxLayout;
    man.createControlSliders(sliderLayout,control,this);
    sliders = new QGroupBox(tr("Control"));
    sliders->setLayout(sliderLayout);
    mainLayout->addWidget(sliders);
}

void ControlPanel::connectWithScene(const Scene *scene){
    connect(trace,SIGNAL(toggled(bool)),scene,SLOT(setTraceVisible(bool)));
    connect(eraseTrace,SIGNAL(clicked(bool)),scene,SLOT(clearTrace()));
    connect(manipulatorVisibility,SIGNAL(toggled(bool)),scene,SLOT(setManipulatorVisible(bool)));
    connect(coordinatesVisibility,SIGNAL(toggled(bool)),scene,SLOT(setCoordinatesVisible(bool)));
    connect(resetObserverPos,SIGNAL(clicked(bool)),scene,SLOT(resetObserverPosition()));
    trace->setChecked(false);
    manipulatorVisibility->setChecked(true);
    coordinatesVisibility->setChecked(false);
    DHTable->setModel(scene->getParameters());
}

void ControlPanel::resetAngles(){
    for(auto x : control)
        x->setValue(0);
}

void ControlPanel::animationStep(){
    if(animationTrace.isEmpty())
        animationTimer.stop();
    else
        setAngles(animationTrace.dequeue());
}

void ControlPanel::showTipCoordinates(QVector3D position){
    tipCoordX->setText("x: " + QString::number(static_cast<double>(position.x()),'f',TIP_COORD_PRECISION));
    tipCoordY->setText("y: " + QString::number(static_cast<double>(position.y()),'f',TIP_COORD_PRECISION));
    tipCoordZ->setText("z: " + QString::number(static_cast<double>(position.z()),'f',TIP_COORD_PRECISION));
}

void ControlPanel::animateMovement(const QQueue<QVector<float> > &q){
    animationTrace = q;
    animationTimer.start();
}

void ControlPanel::setAngles(const QVector<float> &q){
    int size = control.size();
    if(size != q.size()){
        qDebug() << "Wrong q vector size in set Manipulator::setAngles function!";
        return;
    }
    for(int i=0; i<size; ++i)
        control[i]->setValue(q[i]*SLIDER_SENSITIVITY);
}
