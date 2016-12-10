#ifndef ELEMENT
#define ELEMENT

#include "model_manager.h"
#include "constants.h"
#include <QObject>


class OctaveConnection;


class Element: public QObject{
    Q_OBJECT

    ElementType type;
    int material;
    float rotation;
    float maxPositiveAngle;
    float maxNegativeAngle;
    /*!
     * \brief Variable parameter
     *
     * In movable elements like joints, this parameter
     * represents angle. If the element is a link, this
     * is a length value instead. In both cases, render
     * function need this parameter to execute properly.
     */
    float variableParameter;
public:
    Element(ElementType t, int m, float r, float mpa=0.0, float mna=0.0, float v=0.0);
    void render() const;
    void calculateZAxis(OctaveConnection* octave, QVector<QVector3D> &axis) const;
    bool isMovable() const;

    ElementType getType() const {return type; }
    int getMaterial() const {return material; }
    float getRotation() const {return rotation; }
    float getMaxPosAngle() const {return maxPositiveAngle; }
    float getMaxNegAngle() const {return maxNegativeAngle; }
    float getVariableParameter() const {return variableParameter; }

    void setType(ElementType t){type = t; }
    void setMaterial(int m){material = m; }
    void setRotation(float r){rotation = r;}
    void setMPA(float mpa){ maxPositiveAngle = mpa; }
    void setMNA(float mna){ maxNegativeAngle = mna; }
    void setVariableParameter(float v){variableParameter = v; }
public slots:
    void setAngle(int ang) {variableParameter = static_cast<float>(ang) / SLIDER_SENSITIVITY;}
};

#endif // ELEMENT

