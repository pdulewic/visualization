#ifndef OCTAVE_CONNECTION
#define OCTAVE_CONNECTION

#include <QProcess>
#include <QString>
#include <QVector>
#include <QVector3D>
#include <QQueue>
#include <array>
#include "element_type.h"
#include "ik_dialog.h"
#include "octave_terminal.h"

#include <GL/gl.h>

#include <QDebug>

class QDialog;
class QTextBrowser;
class QTextEdit;
class QPushButton;

struct IK{
    QVector<float> pose;
    QQueue<QVector<float>> animation;
    int cntPose;
    int cntAnimation;
    void clear(int sz=0);
    IK(){ clear(); }
};

struct DH{
    std::array<double, 4> row;
    QVector<std::array<double, 4>> table;
    int cntRow;
    int cntTable;
    void clear();
    DH(){clear(); }
};


/*!
 * \brief Class responsible for communication with Octave
 *
 * This class runs Octave as a child process, in order to
 * perform calculations on matrices. Matrices are represented
 * by GLFloat arrays. Existing QMatrix4x4 class was not used
 * because it did not fully meet the requirements.
 * Overloaded operator << returns text in incompatible
 * format. Also all member functions performing calculations
 * are unnecessary, when efficient octave alghoritms are
 * ready to use.
 */

class OctaveConnection: public QProcess{
    Q_OBJECT

    QString path;
    QString currentType;
    QString lastErrorMessage;
    //QVector3D previousPosition;
    int numberOfPartialKinematics;
    int qSize;

    bool terminalIsOpen;
    bool errorFlag;
    OctaveTerminal* terminal;
   /* QTextBrowser* display;
    QTextEdit* commandLine;
    QPushButton* send;*/

    DH tmpDH;
    IK tmpIK;
public:
    OctaveConnection(QString p = QString(), QObject* parent = nullptr);
    void loadPartialKinematics(const GLfloat matrix[]);
    void sendKinematicsRequest(const QVector<QVector3D> &zAxis);
    void runOctave();
    void closeOctave();
    void resetPartialKinematics();
private slots:
    void readStandardOutput();
    void readStandardError();
    void restartAfterCrash(QProcess::ProcessState newState);
    void callMeWhenFinished(){ runOctave(); }
    void setTerminalIsOpenFalse() {terminalIsOpen = false; }
    void sendCommand(const QString& command);
public slots:
    void openTerminal();
    void sendIKRequest(const QVector3D &xd, const QVector<float> &q0, IKDialog::Model type);
signals:
    void newDHTable(QVector<std::array<double, 4>>& param, ElementType type);
    void newInverseKinematics(const QQueue<QVector<float>> &q);
    void newText(const QString& text);
};

#endif // OCTAVE_CONNECTION

