#ifndef CGLWIDGET_H
#define CGLWIDGET_H

#include <QOpenGLWidget>

#include <GL/glu.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

#include <QMap>

struct sBuffer {
    sBuffer(){}
    sBuffer(QVector<GLfloat> &vertices, QOpenGLBuffer vbo){ mVertices=vertices; mVBO=vbo; }
    QVector<GLfloat> mVertices;
    QOpenGLBuffer mVBO;
};

class cGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit cGLWidget(QWidget *parent = 0);

    void setLightPos(const QVector3D lp);



    float scale(float from_min, float from_max, float to_min, float to_max, float val);
    float deg2rad(int deg);


    QMap<QString, sBuffer> mGLBuffers;

    QVector<QOpenGLBuffer> mVBOs;
    QVector<QOpenGLBuffer> mOutlineVBOs;

    QOpenGLBuffer mPointVBO;
    QVector<GLfloat> mPoints;


    void createVBO(QVector<GLfloat> &buffer);
    void createOutlineVBO(QVector<GLfloat> &buffer);

    void updateProjection(bool perspectiveProjection);
    void topView();
    void leftView();
    void rightView();
    void backView();
    void frontView();
    QVector3D lightPos() const;

    void setLightColor(const QColor color);
    QColor lightColor();


    int mCurrFrame=0;

    void destroyVBOs();
    void createPlaneVBO(QVector<GLfloat> &buffer);
    void createTexture(QImage texture);

    int mOpacity=100;

    void setSamplingMode(bool sampling);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);

    void mouseMoveEvent(QMouseEvent *me);
    void keyPressEvent(QKeyEvent *ke);

private:


    bool mPerspectiveProjection;

    bool mSamplingMode;

    QOpenGLBuffer mPlaneVBO;

    QOpenGLTexture *mTexture=0;

    int mCount;

    QOpenGLVertexArrayObject mVAO;



    bool mLeftMouseButtonPressed;
    int mLeftMouseButtonPressCoordX;
    int mLeftMouseButtonPressCoordY;

    GLuint mTextures[10];

    float mRotX;
    float mRotY;

    float mPos2dx;
    float mPos3dy;

    //QVector<QVector<float> > mHeights;

    QOpenGLShaderProgram *mProgram;
    QOpenGLShaderProgram *mPrgOutline;

    int mSavedX;
    int mSavedY;

    //QVector<GLfloat> mVertices;

    QVector3D ray_nds;
    QVector4D ray_clip;

    QVector3D mEye;
    QVector3D mView;
    QVector3D mUp;

    QVector4D mPVertex;

    QMatrix4x4 mProjectionMatrix;

    float mX;
    float mY;
    float mZ;

    QMatrix4x4 mWorldTranslate;
    QMatrix4x4 mWorldRotate;

    void initShaders();
    void initTextures();
    void add(QVector<GLfloat> &b, QVector3D v, const QVector2D tc=QVector2D());
    void cylinder(const float radius, const float length, const int sections);
    bool screenToWorld(int winx, int winy, int z_ndc, QVector3D &ray_world);
signals:
    void glInitialized();
    void mousePressed(QVector3D);
    void mouseMove(QVector3D);



};

#endif // CGLWIDGET_H
