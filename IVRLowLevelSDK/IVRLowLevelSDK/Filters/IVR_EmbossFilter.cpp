#include "IVR_EmbossFilter.h"

CIVREmbossFilter::CIVREmbossFilter():
    CIVRImageFilter(IVR_Image_Filter_Emboss)
{
   IVR_KernelSize = 4;
}

Mat CIVREmbossFilter::IVR_ApplyFilter(const Mat pInputImage)
{
    UMat EmbossImage;
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

    //Number of 90 deg rotations in the kernel before apply the filter
    int s    = 1;
    int height = BaseImage.size().height;
    int width  = BaseImage.size().width;

    Mat y = Mat(height,width,CV_8U,Scalar(128));

    Mat gray;
    cvtColor(BaseImage,gray,COLOR_BGR2GRAY);

    //generates the kernel
    Mat kernel = Mat(IVR_KernelSize,IVR_KernelSize,CV_8S,Scalar(0));
    for (int i=0; i<IVR_KernelSize; i++)
    {
        for (int j=0; j<IVR_KernelSize; j++)
        {
            if (i < j)
            {
                kernel.at<schar>(i,j) = -1;
            }
            else if (i > j)
            {
                kernel.at<schar>(i,j) = 1;
            }
        }
    }

    for (int i=0; i<s; i++)
        rotate(kernel,kernel,ROTATE_90_COUNTERCLOCKWISE);

    Mat dst;
    filter2D(gray,dst,-1,kernel);
    Mat res;
    add(dst,y,res);

    cvtColor(res, res , COLOR_GRAY2RGB);
    res.convertTo(EmbossImage, CV_8UC4);
    cvtColor( EmbossImage,EmbossImage, COLOR_RGB2BGRA );

    return EmbossImage.getMat(cv::ACCESS_READ).clone();
}
