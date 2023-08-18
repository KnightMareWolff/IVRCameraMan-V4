#include "IVR_InvertFilter.h"

CIVRInvertFilter::CIVRInvertFilter():
    CIVRImageFilter(IVR_Image_Filter_Invert)
{

}

Mat CIVRInvertFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat InvertedImage;

    bool isGray=true;
    Mat BaseImage;
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
    bitwise_not(BaseImage,dst);

    dst.convertTo(InvertedImage, CV_8UC4);
    cvtColor( InvertedImage,InvertedImage, COLOR_RGB2BGRA );

    return InvertedImage.getMat(cv::ACCESS_READ).clone();
}
