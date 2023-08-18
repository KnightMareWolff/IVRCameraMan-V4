#include "IVR_OldFilmFilter.h"

CIVROldFilmFilter::CIVROldFilmFilter():
    CIVRImageFilter(IVR_Image_Filter_OldFilm)
{
    IVR_Treshold01 = 100.0f;
    IVR_ExpValue = 50.0f;
}

Mat CIVROldFilmFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat OldFilmImage;
    Mat GrayImage;

    bool isGray=true;
    Mat BaseImage;
    int height;
    int width;
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

        //Convert the Image to GrayScale
        if(!isGray)cvtColor(BaseImage, GrayImage , COLOR_RGB2GRAY);
        else
        {
            cvtColor(BaseImage, GrayImage , COLOR_RGB2GRAY);
        }
    }
    else
    {
        //If is not an BGRA Image we ignore the Filterpass
        return pInputImage.clone();
    }

    height = GrayImage.size().height;
    width  = GrayImage.size().width;

    for (int i=0; i < height; i++)
    {
        for (int j=0; j < width; j++)
        {
            if (rand()%100 <= IVR_Treshold01)
            {
                if (rand()%2 == 0)
                    GrayImage.at<uchar>(i,j) = std::min(GrayImage.at<uchar>(i,j) + rand()%((int)IVR_ExpValue+1), 255);
                else
                    GrayImage.at<uchar>(i,j) = std::max(GrayImage.at<uchar>(i,j) - rand()%((int)IVR_ExpValue+1), 0);
            }
        }
    }

    //Convert the result in a 8bit Color and put it in the Final Image
    GrayImage.convertTo(OldFilmImage, CV_8UC4);
    cvtColor( OldFilmImage,OldFilmImage, COLOR_GRAY2BGRA );

    return OldFilmImage.getMat(cv::ACCESS_READ).clone();
}
