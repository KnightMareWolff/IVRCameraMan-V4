#include "IVR_CartoonFilter.h"

CIVRCartoonFilter::CIVRCartoonFilter():
    CIVRImageFilter(IVR_Image_Filter_Cartoon)
{

    IVR_Treshold01    = 50;
    IVR_Treshold02    = 150;
    //Aperture size  != 3 allways fail, so remove it from the change parameters...;
    IVR_ApertureSize  = 3;
    IVR_UseL2Gradient = false;
    IVR_KernelSize    = 2;

}

Mat CIVRCartoonFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    Mat  BaseImage;
    UMat SmoothedImage;
    UMat CartoonImage;

    //For Cartoon Filter
    UMat ImgMedian;
    Mat ImgCanny;
    Mat ImgFloatCanny;
    Mat ImgBF;
    Mat ImgResult;
    UMat ImgResultF;
    UMat ImgCanny3c;
    UMat ImgKernel;

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
    //---------------------------------------------------
    //Remove noise and Resize
    //---------------------------------------------------
    //remove noise
    medianBlur(BaseImage ,SmoothedImage, 3);
    //---------------------------------------------------
    //Cartoonize the image
    //---------------------------------------------------
    medianBlur(SmoothedImage, ImgMedian, 7);

    //Detect the edges with the Canny algorithm
    Canny(ImgMedian, ImgCanny, IVR_Treshold01 , IVR_Treshold02,IVR_ApertureSize,IVR_UseL2Gradient);

    //make the edges bigger... (Inflate edges)
    getStructuringElement(MORPH_RECT, Size(IVR_KernelSize, IVR_KernelSize)).copyTo(ImgKernel);
    dilate(ImgCanny, ImgCanny, ImgKernel);

    //Scale the edges to 1 and invert values
    ImgCanny = ImgCanny/ 255;
    ImgCanny = 1 - ImgCanny;

    //convert to a float Canny
    ImgCanny.convertTo(ImgFloatCanny, CV_32FC3);

    //Apply a simple blur to smooth the effect.
    blur(ImgFloatCanny, ImgFloatCanny, Size(IVR_KernelSize, IVR_KernelSize));

    //Apply Bilateral Filter to homogenizes the color(Using the Original Image).
    bilateralFilter(BaseImage, ImgBF, 9, 150.0, 150.0);

    //Truncate the Colors
    ImgResult = ImgBF / 25;
    ImgResult = ImgResult * 25;

    //Merge the Colors and the Edges
    Mat cannyChannels[] = { ImgFloatCanny , ImgFloatCanny , ImgFloatCanny };
    merge(cannyChannels, 3, ImgCanny3c);

    //Convert to Float Colors Before Merge it
    ImgResult.convertTo(ImgResultF, CV_32FC3);

    //Multiply Colors and Edges
    multiply(ImgResultF, ImgCanny3c, ImgResultF);

    //Convert the result in a 8bit Color and put it in the Final Image
    ImgResultF.convertTo(CartoonImage, CV_8UC4);
    cvtColor( CartoonImage,CartoonImage, COLOR_RGB2BGRA );

    return CartoonImage.getMat(cv::ACCESS_READ).clone();
}
