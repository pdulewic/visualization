#ifndef ELEM_TYPE
#define ELEM_TYPE

#include <QString>
#include <QObject>
#include <QIcon>
#include <QList>

//order:  movable elements, variable length elements, normal elements

const int NUMBER_OF_ELEMENTS = 9;

enum class ElementType { Base1, Base2, Joint1, Joint2, Joint3, Joint4, Link1, Link2, Tip, xAxis, zAxis, Token };

struct ElementData{
    QString name;
    QString icon;
    double positiveAngleLimit;
    double negativeAngleLimit;
    ElementData(QString na, QString i ,double p = 0.0, double n = 0.0): name(na), icon(i), positiveAngleLimit(p), negativeAngleLimit(n) {}
};

extern QList<ElementData> ElementArray;

/* initialization must occur after loading translator, to make sure all
 * objects will be created with names in proper language */
void initializeElementArray();

#endif // ELEM_TYPE

