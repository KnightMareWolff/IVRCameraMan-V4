#include "IVR_ColdFilter.h"

CIVRColdFilter::CIVRColdFilter():
    CIVRImageFilter(IVR_Image_Filter_Cold)
{

}

Mat CIVRColdFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    Mat BaseImage;
    UMat ColdImage;
    UMat Result;

    bool isGray=true;

    //Check if we receive an BGRA grey/binary image(Some algorithms fail with an RGB Gray image)
    //All input images are BGRA , so we convert it to RGB (Will be converted back at the end)
    if(pInputImage.channels()==4)
    {
        BaseImage = pInputImage.clone();
        //The threshold function fail with rgba images, so we convert it before...
        cvtColor(BaseImage, BaseImage , COLOR_BGRA2BGR);
        pInputImage.convertTo(BaseImage, CV_8UC3);

        Mat dst;
        Mat bgr[4];
        split( BaseImage, bgr );
        absdiff( bgr[0], bgr[1], dst );
        if(countNonZero( dst ))isGray=false;
        absdiff( bgr[0], bgr[2], dst );
        if(countNonZero( dst ))isGray=false;

        //Convert the GrayScale TO RGB to avoid errors receiving a black/white image
        if(isGray && pInputImage.channels() < 3)cvtColor(BaseImage, BaseImage , COLOR_GRAY2BGR);
        if(isGray)cvtColor(BaseImage, BaseImage , COLOR_RGB2BGR);
    }
    else
    {
        //If is not an BGRA Image we ignore the Filterpass
        return pInputImage.clone();
    }

    //Orignal x-axis values
    float originalValue[] = {0, 50, 100, 150, 200, 255};
    //changed points on Y-axis for red and blue channel
    float blueValues[] = {0, 80, 150, 190, 220, 255};
    float redValues [] = {0, 20, 40, 75, 150, 255};

    //spliting the channels
    vector<Mat> channels(3);
    split(BaseImage, channels);

    //create lookup table for red channel
    Mat redLookupTable(1, 256, CV_8U);
    uchar* lut = redLookupTable.ptr();
    //apply interpolation and create red channel lookup table
    for(int i=0; i<256; i++){
        int j=0;
        float a = (float)i;
        while(a > originalValue[j]){
            j++;
        }
        if(a == originalValue[j]){
            lut[i] = redValues[j];
            continue;
        }
        float slope = ((float)(redValues[j] - redValues[j-1]))/((float)(originalValue[j] - originalValue[j-1]));
        float constant = redValues[j] - slope * originalValue[j];
        lut[i] = slope*a + constant;
    }

    //create lookup table for blue channel
    Mat blueLookupTable(1, 256, CV_8U);
    lut = blueLookupTable.ptr();
    //apply interpolation and create blue channel lookup table
    for(int i=0; i<256; i++){
        int j=0;
        float a = (float)i;
        while(a > originalValue[j]){
            j++;
        }
        if(a == originalValue[j]){
            lut[i] = blueValues[j];
            continue;
        }
        float slope = ((float)(blueValues[j] - blueValues[j-1]))/((float)(originalValue[j] - originalValue[j-1]));
        float constant = blueValues[j] - slope * originalValue[j];
        lut[i] = slope*a + constant;
    }

    //Apply mapping for red channel
    LUT(channels[2], redLookupTable, channels[2]);

    //Apply mapping for blue channel
    LUT(channels[0], blueLookupTable, channels[0]);

    merge(channels, Result);

    Result.convertTo(ColdImage, CV_8UC4);
    cvtColor( ColdImage,ColdImage, COLOR_RGB2BGRA );

    return ColdImage.getMat(cv::ACCESS_READ).clone();
}
