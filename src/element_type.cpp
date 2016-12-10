#include "inc/element_type.h"
#include "inc/constants.h"

#include <QDebug>

QList<ElementData> ElementArray;

void initializeElementArray()
{
    ElementArray = {ElementData(QObject::tr("Base 1"), ":/icons/base.png", 90.0, 90.0),
                    ElementData(QObject::tr("Base 2"), ":/icons/base.png", 180.0, 180.0),
                    ElementData(QObject::tr("Joint 1"), ":/icons/joint.png", 90.0, 90.0),
                    ElementData(QObject::tr("Joint 2"), ":/icons/joint.png", 180.0, 180.0),
                    ElementData(QObject::tr("Joint 3"), ":/icons/joint.png", MAX_LINK_LENGTH, 0.0),
                    ElementData(QObject::tr("Joint 4"), ":/icons/joint.png", 180.0, 180.0),
                    ElementData(QObject::tr("Link 1"), ":/icons/link.png"),
                    ElementData(QObject::tr("Link 2"), ":/icons/link.png"),
                    ElementData(QObject::tr("Tip"), ":/icons/tip.png")};
}
