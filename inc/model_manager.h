#ifndef MODEL_MANAGER
#define MODEL_MANAGER

#include <QVector>
#include <QString>
#include "model.h"
#include "element_type.h"


class ModelManager{
    static QVector<Model*> models;
public:
    static void initialize();
    /*!
     * \brief Perform rotations on current OpenGL coordinate system
     *
     * Function rotates the current coordinate system, so that its
     * z and x axes coincides with the z and x axes of the element type
     * \pre current coordinate system is rotated by default (0, 0, 0)
     * \param type - element to adjust the axes
     */
    static void adjustCoordinateSystem(ElementType type);
    static bool renderModel(ElementType type, float variableParameter = 0.0);
};


#endif // MODEL_MANAGER

