#include "IVR_ImageFilter.h"

CIVRImageFilter::CIVRImageFilter(int pIVR_FilterType)
{
    IVR_FilterType = pIVR_FilterType;
}

Mat CIVRImageFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    return pInputImage.clone();
}
