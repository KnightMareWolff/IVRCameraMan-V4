#include "IVR_SharpenFilter.h"

CIVRSharpenFilter::CIVRSharpenFilter():
    CIVRImageFilter(IVR_Image_Filter_Sharpen)
{
   IVR_ExpValue = 9.2f;
}

Mat CIVRSharpenFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat SharpedImage;

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

    //generates the kernel
    //[-1, -1 , -1]
    //[-1, 9.2, -1]
    //[-1, -1 , -1]
    Mat sharpening_kernel = Mat(3,3,CV_32FC1,(float)(-1.0f));
    sharpening_kernel.at<float>(1,1) = (float)(IVR_ExpValue);

    Mat dst;
    filter2D(BaseImage,dst,-1,sharpening_kernel);

    dst.convertTo(SharpedImage, CV_8UC4);
    cvtColor( SharpedImage,SharpedImage, COLOR_RGB2BGRA );

    return SharpedImage.getMat(cv::ACCESS_READ).clone();
}
