#include "cglwidget.h"
#include <QDebug>
#include <QtOpenGL>

#include <qmath.h>
#include "utils.h"

#include <locale.h>

#include <QString>

cGLWidget::cGLWidget(QWidget *parent) :
    QOpenGLWidget(parent), mSavedX(0), mSavedY(0), mLeftMouseButtonPressed(false),
    mProgram(new QOpenGLShaderProgram), mPrgOutline(new QOpenGLShaderProgram), mEye(QVector3D(0,0,50)), mView(QVector3D(0,0,0)), mUp(QVector3D(0,1,0)), mPerspectiveProjection(false),
    mSamplingMode(true)
{
    setMouseTracking(true);

    QSurfaceFormat format;

    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 0);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

    format.setSamples(4);
    setFormat(format);
}

float cGLWidget::scale(float from_min, float from_max, float to_min, float to_max, float val)
{
    return (to_max - to_min)*(val - from_min) / (from_max - from_min) + to_min;
}

bool cGLWidget::screenToWorld(int winx, int winy, int z_ndc, QVector3D &pw)
{
    // define the plane
    sPlane plane(QVector3D(0,0,1), QVector3D(0,0,0));

    QMatrix4x4 modelViewMatrix;

    QVector4D ray_clip((2.0f * winx) / width() - 1.0f, 1.0f - (2.0f * winy) / height(), z_ndc, 1.0);
    QVector4D ray_eye = mProjectionMatrix.inverted() * ray_clip;

    ray_eye.setZ(-1);
    ray_eye.setW(0);

    modelViewMatrix.lookAt(mEye, mView, mUp);
    QVector3D rw = QVector4D(modelViewMatrix.inverted() * ray_eye).toVector3D();

    float c, d = QVector3D::dotProduct(plane.n, plane.p);

    QVector3D rayDir = (mView - mEye).normalized();

    if(mPerspectiveProjection) {
        c = QVector3D::dotProduct(plane.n, mEye) - d;
    } else {
        rw += mEye;
        c = QVector3D::dotProduct(plane.n, rw) - d;
    }

    if(c < 0) { return false; } // we are behind the plane

    float a;
    if(mPerspectiveProjection) {
        a = QVector3D::dotProduct(plane.n, rw);
    } else {
        a = QVector3D::dotProduct(plane.n, rayDir);
    }

    if(a==0) { return false; } // ray is parallel to the plane

    float t = c / a;

    if(t==0) return false;

    if(mPerspectiveProjection) {
        pw = mEye - rw * t;
    } else {
        pw = rw - rayDir * t;
    }

    qd << "ray clip" << ray_clip << "ray eye" << ray_eye << "mEye" << mEye << rw.normalized();
    qd << "ray_world" << rw << "point in world" << pw << "rw*t" << rw*t << "t" << t;

    return true;
}

void cGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.5, 0.8, 0.5, 0.5);

    qd << "Supported shading language version"  << QString((char*)glGetString(GL_VENDOR))<< QString((char*)glGetString(GL_RENDERER)) << QString((char*)glGetString(GL_VERSION)) << QString((char*)glGetString(GL_EXTENSIONS)) << QString((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    qDebug() << "Initializing shaders...";
    initShaders();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_FLAT);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping ( NEW )

    glEnable(GL_MULTISAMPLE); // qt surface format sets the number of sampling

    //glClearDepth(1.0f); // Depth Buffer Setup
    //glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    createTexture(QImage(":/grid.fw.png"));

    installEventFilter(this);


    QVector<GLfloat> data;

    add(data, QVector3D(25,25,-0.1), QVector2D(1,0));
    add(data, QVector3D(-25,25,-0.1), QVector2D(0,0));
    add(data, QVector3D(-25,-25,-0.1), QVector2D(0,1));
    add(data, QVector3D(25,-25,-0.1), QVector2D(1,1));


    createPlaneVBO(data);

    emit(glInitialized());
}

void cGLWidget::createTexture(QImage texture)
{
    if(mTexture) delete mTexture;
    mTexture = new QOpenGLTexture(texture);
    mTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    mTexture->setMagnificationFilter(QOpenGLTexture::Linear);
}

void cGLWidget::initShaders()
{
    // Overriding system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compiling vertex shader
    if (!mProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vert"))
    {
        qd << "error compiling vertex shader";
        return;
    }

    // Compiling geometry shader
    if (!mProgram->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/gshader.geom"))
    {
        qd << "error compiling vertex shader";
        close();
    }

    // Compiling fragment shader
    if (!mProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.frag"))
    {
        qd << "error compiling fragment shader";
        close();
    }

    mProgram->bindAttributeLocation("qt_vertex", 0);
    mProgram->bindAttributeLocation("qt_normal", 1);

    // Linking shader pipeline
    if (!mProgram->link())
        close();

    // Binding shader pipeline for use
    if (!mProgram->bind())
        close();

    mProgram->release();

    if(!mPrgOutline->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/outline.vert"))
    {
        qd << "error compiling vertex shader";
        close();
    }
    if (!mPrgOutline->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/outline.frag"))
    {
        qd << "error compiling fragment shader";
        close();
    }
    mPrgOutline->bindAttributeLocation("qt_vertex", 0);
    mPrgOutline->bindAttributeLocation("qt_textureCoords", 1);

    // Linking shader pipeline
    if (!mPrgOutline->link())
        close();

    // Binding shader pipeline for use
    if (!mPrgOutline->bind())
        close();

    mPrgOutline->release();

    // Restore system locale
    setlocale(LC_ALL, "");
}

void cGLWidget::add(QVector<GLfloat> &b, QVector3D v, const QVector2D tc)
{
    b.append(v.x());
    b.append(v.y());
    b.append(v.z());
    b.append(tc.x());
    b.append(tc.y());
}

void cGLWidget::destroyVBOs()
{
    mPoints.clear();
    mPointVBO.destroy();

    for(int i=0; i<mVBOs.size(); i++) {
        mVBOs[i].destroy();
    }
    mVBOs.clear();

    for(int i=0; i<mOutlineVBOs.size(); i++) {
        mOutlineVBOs[i].destroy();
    }
    mOutlineVBOs.clear();
    update();
}

void cGLWidget::createPlaneVBO(QVector<GLfloat> &buffer)
{
    if(!buffer.size()) {
        return;
    }

    qd << mPlaneVBO.create();

    qd << mPlaneVBO.bind();
    mPlaneVBO.allocate(buffer.constData(), buffer.size() * sizeof(GLfloat));

    mPlaneVBO.release();

}

void cGLWidget::createVBO(QVector<GLfloat> &buffer)
{
    if(!buffer.size()) {
        return;
    }

    QOpenGLBuffer vbo;

    vbo.create();
    vbo.bind();
    vbo.allocate(buffer.constData(), buffer.size() * sizeof(GLfloat));

    vbo.release();
    mVBOs.append(vbo);
}

void cGLWidget::createOutlineVBO(QVector<GLfloat> &buffer)
{
    if(!buffer.size()) {
        return;
    }

    QOpenGLBuffer vbo;

    vbo.create();
    vbo.bind();
    vbo.allocate(buffer.constData(), buffer.size() * sizeof(GLfloat));

    vbo.release();
    mOutlineVBOs.append(vbo);
}

void cGLWidget::topView()
{
    mEye = QVector3D(0,60,0);
    mView = QVector3D(0,0,0);
    mUp = QVector3D(0,0,-1);

    update();
}

void cGLWidget::leftView()
{
    mEye = QVector3D(60,0,0);
    mView = QVector3D(0,0,0);
    mUp = QVector3D(0,1,0);

    update();
}

void cGLWidget::rightView()
{
    mEye = QVector3D(-60,0,0);
    mView = QVector3D(0,0,0);
    mUp = QVector3D(0,1,0);

    update();
}

void cGLWidget::frontView()
{
    mEye = QVector3D(0,0,60);
    mView = QVector3D(0,0,0);
    mUp = QVector3D(0,1,0);

    update();
}

void cGLWidget::backView()
{
    mEye = QVector3D(60,0,60);
    mView = QVector3D(0,0,0);
    mUp = QVector3D(0,1,0);

    update();
}

void cGLWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w)
    Q_UNUSED(h)
    updateProjection(mPerspectiveProjection);
}

void cGLWidget::updateProjection(bool perspectiveProjection)
{
    int w=width(), h=height();

    float aspect = (float) w / (float) h;

    float os = 25.;

    mProjectionMatrix.setToIdentity();
    if(perspectiveProjection) {
        mProjectionMatrix.perspective(45.0f, aspect, 0.01f, 1000.0f);
    } else {
        if(w <= h)  {
            mProjectionMatrix.ortho(-os, os, -os/aspect, os/aspect, 0.01f, 1000.0f);
        } else {
            mProjectionMatrix.ortho(-os*aspect, os*aspect, -os, os, 0.01f, 1000.0f);
        }
    }
    mPerspectiveProjection = perspectiveProjection;
}



void cGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glShadeModel(GL_FLAT);

    QOpenGLFunctions *f=0;
    QMatrix4x4 transition;

    transition.lookAt(mEye, mView, mUp);

    // draw the background plane
    if(!mPlaneVBO.isCreated()) {
        mPlaneVBO.create();
    }

    mPlaneVBO.bind();
    if(mPlaneVBO.size() > 0) {

        mTexture->bind();

        mPrgOutline->bind();
        mPrgOutline->setUniformValue("qt_opacity", mOpacity);
        mPrgOutline->setUniformValue("qt_modelViewMatrix", transition);
        mPrgOutline->setUniformValue("qt_projectionMatrix", mProjectionMatrix);
        f = QOpenGLContext::currentContext()->functions();
        f->glEnableVertexAttribArray(0);
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
        f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        glDrawArrays( GL_QUADS, 0,  mPlaneVBO.size() / 5 / sizeof(GLfloat) );

        mTexture->release();

        mPrgOutline->release();
    }
    mPlaneVBO.release();

    // draw sampled points
    if(!mPointVBO.isCreated()) {
        mPointVBO.create();
    }

    mPointVBO.bind();
    if(mPointVBO.size() > 0) {
        mPrgOutline->bind();
        mPrgOutline->setUniformValue("qt_opacity", 100);
        f = QOpenGLContext::currentContext()->functions();
        f->glEnableVertexAttribArray(0);
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glPointSize(5);
        glDrawArrays( GL_POINTS, 0,  mPointVBO.size() / 3 / sizeof(GLfloat) );
        mPrgOutline->release();
    }
    mPointVBO.release();


    if(!mVBOs.size() || mCurrFrame >= mVBOs.size()) {
        return;
    }


    mProgram->bind();
    mProgram->setUniformValue("qt_modelViewMatrix", transition);
    mProgram->setUniformValue("qt_projectionMatrix", mProjectionMatrix);

    // each vbo contains the current frame to be rendered
    mVBOs[mCurrFrame].bind();

    f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glLineWidth( 3 );
    glDrawArrays( GL_POINTS, 0, mVBOs[mCurrFrame].size() / 6 / sizeof(GLfloat) );
    mVBOs[mCurrFrame].release();

    mProgram->release();

    // shader program for drawing outlines
    mPrgOutline->bind();
    mPrgOutline->setUniformValue("qt_opacity", 100);
    mPrgOutline->setUniformValue("qt_modelViewMatrix", transition);
    mPrgOutline->setUniformValue("qt_projectionMatrix", mProjectionMatrix);

    mOutlineVBOs[mCurrFrame].bind();

    f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glLineWidth( 5 );

    glDrawArrays( GL_LINE_STRIP, 0,  mOutlineVBOs[mCurrFrame].size() / 3 / sizeof(GLfloat) );
    mOutlineVBOs[mCurrFrame].release();



    mPrgOutline->release();
}


float cGLWidget::deg2rad(int deg)
{
    return deg * 0.0174532925;
}

void cGLWidget::keyPressEvent(QKeyEvent *ke)
{
    QVector3D eyeDir = (mView-mEye).normalized();

    if(ke->key() == Qt::Key_W)
    {
        mEye += eyeDir;
        mView += eyeDir;

        qd << "eye" << mEye << "view" << mView << "up" << mUp;
    }
    if(ke->key() == Qt::Key_S)
    {
        mEye += -eyeDir;
        mView += -eyeDir;

        qd << "eye" << mEye << "view" << mView << "up" << mUp;
    }
    if(ke->key() == Qt::Key_A)
    {
        QVector3D xaxis = QVector3D::crossProduct(mUp, eyeDir).normalized();

        mEye += xaxis;
        mView += xaxis;

        qd << "eye" << mEye << "view" << mView << "up" << mUp;
    }
    if(ke->key() == Qt::Key_D)
    {
        QVector3D xaxis = QVector3D::crossProduct(mUp, eyeDir).normalized();

        mEye += -xaxis;
        mView += -xaxis;

        qd << "eye" << mEye << "view" << mView << "up" << mUp;
    }

    if(ke->key() == Qt::Key_Space)
    {
        mEye += mUp;
        mView += mUp;
    }

    update();
}

void cGLWidget::mouseMoveEvent(QMouseEvent *me)
{
    QVector3D eyeDir = (mView-mEye).normalized();

    if(mLeftMouseButtonPressed && !mSamplingMode)
    {
        QQuaternion result;

        float xangle = -deg2rad(mLeftMouseButtonPressCoordX - me->x()) * 0.1;
        float yangle = deg2rad(mLeftMouseButtonPressCoordY - me->y()) * 0.1;

        mLeftMouseButtonPressCoordX = me->x();
        mLeftMouseButtonPressCoordY = me->y();

        QVector3D yaxis = QVector3D(0,1,0); // rotate around the world's up vector
        QQuaternion r_quat(cos(xangle/2), yaxis.x()*sin(xangle/2), yaxis.y()*sin(xangle/2), yaxis.z()*sin(xangle/2));
        QQuaternion v_quat(0, eyeDir); // create view quaternion from view vector
        QQuaternion u_quat(0, mUp);
        result = (r_quat * v_quat) * r_quat.conjugate();
        QVector3D eyeShit = QVector3D(result.x(), result.y(), result.z()).normalized();
        qd << "eyeshit" << eyeShit;
        mView = eyeShit * (mView-mEye).length();

        result = (r_quat * u_quat) * r_quat.conjugate();
        mUp = QVector3D(result.x(), result.y(), result.z()).normalized();


        eyeDir = (mView-mEye).normalized();

        // rotation axis
        QVector3D xaxis = QVector3D::crossProduct(mUp, eyeDir).normalized();

        QQuaternion rr_quat(cos(xangle/2), xaxis.x()*sin(yangle/2), xaxis.y()*sin(yangle/2), xaxis.z()*sin(yangle/2));
        QQuaternion vv_quat(0, eyeDir); // create view quaternion from view vector
        QQuaternion uu_quat(0, mUp);
        result = (rr_quat * vv_quat) * rr_quat.conjugate();
        eyeShit = QVector3D(result.x(), result.y(), result.z()).normalized();
        mView = eyeShit * (mView - mEye).length();

        result = (rr_quat * uu_quat) * rr_quat.conjugate();
        mUp = QVector3D(result.x(), result.y(), result.z()).normalized();

        update();
    }

    if(mSamplingMode) {
        QVector3D pw;
        if(screenToWorld(me->x(), me->y(), -1.0, pw)) {
            emit(mouseMove(pw));
        }
    }

    update();
}

void cGLWidget::mousePressEvent(QMouseEvent *me)
{

    if(me->button() == Qt::LeftButton) {
        mLeftMouseButtonPressed = true;
        mLeftMouseButtonPressCoordX = me->x();
        mLeftMouseButtonPressCoordY = me->y();


        if(mSamplingMode) {
            QVector3D pw;
            if(screenToWorld(me->x(), me->y(), -1.0, pw)) {
                emit(mousePressed(pw));
            }

            mPoints.append(pw.x());
            mPoints.append(pw.y());
            mPoints.append(pw.z());

            mPointVBO.destroy();

            if(!mPointVBO.isCreated())
                mPointVBO.create();

            qd << mPointVBO.bind();
            mPointVBO.allocate(mPoints.constData(), mPoints.size() * sizeof(GLfloat));

            mPointVBO.release();
        }

    }

    update();
}

void cGLWidget::mouseReleaseEvent(QMouseEvent *me)
{
    if(me->button() == Qt::LeftButton) {
        // save total distance to apply to the new rotation at next mouseMoveEvent
        mSavedX += mLeftMouseButtonPressCoordX - me->x();
        mSavedY += mLeftMouseButtonPressCoordY - me->y();

        mLeftMouseButtonPressed = false;
    }
}

void cGLWidget::setSamplingMode(bool mode)
{
    mSamplingMode = mode;
}

