#include "IVR_DilateFilter.h"

CIVRDilateFilter::CIVRDilateFilter():
    CIVRImageFilter(IVR_Image_Filter_Binary)
{
    IVR_KernelSize = 2;
    IVR_Iteractions= 5;
}

Mat CIVRDilateFilter::IVR_ApplyFilter(const Mat pInputImage)
{

    UMat DilatedImage;
    UMat ConvertedImage;

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
    //---------------------------------------------------
    //Dilate the image
    //---------------------------------------------------
    Mat kernel = getStructuringElement(MORPH_CROSS, Size(IVR_KernelSize, IVR_KernelSize));

    dilate(BaseImage, DilatedImage, kernel, Point(-1, -1), IVR_Iteractions);

    //Convert the result in a 8bit Color and put it in the Final Image
    DilatedImage.convertTo(ConvertedImage, CV_8UC4);
    cvtColor( ConvertedImage,ConvertedImage, COLOR_RGB2BGRA );

    return ConvertedImage.getMat(cv::ACCESS_READ).clone();
}
