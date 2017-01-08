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

OctaveConnection::OctaveConnection(QString p, QObject *parent): QProcess(parent), path(p), qSize(0), terminalIsOpen(false){
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
    QTextStream output(data);
    QString buffer;
    output >> buffer;
    if(buffer == "qd"){
        output >> buffer;
        if(buffer != "="){
            qDebug() << "error occured while parsing octave output";
            return;
        }
        QQueue<QVector<float>> trace;
        for(int j=0; j<IK_ANIMATION_STEPS; ++j){
            float tmp;
            QVector<float> q(qSize);
            for(int i=qSize-1; i>=0; --i){
                output >> tmp;
                q[i] = tmp;
            }
            if(qSize == 5) // temporary fanuc detection
                q.push_front(0);
            trace.enqueue(q);
        }
        emit newInverseKinematics(trace);

    }
    else if(buffer == "TOOSMALL"){
        QVector<std::array<double, 4>> tmp;
        emit newDHTable(tmp,ElementType::Base1);
    }
    else if(buffer == "DH"){
        output >> buffer;
        output >> buffer;

        QVector<std::array<double, 4>> params;
        for(int i=0; i<numberOfPartialKinematics-1; ++i){
            std::array<double, 4> tmp;
            for(int j=0; j<4; ++j)
                output >> tmp[j];
            params.push_back(tmp);
        }
        output >> buffer;
        ElementType type;
        if(buffer == "base1")
            type = ElementType::Base1;
        else if(buffer == "base2")
            type = ElementType::Base2;
        else{
            write(QString("calculate_DH(Z,A);\n").toLocal8Bit());
            return;
        }
        emit newDHTable(params,type);
    }
    else if(terminalIsOpen){
        display->append(QString::fromLocal8Bit(data.data() ) );
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
