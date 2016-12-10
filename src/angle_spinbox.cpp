#include "inc/angle_spinbox.h"
#include "inc/constants.h"

AngleSpinBox::AngleSpinBox(QWidget *parent): QDoubleSpinBox(parent) {
    connect(this,SIGNAL(valueChanged(double)),this,SLOT(onValChanged(double)));
    setDecimals(1);
}

void AngleSpinBox::onValChanged(double val){
    int integerVal = static_cast<int>(val * SLIDER_SENSITIVITY);
    emit valueChanged(integerVal);
}

void AngleSpinBox::setValue(int val){
    double doubVal = static_cast<double>(val);
    QDoubleSpinBox::setValue(doubVal / static_cast<double>(SLIDER_SENSITIVITY));
}
