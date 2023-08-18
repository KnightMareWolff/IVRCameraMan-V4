#include "IVR_WarmFilter.h"

CIVRWarmFilter::CIVRWarmFilter():
    CIVRImageFilter(IVR_Image_Filter_Warm)
{

}

Mat CIVRWarmFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat WarmImage;
    UMat Result;

    bool isGray=true;
    Mat BaseImage;
    //Check if we receive an BGRA grey/binary image(Some algorithms fail with an RGB Gray image)
    //All input images are BGRA , so we convert it to RGB (Will be converted back at the end)
    if(pInputImage.channels()==4)
    {
        //The threshold function fail with rgba images, so we convert it before...
        pInputImage.convertTo(BaseImage, CV_8UC3);
        cvtColor(BaseImage, BaseImage , COLOR_BGRA2BGR);

        Mat dst;
        Mat bgr[4];
        split( BaseImage, bgr );
        absdiff( bgr[0], bgr[1], dst );
        if(countNonZero( dst ))isGray=false;
        absdiff( bgr[0], bgr[2], dst );
        if(countNonZero( dst ))isGray=false;

        //Convert the GrayScale TO RGB to avoid errors receiving a black/white image
        if(isGray && pInputImage.channels() < 3)cvtColor(BaseImage, BaseImage , COLOR_GRAY2BGR);
    }
    else
    {
        //If is not an BGRA Image we ignore the Filterpass
        return pInputImage.clone();
    }

    //Reference points for X-Axis
    float originalValue[] = {0,50,100,150,200,255};
    //Changed points on Y-axis for red and blue channels
    float redValue[]  = {0,80,150,190,220,255};
    float blueValue[] = {0,20,40,75,150,255};

    //split into channels
    vector<Mat> channels(3);
    split(BaseImage, channels);

    Mat lookupTable(1, 256, CV_8U);
    uchar* lut = lookupTable.ptr();

    //Linear Interpolation applied to get values for all the points on X-Axis
    for(int i=0; i< 256; i++)
    {
        int j=0;
        float xval = (float) i;
        while(xval > originalValue[j])
        {
            j++;
        }
        if(xval == originalValue[j])
        {
            lut[i] = redValue[j];
            continue;
        }
        float slope = ((float)(redValue[j] - redValue[j-1]))/((float)(originalValue[j] - originalValue[j-1]));
        float constant = redValue[j] - slope * originalValue[j];
        lut[i] = slope*xval + constant;
    }

    //Applying mapping and check for underflow/overflow in Red channel
    LUT(channels[2], lookupTable, channels[2]);
    min(channels[2], 255, channels[2]);
    max(channels[2], 0, channels[2]);

    Mat lookupTable2(1, 256, CV_8U);
    uchar* lut2 = lookupTable2.ptr();

    //Linear Interpolation applied to get values for all the points on X-Axis
    for(int i=0; i< 256; i++)
    {
        int j=0;
        float xval = (float) i;
        while(xval > originalValue[j])
        {
            j++;
        }
        if(xval == originalValue[j]){
            lut2[i] = blueValue[j];
            continue;
        }
        float slope = ((float)(blueValue[j] - blueValue[j-1]))/((float)(originalValue[j] - originalValue[j-1]));
        float constant = blueValue[j] - slope * originalValue[j];
        lut2[i] = slope*xval + constant;
    }

    LUT(channels[0], lookupTable2, channels[0]);
    min(channels[0], 255, channels[0]);
    min(channels[0], 0, channels[0]);

    //Merge the channels
    merge(channels, Result);

    Result.convertTo(WarmImage, CV_8UC4);
    cvtColor( WarmImage,WarmImage, COLOR_BGR2BGRA );

    return WarmImage.getMat(cv::ACCESS_READ).clone();
}
