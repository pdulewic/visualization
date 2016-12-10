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

#include <GL/gl.h>

class QDialog;
class QTextBrowser;
class QTextEdit;
class QPushButton;


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
    QVector3D previousPosition;
    int numberOfPartialKinematics;
    int qSize;

    bool terminalIsOpen;
    QDialog* terminal;
    QTextBrowser* display;
    QTextEdit* commandLine;
    QPushButton* send;
public:
    enum MatrixType{
        Modelview,         // name in octave workspace: Mv
        View,              // name in octave workspace: V
        PartialKinematics  // name in octave workspace: Ax, where x = 1, 2, 3...
    };

    OctaveConnection(QString p = QString(), QObject* parent = nullptr);
    void loadMatrix(const GLfloat matrix[], MatrixType type);
    void sendKinematicsRequest(const QVector<QVector3D> &zAxis);
    void sendTipPositionRequest(ElementType base);
    void runOctave();
    void resetPartialKinematics();
private slots:
    void readStandardOutput();
    void setTerminalIsOpenFalse() {terminalIsOpen = false; }
    void sendCommand();
public slots:
    void openTerminal();
    void sendIKRequest(const QVector3D &xd, const QVector<float> &q0, IKDialog::Model type);
signals:
    void newTipPosition(const QVector3D position);
    void newDHTable(QVector<std::array<double, 4>>& param, ElementType type);
    void newInverseKinematics(const QQueue<QVector<float>> &q);
};

#endif // OCTAVE_CONNECTION

