#include "IVR_BinaryFilter.h"

CIVRBinaryFilter::CIVRBinaryFilter():
    CIVRImageFilter(IVR_Image_Filter_Binary)
{

}

Mat CIVRBinaryFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    Mat       BinarisedImage;
    UMat       DilatedImage;
    UMat       BinGray;
    UMat       BinRGB;
    UMat       ConvertedImage;

    bool isGray=true;
    UMat BaseImage;
    //Check if we receive an BGRA grey/binary image(Some algorithms fail with an RGB Gray image)
    //All input images are BGRA , so we convert it to RGB (Will be converted back at the end)
    if(pInputImage.channels()==4)
    {
        //The threshold function fail with rgba images, so we convert it before...
        pInputImage.convertTo(BaseImage, CV_8UC3);
        cvtColor(BaseImage, BaseImage , COLOR_BGRA2RGB);

        Mat dst;
        Mat bgr[4];
        split( pInputImage, bgr );
        absdiff( bgr[0], bgr[1], dst );
        if(countNonZero( dst ))isGray=false;
        absdiff( bgr[0], bgr[2], dst );
        if(countNonZero( dst ))isGray=false;

        //Convert the Image to GrayScale if we not receive a black/white image
        if(!isGray)cvtColor(BaseImage, BinGray , COLOR_RGB2GRAY);
        else
        {
            cvtColor(BaseImage, BinGray , COLOR_RGB2GRAY);
        }
    }
    else
    {
        //If is not an BGRA Image we ignore the Filterpass
        return pInputImage.clone();
    }


    //Binary The grayscale (1 Channel Image)
    threshold(BinGray, BinarisedImage, 0, 255, THRESH_OTSU);

    //We count and invert the Black/white pixels
    int white = countNonZero(BinarisedImage);
    int black = BinarisedImage.size().area() - white;

    if (white > black)
    {
        BinarisedImage = ~BinarisedImage;
    }

    //Convert the result in a 8bit Color RGBA and put it in the Final Image
    BinarisedImage.convertTo(ConvertedImage, CV_8UC4);
    cvtColor( ConvertedImage,ConvertedImage, COLOR_GRAY2BGRA );

    return ConvertedImage.getMat(cv::ACCESS_READ).clone();
}
