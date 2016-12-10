#include "inc/scene.h"
#include "inc/manipulator.h"
#include "inc/octave_connection.h"
#include <GL/glu.h>
#include <QMouseEvent>

#include <QDebug>
#include "inc/materials.h"

Scene::Scene(Manipulator *m, OctaveConnection *o, QWidget *parent): QGLWidget(parent), posX(0.0),posY(0.0),rotX(0.0),rotY(0.0),
                                                                    size(1.0), man(m), octave(o), postureChanged(false),
                                                                    isPaintingEnabled(true), isTokenVisible(false){
    parameters = new DHParameters(this,this);
    connect(octave,SIGNAL(newDHTable(QVector<std::array<double,4> >&,ElementType)),parameters,SLOT(setDHParameters(QVector<std::array<double,4> >&,ElementType)));
    leftButtonIsClicked = false;
    rightButtonIsClicked = false;
    timer.setInterval(SCENE_TIMER_INTERVAL);
    connect(&timer,SIGNAL(timeout()),this,SLOT(periodicEvent()));
    timer.start();
}

void Scene::initializeGL(){
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    //depth test settings
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //hints for quality image

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_NORMALIZE );
}

void Scene::paintGL(){
    if(!isPaintingEnabled)
        return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -7.0f);
    // transformations connected with observer position
    glTranslatef(posX,posY,0.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(size,size,size);

    GLfloat viewMatrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);

    if(isManipulatorVisible)
        man->render();

    // if the user has changed the angle at any joint, a new
    // kinematics must be calculated by octave
    if(postureChanged){
        postureChanged = false;
        angles = man->getAngles();
        octave->loadMatrix(viewMatrix,OctaveConnection::View);
        GLfloat modelviewMatrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
        octave->loadMatrix(modelviewMatrix,OctaveConnection::Modelview);
        octave->sendTipPositionRequest(parameters->getBaseType());
    }

    if(areCoordinatesVisible){
        glLoadMatrixf(viewMatrix);
        parameters->adjustCoordinateSystem();
        parameters->render(angles);
    }

    if(isTraceVisible){
        glLoadMatrixf(viewMatrix);
        parameters->adjustCoordinateSystem();
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        for(auto it = tipTrace.begin(); it+1 < tipTrace.end();){
            if(QVector3D(*it - *(it+1)).length() > MAX_TIP_TRACE_STEP){
                ++it;
                continue;
            }
            glVertex3f(it->x(), it->y(), it->z());
            ++it;
            glVertex3f(it->x(), it->y(), it->z());
        }
        glEnd();
        glEnable( GL_LIGHTING );
    }
    if(isTokenVisible){
        glLoadMatrixf(viewMatrix);
        parameters->adjustCoordinateSystem();
        glTranslatef(token.x(),token.y(),token.z());
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, Materials::data[TOKEN_MATERIAL_ID].getAmbient() );
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, Materials::data[TOKEN_MATERIAL_ID].getDiffuse() );
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, Materials::data[TOKEN_MATERIAL_ID].getSpecular() );
        glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, Materials::data[TOKEN_MATERIAL_ID].getShininess() );
        ModelManager::renderModel(ElementType::Token);
    }
}


void Scene::resizeGL(int w, int h){
    if(h == 0)
        h = 1;
    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<GLfloat>(w) / static_cast<GLfloat>(h), 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Scene::mousePressEvent(QMouseEvent *e){
    if(e->button() == Qt::LeftButton)
        leftButtonIsClicked = true;
    else if(e->button() == Qt::RightButton)
        rightButtonIsClicked = true;
    referencePoint = mapFromGlobal(QCursor::pos());
}


void Scene::mouseReleaseEvent(QMouseEvent *){
    leftButtonIsClicked = false;
    rightButtonIsClicked = false;
}


void Scene::wheelEvent(QWheelEvent *e){
    if(e->delta() > 0){
        size *= (1.0 + SCROLL_SENSITIVITY);
        if(size > MAX_SCENE_ZOOM)
            size = MAX_SCENE_ZOOM;
    }
    else{
        size *= (1.0 - SCROLL_SENSITIVITY);
        if(size < MIN_SCENE_ZOOM)
            size = MIN_SCENE_ZOOM;
    }
}




void Scene::periodicEvent(){
    if(leftButtonIsClicked){        //grab
        QPoint tmp = referencePoint - mapFromGlobal(QCursor::pos());
        posX += static_cast<float>(-tmp.x())/(MOUSE_SENSITIVITY);
        posY += static_cast<float>(tmp.y())/(MOUSE_SENSITIVITY);
        referencePoint = mapFromGlobal(QCursor::pos());
    }
    else if(rightButtonIsClicked){  //rotate
        QPoint tmp = referencePoint - mapFromGlobal(QCursor::pos());
        rotX += static_cast<float>(-tmp.y());
        rotY += static_cast<float>(-tmp.x());
        referencePoint = mapFromGlobal(QCursor::pos());
    }
    update();
}

void Scene::addTipPosition(QVector3D pos){
    if(pos != QVector3D(0.0, 0.0, 0.0))
        tipTrace.push_back(pos);
}


void Scene::resetObserverPosition(){
    rotX = 0.0;
    rotY = 0.0;
    posX = 0.0;
    posY = 0.0;
    size = 1.0;
}

void Scene::calculateDH(){
    // we don't want any rendering when OpenGL is being
    // used to calculate z axes coordinates
    QVector<QVector3D> zAxis;
    octave->resetPartialKinematics();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    man->calculateZ(octave,zAxis);
    parameters->setAngleTypes(man->getAngleTypes());
    octave->sendKinematicsRequest(zAxis);
    // angles back to the default position, so probably posture changes.
    // forcing angles to reload is necessary anyway
    postureChanged = true;

}
