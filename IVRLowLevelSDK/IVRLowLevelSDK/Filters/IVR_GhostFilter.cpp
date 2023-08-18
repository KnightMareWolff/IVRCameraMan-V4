#include "IVR_GhostFilter.h"

CIVRGhostFilter::CIVRGhostFilter():
    CIVRImageFilter(IVR_Image_Filter_Ghost)
{
    IVR_R = 255;
    IVR_G = 255;
    IVR_B = 255;
    IVR_Transparency = 127;
}

Mat CIVRGhostFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat GhostImage;
    UMat ConvertedImage;

    bool isGray=true;
    Mat BaseImage;
    //Check if we receive an BGRA grey/binary image(Some algorithms fail with an RGB Gray image)
    //All input images are BGRA , so we convert it to RGB (Will be converted back at the end)
    if(pInputImage.channels()==4)
    {
        Mat dst;
        Mat bgr[4];
        split( pInputImage, bgr );
        absdiff( bgr[0], bgr[1], dst );
        if(countNonZero( dst ))isGray=false;
        absdiff( bgr[0], bgr[2], dst );
        if(countNonZero( dst ))isGray=false;

        //The threshold function fail with rgba images, so we convert it before...
        pInputImage.convertTo(BaseImage, CV_8UC4);

        //Convert the GrayScale TO RGB to avoid errors receiving a black/white image
        if(isGray && pInputImage.channels()<=3)cvtColor(pInputImage, BaseImage , COLOR_GRAY2BGRA);
    }
    else
    {
        //If is not an BGRA Image we ignore the Filterpass
        return pInputImage.clone();
    }

    int height = BaseImage.size().height;
    int width  = BaseImage.size().width;

    for (int i=0; i < height; i++)
    {
        for (int j=0; j < width; j++)
        {
            Vec4b pixel = BaseImage.at<Vec4b>(i,j);

            if(pixel.val[0]==IVR_B &&
               pixel.val[1]==IVR_G &&
               pixel.val[2]==IVR_R)
            {
                pixel.val[3] = IVR_Transparency;
                BaseImage.at<Vec4b>(i,j) = pixel;
            }

        }
    }

    //We not convert back since for transparency we already have an BGRA image...
    return BaseImage.clone();
}
