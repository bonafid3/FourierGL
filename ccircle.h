#ifndef CCIRCLE_H
#define CCIRCLE_H

#include <QVector2D>
#include <QtOpenGL>

class cCircle : QVector2D
{
public:
    cCircle();
    cCircle(float a, float p, float f);

    QVector2D tick(int index, qint64 msElapsed, QVector2D pos);

    float mAmp;
    float mPhase;
    float mFreq;

    QVector<GLfloat> mVertices;

};

#endif // CCIRCLE_H
