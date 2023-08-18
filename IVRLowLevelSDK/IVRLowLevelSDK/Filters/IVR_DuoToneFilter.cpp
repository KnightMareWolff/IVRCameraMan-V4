#include "IVR_DuoToneFilter.h"

CIVRDuoToneFilter::CIVRDuoToneFilter():
    CIVRImageFilter(IVR_Image_Filter_DuoTone)
{
   IVR_LightIntensity = 0;
   IVR_ToneChannel01  = 0;
   IVR_ToneChannel02  = 2;
   IVR_ExpValue       = 5;
}

Mat exponential_function(Mat channel, float exp)
{
    Mat table(1, 256, CV_8U);
    for (int i = 0; i < 256; i++)
        table.at<uchar>(i) = min((int)pow(i,exp),255);
    LUT(channel,table,channel);
    return channel;
}

Mat CIVRDuoToneFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat DuoToneImage;
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

    //1 to 10
    float exp = 1 + IVR_ExpValue/100.0;

    Mat res = BaseImage.clone();
    Mat channels[3];
    split(BaseImage,channels);
    for (int i=0; i<3; i++)
    {
        if ((i == IVR_ToneChannel01)||(i==IVR_ToneChannel02))
        {
             channels[i] = exponential_function(channels[i],exp);
        }
        else
        {
            if (IVR_LightIntensity)
            {
                channels[i] = exponential_function(channels[i],2-exp);
            }
            else
            {
                channels[i] = Mat::zeros(channels[i].size(),CV_8UC1);
            }
        }
    }
    vector<Mat> newChannels{channels[0],channels[1],channels[2]};
    merge(newChannels,res);

    res.convertTo(DuoToneImage, CV_8UC4);
    cvtColor( DuoToneImage,DuoToneImage, COLOR_RGB2BGRA );

    return DuoToneImage.getMat(cv::ACCESS_READ).clone();
}
