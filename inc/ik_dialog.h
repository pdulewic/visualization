#ifndef IK_DIALOG
#define IK_DIALOG

#include <QDialog>
#include <QVector>
#include <QVector3D>
#include <QString>

class QDoubleSpinBox;
class QComboBox;
class Manipulator;

class IKDialog: public QDialog{
    Q_OBJECT

    QDoubleSpinBox* xCoord;
    QDoubleSpinBox* yCoord;
    QDoubleSpinBox* zCoord;
    QComboBox* manipChoice;

    Manipulator* manipulator;
public:
    IKDialog(Manipulator* man, QWidget* parent = nullptr);
    enum Model{DOUBLE_PENDULUM, TRIPLE_PENDULUM, SCARA, FANUC};
private slots:
    void execute();
    void sendNewPosition();
signals:
    void execIKCommand(const QVector3D &xd, const QVector<float> &q0, IKDialog::Model type);
    void newPosition(const QVector3D &pos);
};

#endif // IK_DIALOG

