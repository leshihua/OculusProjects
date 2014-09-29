// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.1.0 (2014/08/17)

//----------------------------------------------------------------------------
template <typename Real>
IntpBilinear2<Real>::IntpBilinear2(int xBound, int yBound, Real xMin,
    Real xSpacing, Real yMin, Real ySpacing, Real const* F)
    :
    mXBound(xBound),
    mYBound(yBound),
    mQuantity(xBound * yBound),
    mXMin(xMin),
    mXSpacing(xSpacing),
    mYMin(yMin),
    mYSpacing(ySpacing),
    mF(F)
{
    // At least a 2x2 block of data points are needed.
    LogAssert(mXBound >= 2 && mYBound >= 2 && mF, "Invalid input.");
    LogAssert(mXSpacing > (Real)0 && mYSpacing > (Real)0, "Invalid input.");

    mXMax = mXMin + mXSpacing * static_cast<Real>(mXBound - 1);
    mInvXSpacing = ((Real)1) / mXSpacing;
    mYMax = mYMin + mYSpacing * static_cast<Real>(mYBound - 1);
    mInvYSpacing = ((Real)1) / mYSpacing;

    mBlend[0][0] = (Real)1;
    mBlend[0][1] = (Real)-1;
    mBlend[1][0] = (Real)0;
    mBlend[1][1] = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real> inline
int IntpBilinear2<Real>::GetXBound() const
{
    return mXBound;
}
//----------------------------------------------------------------------------
template <typename Real> inline
int IntpBilinear2<Real>::GetYBound() const
{
    return mYBound;
}
//----------------------------------------------------------------------------
template <typename Real> inline
int IntpBilinear2<Real>::GetQuantity() const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real const* IntpBilinear2<Real>::GetF() const
{
    return mF;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real IntpBilinear2<Real>::GetXMin() const
{
    return mXMin;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real IntpBilinear2<Real>::GetXMax() const
{
    return mXMax;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real IntpBilinear2<Real>::GetXSpacing() const
{
    return mXSpacing;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real IntpBilinear2<Real>::GetYMin() const
{
    return mYMin;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real IntpBilinear2<Real>::GetYMax() const
{
    return mYMax;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Real IntpBilinear2<Real>::GetYSpacing() const
{
    return mYSpacing;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBilinear2<Real>::operator()(Real x, Real y) const
{
    // Compute x-index and clamp to image.
    Real xIndex = (x - mXMin) * mInvXSpacing;
    int ix = static_cast<int>(xIndex);
    if (ix < 0)
    {
        ix = 0;
    }
    else if (ix >= mXBound)
    {
        ix = mXBound - 1;
    }

    // Compute y-index and clamp to image.
    Real yIndex = (y - mYMin) * mInvYSpacing;
    int iy = static_cast<int>(yIndex);
    if (iy < 0)
    {
        iy = 0;
    }
    else if (iy >= mYBound)
    {
        iy = mYBound - 1;
    }

    Real U[2];
    U[0] = (Real)1;
    U[1] = xIndex - ix;

    Real V[2];
    V[0] = (Real)1.0;
    V[1] = yIndex - iy;

    // Compute P = M*U and Q = M*V.
    Real P[2], Q[2];
    for (int row = 0; row < 2; ++row)
    {
        P[row] = (Real)0;
        Q[row] = (Real)0;
        for (int col = 0; col < 2; ++col)
        {
            P[row] += mBlend[row][col] * U[col];
            Q[row] += mBlend[row][col] * V[col];
        }
    }

    // Compute (M*U)^t D (M*V) where D is the 2x2 subimage containing (x,y).
    Real result = (Real)0;
    for (int row = 0; row < 2; ++row)
    {
        int yClamp = iy + row;
        if (yClamp >= mYBound)
        {
            yClamp = mYBound - 1;
        }

        for (int col = 0; col < 2; ++col)
        {
            int xClamp = ix + col;
            if (xClamp >= mXBound)
            {
                xClamp = mXBound - 1;
            }

            result += P[col] * Q[row] * mF[xClamp + mXBound * yClamp];
        }
    }

    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntpBilinear2<Real>::operator()(int xOrder, int yOrder, Real x, Real y)
    const
{
    // Compute x-index and clamp to image.
    Real xIndex = (x - mXMin) * mInvXSpacing;
    int ix = static_cast<int>(xIndex);
    if (ix < 0)
    {
        ix = 0;
    }
    else if (ix >= mXBound)
    {
        ix = mXBound - 1;
    }

    // Compute y-index and clamp to image.
    Real yIndex = (y - mYMin) * mInvYSpacing;
    int iy = static_cast<int>(yIndex);
    if (iy < 0)
    {
        iy = 0;
    }
    else if (iy >= mYBound)
    {
        iy = mYBound - 1;
    }

    Real U[2], dx, xMult;
    switch (xOrder)
    {
    case 0:
        dx = xIndex - ix;
        U[0] = (Real)1;
        U[1] = dx;
        xMult = (Real)1;
        break;
    case 1:
        dx = xIndex - ix;
        U[0] = (Real)0;
        U[1] = (Real)1;
        xMult = mInvXSpacing;
        break;
    default:
        return (Real)0;
    }

    Real V[2], dy, yMult;
    switch (yOrder)
    {
    case 0:
        dy = yIndex - iy;
        V[0] = (Real)1;
        V[1] = dy;
        yMult = (Real)1;
        break;
    case 1:
        dy = yIndex - iy;
        V[0] = (Real)0;
        V[1] = (Real)1;
        yMult = mInvYSpacing;
        break;
    default:
        return (Real)0;
    }

    // Compute P = M*U and Q = M*V.
    Real P[2], Q[2];
    for (int row = 0; row < 2; ++row)
    {
        P[row] = (Real)0;
        Q[row] = (Real)0;
        for (int col = 0; col < 2; ++col)
        {
            P[row] += mBlend[row][col] * U[col];
            Q[row] += mBlend[row][col] * V[col];
        }
    }

    // Compute (M*U)^t D (M*V) where D is the 2x2 subimage containing (x,y).
    Real result = (Real)0;
    for (int row = 0; row < 2; ++row)
    {
        int yClamp = iy + row;
        if (yClamp >= mYBound)
        {
            yClamp = mYBound - 1;
        }

        for (int col = 0; col < 2; ++col)
        {
            int xClamp = ix + col;
            if (xClamp >= mXBound)
            {
                xClamp = mXBound - 1;
            }

            result += P[col] * Q[row] * mF[xClamp + mXBound * yClamp];
        }
    }
    result *= xMult * yMult;

    return result;
}
//----------------------------------------------------------------------------