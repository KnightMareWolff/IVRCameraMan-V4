#include "IVR_PencilSketchFilter.h"

CIVRPencilSketchFilter::CIVRPencilSketchFilter():
    CIVRImageFilter(IVR_Image_Filter_PencilSkt)
{
    IVR_SigmaS      = 20.0f;
    IVR_SigmaR      = 0.5f;
    IVR_ShadeFactor = 0.02f;

    IVR_ColorOutput = true;
}

Mat CIVRPencilSketchFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat GraySketch;
    UMat ColorSketch;
    UMat ConvertedImage;
    UMat FinalImage;

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

    pencilSketch(BaseImage,GraySketch,ColorSketch,IVR_SigmaS,IVR_SigmaR,IVR_ShadeFactor);

    if(IVR_ColorOutput)
    {
        ColorSketch.convertTo(FinalImage, CV_8UC4);
        cvtColor( FinalImage,FinalImage, COLOR_RGB2BGRA );
    }
    else
    {
        cvtColor(GraySketch, GraySketch , COLOR_GRAY2RGB);
        GraySketch.convertTo(FinalImage, CV_8UC4);
        cvtColor( FinalImage,FinalImage, COLOR_RGB2BGRA );
    }

    return FinalImage.getMat(cv::ACCESS_READ).clone();
}
