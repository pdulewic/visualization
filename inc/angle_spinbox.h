#ifndef ANGLE_SPINBOX
#define ANGLE_SPINBOX

#include <QDoubleSpinBox>

class AngleSpinBox: public QDoubleSpinBox{
    Q_OBJECT

public:
    AngleSpinBox(QWidget* parent = nullptr);
private slots:
    void onValChanged(double val);
public slots:
    void setValue(int val);
signals:
    void valueChanged(int val);

};

#endif // ANGLE_SPINBOX

