#include "IVR_DetailFilter.h"

CIVRDetailFilter::CIVRDetailFilter():
    CIVRImageFilter(IVR_Image_Filter_Detail)
{
    IVR_SigmaS      = 15.0f;
    IVR_SigmaR      = 0.15f;
}

Mat CIVRDetailFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat DetailImage;
    Mat BaseImage;

    bool isGray=true;

    //Check if we receive an BGRA grey/binary image(Some algorithms fail with an RGB Gray image)
    //All input images are BGRA , so we convert it to RGB (Will be converted back at the end)
    if(pInputImage.channels()==4)
    {
        //The threshold function fail with rgba images, so we convert it before...
        pInputImage.convertTo(BaseImage, CV_8UC3);
        cvtColor(BaseImage, BaseImage , COLOR_BGRA2RGB);

        Mat dst;
        Mat bgr[4];
        split( BaseImage, bgr );
        absdiff( bgr[0], bgr[1], dst );
        if(countNonZero( dst ))isGray=false;
        absdiff( bgr[0], bgr[2], dst );
        if(countNonZero( dst ))isGray=false;

        //Convert the GrayScale TO RGB to avoid errors receiving a black/white image
        if(isGray && pInputImage.channels() < 3)cvtColor(BaseImage, BaseImage , COLOR_GRAY2RGB);
    }
    else
    {
        //If is not an BGRA Image we ignore the Filterpass
        return pInputImage.clone();
    }

    Mat dst;
    detailEnhance(BaseImage,dst,IVR_SigmaS,IVR_SigmaR);

    dst.convertTo(DetailImage, CV_8UC4);
    cvtColor( DetailImage,DetailImage, COLOR_RGB2BGRA );

    return DetailImage.getMat(cv::ACCESS_READ).clone();
}
