#include "ccircle.h"
#include "utils.h"

cCircle::cCircle()
{
    Q_ASSERT(false);
}

cCircle::cCircle(float a, float p, float f) : mAmp(a), mPhase(p), mFreq(f)
{
    setX(0);
    setY(0);
}

QVector2D cCircle::tick(int index, qint64 msElapsed, QVector2D pos)
{
    float elapsed = msElapsed / 1000.0f;
    float angle = mPhase + mFreq * 2 * M_PI * elapsed;

    setX(cos(angle) * mAmp);
    setY(sin(angle) * mAmp);

    mVertices.append(pos.x());
    mVertices.append(pos.y());
    mVertices.append(0.0);

    mVertices.append(mAmp);
    mVertices.append(angle);
    mVertices.append(index / 2.); // can't do too much with the freq in the shaders, so use index for z indexing

    return pos + *this;
}
