#include "inc/ik_dialog.h"
#include "inc/manipulator.h"
#include "inc/constants.h"
#include <QString>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

IKDialog::IKDialog(Manipulator *man, QWidget *parent): QDialog(parent), manipulator(man){
    setWindowTitle(tr("Inverse kinematics"));
    manipChoice = new QComboBox;
    manipChoice->addItem(tr("Double planar pendulum"));
    manipChoice->addItem(tr("Triple planar pendulum"));
    manipChoice->addItem("SCARA");
    manipChoice->addItem("Fanuc");

    int currentManipType = manipulator->countMovableElements();  // temporary detection - rigid but easy
    if(currentManipType == 6)
        currentManipType = 3;
    else
        currentManipType -= 2;
    manipChoice->setCurrentIndex(currentManipType);
    QHBoxLayout* upperLayout = new QHBoxLayout;
    upperLayout->addWidget(new QLabel(tr("Manipulator type:")));
    upperLayout->addWidget(manipChoice);
    xCoord = new QDoubleSpinBox;
    xCoord->setRange(-IK_RANGE,IK_RANGE);
    connect(xCoord,SIGNAL(valueChanged(double)),this,SLOT(sendNewPosition()));
    yCoord = new QDoubleSpinBox;
    yCoord->setRange(-IK_RANGE,IK_RANGE);
    connect(yCoord,SIGNAL(valueChanged(double)),this,SLOT(sendNewPosition()));
    zCoord = new QDoubleSpinBox;
    zCoord->setRange(-IK_RANGE,IK_RANGE);
    connect(zCoord,SIGNAL(valueChanged(double)),this,SLOT(sendNewPosition()));
    QHBoxLayout* midLayout = new QHBoxLayout;
    midLayout->addWidget(new QLabel("x:"));
    midLayout->addWidget(xCoord);
    midLayout->addWidget(new QLabel("y:"));
    midLayout->addWidget(yCoord);
    midLayout->addWidget(new QLabel("z:"));
    midLayout->addWidget(zCoord);
    QPushButton* execButton = new QPushButton(tr("Execute"));
    connect(execButton,SIGNAL(clicked(bool)),this,SLOT(execute()));
    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect(closeButton,SIGNAL(clicked(bool)),this,SLOT(close()));
    QHBoxLayout* downLayout = new QHBoxLayout;
    downLayout->addStretch();
    downLayout->addWidget(execButton);
    downLayout->addWidget(closeButton);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(upperLayout);
    layout->addLayout(midLayout);
    layout->addLayout(downLayout);
    setLayout(layout);
}

void IKDialog::execute(){
    QVector3D tipCoordinates {static_cast<float>(xCoord->value()), static_cast<float>(yCoord->value()), static_cast<float>(zCoord->value())};
    QVector<float> angles = manipulator->getAngles();
    if(manipChoice->currentIndex() == FANUC)
        angles.pop_back();
    emit execIKCommand(tipCoordinates,angles,static_cast<Model>(manipChoice->currentIndex()));
}

void IKDialog::sendNewPosition(){
    QVector3D tipCoordinates {static_cast<float>(xCoord->value()), static_cast<float>(yCoord->value()), static_cast<float>(zCoord->value())};
    emit newPosition(tipCoordinates);
}
