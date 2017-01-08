#include "inc/octave_connection.h"
#include "inc/constants.h"
#include <QTextStream>
#include <QDialog>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

OctaveConnection::OctaveConnection(QString p, QObject *parent): QProcess(parent), path(p), currentType(""), qSize(0), terminalIsOpen(false){
    connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(readStandardOutput()));

}

void OctaveConnection::loadPartialKinematics(const GLfloat matrix[]){
    QString command = QString("A{%1} = [").arg(++numberOfPartialKinematics);
    for(int i=0; i<4; ++i){
        for(int j=0; j<4; ++j)
            command.append(QString::number(matrix[i+4*j]) + " ");
        command.append(';');
    }
    command.append("];\n");
    write(command.toLocal8Bit());
}

void OctaveConnection::sendKinematicsRequest(const QVector<QVector3D> &zAxis){
    QString command = "Z = [";
    for(const auto &it : zAxis)
        command.append(QString("%1, %2, %3;").arg(it.x()).arg(it.y()).arg(it.z()));
    command.append("];\n");
    write(command.toLocal8Bit());
    write(QString("calculate_DH(Z,A);\n").toLocal8Bit());
}


void OctaveConnection::runOctave(){
    if(path.isEmpty())
        return;
    QStringList arguments;
    arguments << "--no-gui";
    start(path,arguments);
}

void OctaveConnection::resetPartialKinematics(){
    numberOfPartialKinematics = 0;
    // initialize cell array for partial kinematics
    write(QString("A = {};\n").toLocal8Bit());
}

void OctaveConnection::readStandardOutput(){
    QByteArray data = readAllStandardOutput();
    if(terminalIsOpen){
        display->append(QString::fromLocal8Bit(data.data() ) );
        return;
    }
    QTextStream output(data);
    QString buffer;

    if(currentType == ""){
       output >> currentType;
       // on windows data stream ends with additional character
       // WARNING! this code wasn't tested on Mac OS
       #ifdef Q_OS_WIN
       output >> buffer;
       #endif
       if(currentType == "IK"){
           tmpIK.clear(qSize);
       }
       else if(currentType == "TOOSMALL"){
           QVector<std::array<double, 4>> tmp;
           emit newDHTable(tmp,ElementType::Base1);
       }
    }
    if(currentType == "DH"){      // DH table
        while(!output.atEnd()){
            if(tmpDH.cntTable < numberOfPartialKinematics-1){ // there is still some numerical data
                output >> tmpDH.row[tmpDH.cntRow];
                ++tmpDH.cntRow;
                if(tmpDH.cntRow == 4){
                    tmpDH.table.push_back(tmpDH.row);
                    tmpDH.cntRow = 0;
                    ++tmpDH.cntTable;
                }
            }
            else{              // only base type left in buffer
                output >> buffer;
                ElementType type;
                if(buffer == "base1")
                    type = ElementType::Base1;
                else if(buffer == "base2")
                    type = ElementType::Base2;
                else{
                    currentType = "";
                    write(QString("calculate_DH(Z,A);\n").toLocal8Bit());
                    return;
                }
                currentType = "";
                emit newDHTable(tmpDH.table,type);
                tmpDH.clear();
                return;
            }
        }
    }
    else if(currentType == "IK"){     // Inverse kinematics
        while(!output.atEnd()){
            output >> tmpIK.pose[qSize - 1 - tmpIK.cntPose];
            if(output.atEnd())
                return;
            ++tmpIK.cntPose;
            if(tmpIK.cntPose == qSize){
                if(tmpIK.cntPose == 5){  // fanuc
                    QVector<float> fanucPose(tmpIK.pose);
                    fanucPose.push_front(0.0);
                    tmpIK.animation.enqueue(fanucPose);
                }
                else{
                    tmpIK.animation.enqueue(tmpIK.pose);
                }
                ++tmpIK.cntAnimation;
                tmpIK.cntPose = 0;
            }
            if(tmpIK.cntAnimation == IK_ANIMATION_STEPS){
                currentType = "";
                emit newInverseKinematics(tmpIK.animation);
                return;
            }
        }
    }
    else{
        tmpDH.clear();
        currentType = "";
    }
}

void OctaveConnection::sendCommand(){
    if(terminalIsOpen){
        QString command = commandLine->toPlainText() + "\n";
        write(command.toLocal8Bit());
        commandLine->setText("");
    }
}

void OctaveConnection::openTerminal(){
    terminal = new QDialog(qobject_cast<QWidget*>(parent()));
    terminal->setWindowTitle(tr("Terminal"));
    connect(terminal,SIGNAL(destroyed(QObject*)),this,SLOT(setTerminalIsOpenFalse()));
    display = new QTextBrowser;
    display->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    commandLine = new QTextEdit;
    commandLine->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    commandLine->setFixedHeight(40);
    send = new QPushButton(tr("Send"));
    connect(send,SIGNAL(clicked(bool)),this,SLOT(sendCommand()));

    QHBoxLayout* downLayout = new QHBoxLayout;
    downLayout->addWidget(commandLine);
    downLayout->addWidget(send);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(display);
    mainLayout->addLayout(downLayout);

    terminal->setLayout(mainLayout);
    terminalIsOpen = true;
    terminal->show();
}

void OctaveConnection::sendIKRequest(const QVector3D &xd, const QVector<float> &q0, IKDialog::Model type){
    qSize = q0.size();
    QString xdString;
    if(type < 2)
        xdString = QString("[%1; %2]").arg(xd.x()).arg(xd.y());
    else
        xdString = QString("[%1; %2; %3]").arg(xd.x()).arg(xd.y()).arg(xd.z());
    QString q0String {"["};
    for(const auto &x : q0)
        q0String.append(QString("%1 ").arg(x));
    q0String.append("]");

    QString command = "InverseKinematics(" + xdString + "," + q0String + "," + QString::number(type) + ");\n";
    write(command.toLocal8Bit());
}

void IK::clear(int sz){
    pose = QVector<float>(sz);
    animation.clear();
    cntPose = 0;
    cntAnimation = 0;
}

void DH::clear(){
    table.clear();
    cntRow = 0;
    cntTable = 0;
}
