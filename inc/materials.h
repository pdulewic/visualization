#ifndef MATERIALS_H
#define MATERIALS_H

#include <GL/gl.h>
#include <QVector>
#include <QList>
#include <QString>
#include <QObject>
#include <initializer_list>

/*
 * Materials are represented by int value
 * which can be use as a data table
 * index. To iterate over all materials,
 * a const int NUMBER_OF_MATERIALS is needed
 */

namespace Materials{

const int NUMBER = 7;

class Properties{
    QString name;
    QVector<GLfloat> v;
public:
    Properties(QString n, std::initializer_list<GLfloat> l): name(n), v(l) {}
    const GLfloat* getAmbient() const{
        return &v[0];
    }
    const GLfloat* getDiffuse() const{
        return &v[4];
    }
    const GLfloat* getSpecular() const{
        return &v[8];
    }
    GLfloat getShininess() const{
        return v[12];
    }
    QString getName() const{
        return name;
    }
};


extern QList<Properties> data;

/* initialization must occur after loading translator, to make sure all
 * objects will be created with names in proper language */
void initializeMaterials();

}

#endif // MATERIALS_H
