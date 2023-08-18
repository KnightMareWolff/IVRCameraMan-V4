#include "IVR_ImageHandler.h"

CIVRImageHandler::CIVRImageHandler()
{

}

CIVRImageHandler::~CIVRImageHandler()
{
    //Image Handler data sources
    IVR_ImageData.release();
}

bool CIVRImageHandler::IVR_LoadImage(QString pFullPath)
{
    UMat tImageLoaded;
    UMat tImageConverted;
    // To avoid log spam, make sure it exists before doing anything else.
    if (!QFile::exists(pFullPath)){	return false; }

    //Load the Image
    imread(pFullPath.toStdString(), IMREAD_UNCHANGED).copyTo(tImageLoaded);

    if(tImageLoaded.channels()<3)
    {
       tImageLoaded.convertTo(tImageConverted, CV_8UC4);
       cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
    }
    else if(tImageLoaded.channels()==3)
    {
       tImageLoaded.convertTo(tImageConverted, CV_8UC4);
       cvtColor( tImageConverted,tImageConverted, COLOR_BGR2BGRA );

    }
    else
    {
        tImageLoaded.convertTo(tImageConverted, CV_8UC4);
        cvtColor( tImageConverted,tImageConverted, COLOR_BGR2BGRA );
    }

    IVR_PixelSize = 4;
    IVR_Size = QVector2D(tImageConverted.cols, tImageConverted.rows);

    if(IVR_Size.x() == 0 && IVR_Size.y()==0)return false;

    //Set the Image Data
    tImageConverted.copyTo(IVR_ImageData);

    return true;
}

bool CIVRImageHandler::IVR_LoadImageMAT(const Mat& pIVRMatImage)
{
    UMat tImageLoaded;
    UMat tImageConverted;

    pIVRMatImage.copyTo(tImageLoaded);

    if(tImageLoaded.channels()<3)
    {
       tImageLoaded.convertTo(tImageConverted, CV_8UC4);
       cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
    }
    else if(tImageLoaded.channels()==3)
    {
       tImageLoaded.convertTo(tImageConverted, CV_8UC4);
       cvtColor( tImageConverted,tImageConverted, COLOR_RGB2BGRA );
    }
    else
    {
        tImageLoaded.convertTo(tImageConverted, CV_8UC4);
        cvtColor( tImageConverted,tImageConverted, COLOR_BGR2BGRA );
    }

    IVR_PixelSize = 4;
    IVR_Size = QVector2D(tImageConverted.cols, tImageConverted.rows);

    if(IVR_Size.x() == 0 && IVR_Size.y()==0)return false;

    //Set the Image Data
    tImageConverted.copyTo(IVR_ImageData);
    return true;
}

bool CIVRImageHandler::IVR_LoadImageUMAT(const UMat& pIVRMatImage)
{
    UMat tImageLoaded;
    UMat tImageConverted;

    IVR_PixelSize = pIVRMatImage.channels();
    IVR_Size      = QVector2D(pIVRMatImage.cols, pIVRMatImage.rows);

    if(IVR_Size.x() == 0 && IVR_Size.y()==0)return false;

    pIVRMatImage.copyTo(tImageLoaded);

    if(tImageLoaded.channels()<3)
    {
       tImageLoaded.convertTo(tImageConverted, CV_8UC4);
       cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
    }
    else if(tImageLoaded.channels()==3)
    {
       tImageLoaded.convertTo(tImageConverted, CV_8UC4);
       cvtColor( tImageConverted,tImageConverted, COLOR_RGB2BGRA );
    }
    else
    {
        tImageLoaded.convertTo(tImageConverted, CV_8UC4);
        cvtColor( tImageConverted,tImageConverted, COLOR_BGR2BGRA );
    }

    //Set the Image Data
    tImageConverted.copyTo(IVR_ImageData);
    return true;
}

bool CIVRImageHandler::IVR_SaveImage(QString pFullPath)
{
    bool tRet;

    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    //Save the Image
    tRet = imwrite(pFullPath.toStdString(),IVR_ImageData,compression_params);
    //If fails with PNG try again for a normal file
    if(!tRet)
    {
        //Save the Image
        tRet = imwrite(pFullPath.toStdString(),IVR_ImageData);
    }

    if(!tRet)return false;
    return true;
}

bool CIVRImageHandler::IVR_SaveEffect(QString effectName)
{
    FILE *effectFile;

    effectFile = fopen(QString(CIVRConfig::IVR_RootFolder + "/" + effectName + ".ief").toStdString().c_str(),"wb");

    if (effectFile != NULL)
    {
        uint effectsSize = filterArray.size();
        fwrite(&effectsSize, sizeof(uint), 1, effectFile);
        for(auto filter:filterArray)
        {
            int effectsType = filter->IVR_FilterType;
            fwrite(&effectsType,sizeof(int), 1, effectFile);

            switch(filter->IVR_FilterType)
            {
            case IVR_Image_Filter_Cartoon  :
            {
                int   aperture = dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_ApertureSize;
                int   kernel   = dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_KernelSize;
                float tre01    = dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_Treshold01;
                float tre02    = dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_Treshold02;
                bool  bL2gra   = dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_UseL2Gradient;
                fwrite(&aperture,sizeof(int)  , 1, effectFile);
                fwrite(&kernel  ,sizeof(int)  , 1, effectFile);
                fwrite(&tre01   ,sizeof(float), 1, effectFile);
                fwrite(&tre02   ,sizeof(float), 1, effectFile);
                fwrite(&bL2gra  ,sizeof(bool) , 1, effectFile);
            }break;
            case IVR_Image_Filter_Histogram:{/*No Filter*/}break;
            case IVR_Image_Filter_Binary   :
            {
                //No Parameter
            }break;
            case IVR_Image_Filter_Crop     :{/*No Filter*/}break;
            case IVR_Image_Filter_Contours :{/*No Filter*/}break;
            case IVR_Image_Filter_Shapes   :{/*No Filter*/}break;
            case IVR_Image_Filter_Dilate   :
            {
                int   iteractions= dynamic_cast<CIVRDilateFilter*>(filter)->IVR_Iteractions;
                int   kernel     = dynamic_cast<CIVRDilateFilter*>(filter)->IVR_KernelSize;
                fwrite(&iteractions,sizeof(int)  , 1, effectFile);
                fwrite(&kernel     ,sizeof(int)  , 1, effectFile);
            }break;
            case IVR_Image_Filter_OldFilm  :
            {
                float expval=dynamic_cast<CIVROldFilmFilter*>(filter)->IVR_ExpValue;
                float tres01=dynamic_cast<CIVROldFilmFilter*>(filter)->IVR_Treshold01;
                fwrite(&expval   ,sizeof(float), 1, effectFile);
                fwrite(&tres01   ,sizeof(float), 1, effectFile);
            }break;
            case IVR_Image_Filter_PencilSkt:
            {
                bool  color =dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_ColorOutput;
                float shade =dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_ShadeFactor;
                float sigmas=dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_SigmaS;
                float sigmar=dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_SigmaR;
                fwrite(&color  ,sizeof(bool) , 1, effectFile);
                fwrite(&shade  ,sizeof(float), 1, effectFile);
                fwrite(&sigmas ,sizeof(float), 1, effectFile);
                fwrite(&sigmar ,sizeof(float), 1, effectFile);

            }break;
            case IVR_Image_Filter_Sepia    :
            {
                //No Parameter
            }break;
            case IVR_Image_Filter_Emboss   :
            {
                int   kernel = dynamic_cast<CIVREmbossFilter*>(filter)->IVR_KernelSize;
                fwrite(&kernel ,sizeof(int)  , 1, effectFile);
            }break;
            case IVR_Image_Filter_DuoTone  :
            {
                int tone01=dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_ToneChannel01;
                int tone02=dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_ToneChannel02;
                float expval=dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_ExpValue;
                int light=dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_LightIntensity;
                fwrite(&tone01,sizeof(int)  , 1, effectFile);
                fwrite(&tone02,sizeof(int)  , 1, effectFile);
                fwrite(&expval,sizeof(float), 1, effectFile);
                fwrite(&light ,sizeof(int)  , 1, effectFile);
            }break;
            case IVR_Image_Filter_Warm     :
            {
                //No Parameter
            }break;
            case IVR_Image_Filter_Cold     :
            {
                //No Parameter
            }break;
            case IVR_Image_Filter_Gotham   :
            {
                //No Parameter
            }break;
            case IVR_Image_Filter_Sharpen  :
            {
                float expval=dynamic_cast<CIVRSharpenFilter*>(filter)->IVR_ExpValue;
                fwrite(&expval,sizeof(float), 1, effectFile);
            }break;
            case IVR_Image_Filter_Detail   :
            {
                float sigmas=dynamic_cast<CIVRDetailFilter*>(filter)->IVR_SigmaS;
                float sigmar=dynamic_cast<CIVRDetailFilter*>(filter)->IVR_SigmaR;
                fwrite(&sigmas ,sizeof(float), 1, effectFile);
                fwrite(&sigmar ,sizeof(float), 1, effectFile);
            }break;
            case IVR_Image_Filter_Invert   :
            {
                //No Parameter
            }break;
            case IVR_Image_Filter_Stylize  :
            {
                float sigmas=dynamic_cast<CIVRStylizeFilter*>(filter)->IVR_SigmaS;
                float sigmar=dynamic_cast<CIVRStylizeFilter*>(filter)->IVR_SigmaR;
                fwrite(&sigmas ,sizeof(float), 1, effectFile);
                fwrite(&sigmar ,sizeof(float), 1, effectFile);
            }break;
            case IVR_Image_Filter_Ghost  :
            {
                int colorR=dynamic_cast<CIVRGhostFilter*>(filter)->IVR_R;
                int colorG=dynamic_cast<CIVRGhostFilter*>(filter)->IVR_G;
                int colorB=dynamic_cast<CIVRGhostFilter*>(filter)->IVR_B;
                int transp=dynamic_cast<CIVRGhostFilter*>(filter)->IVR_Transparency;
                fwrite(&colorR ,sizeof(int), 1, effectFile);
                fwrite(&colorG ,sizeof(int), 1, effectFile);
                fwrite(&colorB ,sizeof(int), 1, effectFile);
                fwrite(&transp ,sizeof(int), 1, effectFile);
            }break;
            }
        }
    }

    fclose(effectFile);
    return true;
}

bool CIVRImageHandler::IVR_LoadEffect(QString effectName)
{
    FILE *effectFile;

    if(filterArray.size()>0)IVR_CleanUpFilters();

    effectFile = fopen(QString(CIVRConfig::IVR_RootFolder + "/" + effectName + ".ief").toStdString().c_str(),"rb");
    if (effectFile != NULL)
    {
        uint effectsSize;
        fread(&effectsSize, sizeof(uint), 1, effectFile);
        for(uint i=0;i<effectsSize;i++)
        {
            int effectsType;
            fread(&effectsType,sizeof(int), 1, effectFile);

            switch(effectsType)
            {
            case IVR_Image_Filter_Cartoon  :
            {
                int   aperture;
                int   kernel  ;
                float tre01   ;
                float tre02   ;
                bool  bL2gra  ;

                fread(&aperture,sizeof(int)  , 1, effectFile);
                fread(&kernel  ,sizeof(int)  , 1, effectFile);
                fread(&tre01   ,sizeof(float), 1, effectFile);
                fread(&tre02   ,sizeof(float), 1, effectFile);
                fread(&bL2gra  ,sizeof(bool) , 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Cartoon);
                dynamic_cast<CIVRCartoonFilter*>(filterArray[i])->IVR_ApertureSize  = aperture;
                dynamic_cast<CIVRCartoonFilter*>(filterArray[i])->IVR_KernelSize    = kernel;
                dynamic_cast<CIVRCartoonFilter*>(filterArray[i])->IVR_Treshold01    = tre01;
                dynamic_cast<CIVRCartoonFilter*>(filterArray[i])->IVR_Treshold02    = tre02;
                dynamic_cast<CIVRCartoonFilter*>(filterArray[i])->IVR_UseL2Gradient = bL2gra;
            }break;
            case IVR_Image_Filter_Histogram:{/*No Filter*/}break;
            case IVR_Image_Filter_Binary   :
            {
                IVR_AddFilter(IVR_Image_Filter_Binary);
            }break;
            case IVR_Image_Filter_Crop     :{/*No Filter*/}break;
            case IVR_Image_Filter_Contours :{/*No Filter*/}break;
            case IVR_Image_Filter_Shapes   :{/*No Filter*/}break;
            case IVR_Image_Filter_Dilate   :
            {
                int   iteractions;
                int   kernel     ;

                fread(&iteractions,sizeof(int)  , 1, effectFile);
                fread(&kernel     ,sizeof(int)  , 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Dilate);
                dynamic_cast<CIVRDilateFilter*>(filterArray[i])->IVR_Iteractions = iteractions;
                dynamic_cast<CIVRDilateFilter*>(filterArray[i])->IVR_KernelSize  = kernel;
            }break;
            case IVR_Image_Filter_OldFilm  :
            {
                float expval;
                float tres01;
                fread(&expval   ,sizeof(float), 1, effectFile);
                fread(&tres01   ,sizeof(float), 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_OldFilm);
                dynamic_cast<CIVROldFilmFilter*>(filterArray[i])->IVR_ExpValue   =expval;
                dynamic_cast<CIVROldFilmFilter*>(filterArray[i])->IVR_Treshold01 =tres01;
            }break;
            case IVR_Image_Filter_PencilSkt:
            {
                bool  color ;
                float shade ;
                float sigmas;
                float sigmar;
                fread(&color  ,sizeof(bool) , 1, effectFile);
                fread(&shade  ,sizeof(float), 1, effectFile);
                fread(&sigmas ,sizeof(float), 1, effectFile);
                fread(&sigmar ,sizeof(float), 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_PencilSkt);
                dynamic_cast<CIVRPencilSketchFilter*>(filterArray[i])->IVR_ColorOutput =color;
                dynamic_cast<CIVRPencilSketchFilter*>(filterArray[i])->IVR_ShadeFactor =shade;
                dynamic_cast<CIVRPencilSketchFilter*>(filterArray[i])->IVR_SigmaS      =sigmas;
                dynamic_cast<CIVRPencilSketchFilter*>(filterArray[i])->IVR_SigmaR      =sigmar;
            }break;
            case IVR_Image_Filter_Sepia    :
            {
                IVR_AddFilter(IVR_Image_Filter_Sepia);
            }break;
            case IVR_Image_Filter_Emboss   :
            {
                int   kernel ;
                fread(&kernel ,sizeof(int)  , 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Emboss);
                dynamic_cast<CIVREmbossFilter*>(filterArray[i])->IVR_KernelSize = kernel;
            }break;
            case IVR_Image_Filter_DuoTone  :
            {
                int tone01;
                int tone02;
                float expval;
                int light;
                fread(&tone01,sizeof(int)  , 1, effectFile);
                fread(&tone02,sizeof(int)  , 1, effectFile);
                fread(&expval,sizeof(float), 1, effectFile);
                fread(&light ,sizeof(int)  , 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_DuoTone);
                dynamic_cast<CIVRDuoToneFilter*>(filterArray[i])->IVR_ToneChannel01 =tone01;
                dynamic_cast<CIVRDuoToneFilter*>(filterArray[i])->IVR_ToneChannel02 =tone02;
                dynamic_cast<CIVRDuoToneFilter*>(filterArray[i])->IVR_ExpValue      =expval;
                dynamic_cast<CIVRDuoToneFilter*>(filterArray[i])->IVR_LightIntensity=light;
            }break;
            case IVR_Image_Filter_Warm     :
            {
                IVR_AddFilter(IVR_Image_Filter_Warm);
            }break;
            case IVR_Image_Filter_Cold     :
            {
                IVR_AddFilter(IVR_Image_Filter_Cold);
            }break;
            case IVR_Image_Filter_Gotham   :
            {
                IVR_AddFilter(IVR_Image_Filter_Gotham);
            }break;
            case IVR_Image_Filter_Sharpen  :
            {
                float expval;
                fread(&expval,sizeof(float), 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Sharpen);
                dynamic_cast<CIVRSharpenFilter*>(filterArray[i])->IVR_ExpValue = expval;
            }break;
            case IVR_Image_Filter_Detail   :
            {
                float sigmas;
                float sigmar;
                fread(&sigmas ,sizeof(float), 1, effectFile);
                fread(&sigmar ,sizeof(float), 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Detail);
                dynamic_cast<CIVRDetailFilter*>(filterArray[i])->IVR_SigmaS =sigmas;
                dynamic_cast<CIVRDetailFilter*>(filterArray[i])->IVR_SigmaR =sigmar;
            }break;
            case IVR_Image_Filter_Invert   :
            {
                IVR_AddFilter(IVR_Image_Filter_Invert);
            }break;
            case IVR_Image_Filter_Stylize  :
            {
                float sigmas;
                float sigmar;
                fread(&sigmas ,sizeof(float), 1, effectFile);
                fread(&sigmar ,sizeof(float), 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Stylize);
                dynamic_cast<CIVRStylizeFilter*>(filterArray[i])->IVR_SigmaS =sigmas;
                dynamic_cast<CIVRStylizeFilter*>(filterArray[i])->IVR_SigmaR =sigmar;
            }break;
            case IVR_Image_Filter_Ghost  :
            {
                int colorR;
                int colorG;
                int colorB;
                int transp;
                fread(&colorR ,sizeof(int), 1, effectFile);
                fread(&colorG ,sizeof(int), 1, effectFile);
                fread(&colorB ,sizeof(int), 1, effectFile);
                fread(&transp ,sizeof(int), 1, effectFile);

                IVR_AddFilter(IVR_Image_Filter_Ghost);
                dynamic_cast<CIVRGhostFilter*>(filterArray[i])->IVR_R            =colorR;
                dynamic_cast<CIVRGhostFilter*>(filterArray[i])->IVR_G            =colorG;
                dynamic_cast<CIVRGhostFilter*>(filterArray[i])->IVR_B            =colorB;
                dynamic_cast<CIVRGhostFilter*>(filterArray[i])->IVR_Transparency =transp;
            }break;
            }
        }
    }

    fclose(effectFile);
    return true;
}

bool CIVRImageHandler::IVR_LoadImageW64(QString pFullPath)
{
    UMat tImageLoaded;
    UMat tImageConverted;

    // To avoid log spam, make sure it exists before doing anything else.
    if (!QFile::exists(pFullPath)){	return false; }

    QFile w64buffer(pFullPath);

    if(w64buffer.open(QIODevice::ReadOnly))
    {

        QByteArray w64EncodedBuffer = w64buffer.readAll();

        QByteArray pDecodedImage = QByteArray::fromBase64(w64EncodedBuffer, QByteArray::Base64Encoding);

        vector<char> dataC(pDecodedImage.data(),pDecodedImage.data() + pDecodedImage.size());

        imdecode(dataC,IMREAD_UNCHANGED).copyTo(tImageLoaded);

        if(tImageLoaded.channels()<3)
        {
           tImageLoaded.convertTo(tImageConverted, CV_8UC4);
           cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
        }
        else if(tImageLoaded.channels()==3)
        {
           tImageLoaded.convertTo(tImageConverted, CV_8UC4);
           cvtColor( tImageConverted,tImageConverted, COLOR_RGB2BGRA );
        }
        else
        {
            tImageLoaded.convertTo(tImageConverted, CV_8UC4);
            cvtColor( tImageConverted,tImageConverted, COLOR_BGR2BGRA );
        }

        IVR_PixelSize = 4;
        IVR_Size = QVector2D(tImageConverted.cols, tImageConverted.rows);

        //Set the Image Data
        tImageConverted.copyTo(IVR_ImageData);
    }
    else
    {
        return false;
    }

    return true;
}

bool CIVRImageHandler::IVR_LoadImageBUF(const char * pIVRBUFEncodedBuffer, int pIVRBUFSize)
{
    UMat tImageLoaded;
    UMat tImageConverted;

    if(pIVRBUFSize > 0)
    {
        QByteArray pDecodedImage = QByteArray::fromRawData(pIVRBUFEncodedBuffer,pIVRBUFSize);

        vector<char> dataC(pDecodedImage.data(),pDecodedImage.data() + pDecodedImage.size());

         imdecode(dataC,IMREAD_UNCHANGED).copyTo(tImageLoaded);

        int channels = tImageLoaded.channels();

        if(channels<3)
        {
           if(channels==1)
           {
               tImageLoaded.convertTo(tImageConverted, CV_8UC4);
               cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
           }
           else
           {
              tImageLoaded.convertTo(tImageConverted, CV_8UC4);
              cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
           }
        }
        else if(channels==3)
        {
           tImageLoaded.convertTo(tImageConverted, CV_8UC4);
           cvtColor( tImageConverted,tImageConverted, COLOR_RGB2BGRA );
        }
        else
        {
            tImageLoaded.convertTo(tImageConverted, CV_8UC4);
            cvtColor( tImageConverted,tImageConverted, COLOR_RGBA2BGRA );
        }

        IVR_Size = QVector2D(tImageConverted.cols, tImageConverted.rows);

        //Set the Logic Image Data
        tImageConverted.copyTo(IVR_ImageData);
    }
    else
    {
        return false;
    }

    return true;
}

bool CIVRImageHandler::IVR_LoadImageBUF(const char * pIVRBUFEncodedBuffer, int pIVRWidth, int pIVRHeight, int pIVRPixelSize)
{
    UMat tImageLoaded;
    UMat tImageConverted;

    if((pIVRWidth * pIVRHeight) > 0)
    {
         Mat(pIVRHeight , pIVRWidth, CV_8UC4 , (char*)pIVRBUFEncodedBuffer).copyTo(tImageLoaded);

        int channels = tImageLoaded.channels();

        if(channels<3)
        {
           if(channels==1)
           {
               tImageLoaded.convertTo(tImageConverted, CV_8UC4);
               cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
           }
           else
           {
              tImageLoaded.convertTo(tImageConverted, CV_8UC4);
              cvtColor( tImageConverted,tImageConverted, COLOR_GRAY2BGRA );
           }
        }
        else if(channels==3)
        {
           tImageLoaded.convertTo(tImageConverted, CV_8UC4);
           cvtColor( tImageConverted,tImageConverted, COLOR_RGB2BGRA );
        }
        else
        {
            tImageLoaded.convertTo(tImageConverted, CV_8UC4);
            cvtColor( tImageConverted,tImageConverted, COLOR_RGBA2BGRA );
        }

        IVR_Size = QVector2D(tImageConverted.cols, tImageConverted.rows);
        IVR_PixelSize = pIVRPixelSize;

        //Set the Logic Image Data
        tImageConverted.copyTo(IVR_ImageData);
    }
    else
    {
        return false;
    }

    return true;
}

bool CIVRImageHandler::IVR_LoadImageW64(const char * pIVRW64EncodedBuffer, int pIVRW64Size)
{
    UMat tImageLoaded;
    UMat tImageConverted;

    if(pIVRW64Size > 0)
    {
        QByteArray pDecodedImage = QByteArray::fromBase64(QByteArray::fromRawData(pIVRW64EncodedBuffer,pIVRW64Size), QByteArray::Base64Encoding);

        vector<char> dataC(pDecodedImage.data(),pDecodedImage.data() + pDecodedImage.size());

         imdecode(dataC,IMREAD_UNCHANGED).copyTo(tImageLoaded);

        tImageLoaded.convertTo(tImageConverted, CV_8UC4);

        cvtColor( tImageConverted,tImageConverted, COLOR_BGR2BGRA );

        IVR_Size = QVector2D(tImageConverted.cols, tImageConverted.rows);

        //Set the Logic Image Data
        tImageConverted.copyTo(IVR_ImageData);
    }
    else
    {
        return false;
    }

    return true;
}

void CIVRImageHandler::IVR_SetBrightness(int pIVRBrightness)
{
    UMat tImageChanged;
    IVR_Brightness = pIVRBrightness;
    IVR_ImageData.convertTo(tImageChanged,-1,IVR_Contrast,IVR_Brightness);
    //Set the Logic Image Data
    tImageChanged.copyTo(IVR_ImageData);
}

void CIVRImageHandler::IVR_SetContrast(double pIVRContrast)
{
    UMat tImageChanged;
    IVR_Contrast = pIVRContrast;
    IVR_ImageData.convertTo(tImageChanged,-1,IVR_Contrast,IVR_Brightness);
    //Set the Logic Image Data
    tImageChanged.copyTo(IVR_ImageData);
}

void CIVRImageHandler::IVR_SetSize(int pDAIWidth ,int pDAIHeight)
{
    UMat tImageResized;
    IVR_Size = QVector2D(pDAIWidth,pDAIHeight);
    resize(IVR_ImageData, tImageResized, Size(pDAIWidth, pDAIHeight));
    //Set the Logic Image Data
    tImageResized.copyTo(IVR_ImageData);
}

void CIVRImageHandler::IVR_AddFilter(int pIVRFilterType)
{
    switch(pIVRFilterType)
    {
    case IVR_Image_Filter_Cartoon  :{filterArray.push_back(new CIVRCartoonFilter());}break;
    case IVR_Image_Filter_Histogram:{filterArray.push_back(new CIVRImageFilter(IVR_Image_Filter_Histogram));}break;
    case IVR_Image_Filter_Binary   :{filterArray.push_back(new CIVRBinaryFilter());}break;
    case IVR_Image_Filter_Crop     :{filterArray.push_back(new CIVRImageFilter(IVR_Image_Filter_Crop));}break;
    case IVR_Image_Filter_Contours :{filterArray.push_back(new CIVRImageFilter(IVR_Image_Filter_Contours));}break;
    case IVR_Image_Filter_Shapes   :{filterArray.push_back(new CIVRImageFilter(IVR_Image_Filter_Shapes));}break;
    case IVR_Image_Filter_Dilate   :{filterArray.push_back(new CIVRDilateFilter());}break;
    case IVR_Image_Filter_OldFilm  :{filterArray.push_back(new CIVROldFilmFilter());}break;
    case IVR_Image_Filter_PencilSkt:{filterArray.push_back(new CIVRPencilSketchFilter());}break;
    case IVR_Image_Filter_Sepia    :{filterArray.push_back(new CIVRSepiaFilter());}break;
    case IVR_Image_Filter_Emboss   :{filterArray.push_back(new CIVREmbossFilter());}break;
    case IVR_Image_Filter_DuoTone  :{filterArray.push_back(new CIVRDuoToneFilter());}break;
    case IVR_Image_Filter_Warm     :{filterArray.push_back(new CIVRWarmFilter());}break;
    case IVR_Image_Filter_Cold     :{filterArray.push_back(new CIVRColdFilter());}break;
    case IVR_Image_Filter_Gotham   :{filterArray.push_back(new CIVRGothamFilter());}break;
    case IVR_Image_Filter_Sharpen  :{filterArray.push_back(new CIVRSharpenFilter());}break;
    case IVR_Image_Filter_Detail   :{filterArray.push_back(new CIVRDetailFilter());}break;
    case IVR_Image_Filter_Invert   :{filterArray.push_back(new CIVRInvertFilter());}break;
    case IVR_Image_Filter_Stylize  :{filterArray.push_back(new CIVRStylizeFilter());}break;
    case IVR_Image_Filter_Ghost    :{filterArray.push_back(new CIVRGhostFilter());}break;
    }
}

void CIVRImageHandler::IVR_SetFilterParameters(float pIVR_Treshold01,
                                               float pIVR_Treshold02,
                                               int   pIVR_ApertureSize,
                                               bool  pIVR_UseL2Gradient,
                                               int   pIVR_KernelSize,
                                               float pIVR_SigmaS,
                                               float pIVR_SigmaR,
                                               int   pIVR_Iteractions,
                                               int   pIVR_LightIntensity,
                                               int   pIVR_ToneChannel01,
                                               int   pIVR_ToneChannel02,
                                               float pIVR_ExpValue,
                                               float pIVR_ShadeFactor,
                                               bool  pIVR_ColorOutput,
                                               int   pIVR_R,
                                               int   pIVR_G,
                                               int   pIVR_B,
                                               int   pIVR_Transparency)
{

    //Aperture Size must be allways 3! when OpenCV Solve this we can unlock it...
    pIVR_ApertureSize = 3;

    for(auto filter:filterArray)
    {
        switch(filter->IVR_FilterType)
        {
        case IVR_Image_Filter_Cartoon  :
        {
            dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_ApertureSize  = pIVR_ApertureSize;
            dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_KernelSize    = pIVR_KernelSize;
            dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_Treshold01    = pIVR_Treshold01;
            dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_Treshold02    = pIVR_Treshold02;
            dynamic_cast<CIVRCartoonFilter*>(filter)->IVR_UseL2Gradient = pIVR_UseL2Gradient;
        }break;
        case IVR_Image_Filter_Histogram:{/*No Filter*/}break;
        case IVR_Image_Filter_Binary   :
        {
            //No Parameter
        }break;
        case IVR_Image_Filter_Crop     :{/*No Filter*/}break;
        case IVR_Image_Filter_Contours :{/*No Filter*/}break;
        case IVR_Image_Filter_Shapes   :{/*No Filter*/}break;
        case IVR_Image_Filter_Dilate   :
        {
            dynamic_cast<CIVRDilateFilter*>(filter)->IVR_Iteractions = pIVR_Iteractions;
            dynamic_cast<CIVRDilateFilter*>(filter)->IVR_KernelSize  = pIVR_KernelSize;
        }break;
        case IVR_Image_Filter_OldFilm  :
        {
            dynamic_cast<CIVROldFilmFilter*>(filter)->IVR_ExpValue   = pIVR_ExpValue;
            dynamic_cast<CIVROldFilmFilter*>(filter)->IVR_Treshold01 = pIVR_Treshold01;
        }break;
        case IVR_Image_Filter_PencilSkt:
        {
            dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_ColorOutput = pIVR_ColorOutput;
            dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_ShadeFactor = pIVR_ShadeFactor;
            dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_SigmaS      = pIVR_SigmaS;
            dynamic_cast<CIVRPencilSketchFilter*>(filter)->IVR_SigmaR      = pIVR_SigmaR;
        }break;
        case IVR_Image_Filter_Sepia    :
        {
            //No Parameter
        }break;
        case IVR_Image_Filter_Emboss   :
        {
            dynamic_cast<CIVREmbossFilter*>(filter)->IVR_KernelSize = pIVR_KernelSize;
        }break;
        case IVR_Image_Filter_DuoTone  :
        {
            dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_ToneChannel01  = pIVR_ToneChannel01;
            dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_ToneChannel02  = pIVR_ToneChannel02;
            dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_ExpValue       = pIVR_ExpValue;
            dynamic_cast<CIVRDuoToneFilter*>(filter)->IVR_LightIntensity = pIVR_LightIntensity;
        }break;
        case IVR_Image_Filter_Warm     :
        {
            //No Parameter
        }break;
        case IVR_Image_Filter_Cold     :
        {
            //No Parameter
        }break;
        case IVR_Image_Filter_Gotham   :
        {
            //No Parameter
        }break;
        case IVR_Image_Filter_Sharpen  :
        {
            dynamic_cast<CIVRSharpenFilter*>(filter)->IVR_ExpValue = pIVR_ExpValue;
        }break;
        case IVR_Image_Filter_Detail   :
        {
            dynamic_cast<CIVRDetailFilter*>(filter)->IVR_SigmaS = pIVR_SigmaS;
            dynamic_cast<CIVRDetailFilter*>(filter)->IVR_SigmaR = pIVR_SigmaR;
        }break;
        case IVR_Image_Filter_Invert   :
        {
            //No Parameter
        }break;
        case IVR_Image_Filter_Stylize  :
        {
            dynamic_cast<CIVRStylizeFilter*>(filter)->IVR_SigmaS = pIVR_SigmaS;
            dynamic_cast<CIVRStylizeFilter*>(filter)->IVR_SigmaR = pIVR_SigmaR;
        }break;
        case IVR_Image_Filter_Ghost  :
        {
            dynamic_cast<CIVRGhostFilter*>(filter)->IVR_R = pIVR_R;
            dynamic_cast<CIVRGhostFilter*>(filter)->IVR_G = pIVR_G;
            dynamic_cast<CIVRGhostFilter*>(filter)->IVR_B = pIVR_B;
            dynamic_cast<CIVRGhostFilter*>(filter)->IVR_Transparency = pIVR_Transparency;
        }break;
        }
    }
}

bool CIVRImageHandler::IVR_ApplyFilters()
{
    UMat FilterInput;
    IVR_ImageData.copyTo(FilterInput);

    for(auto filter:filterArray)
    {
        filter->IVR_ApplyFilter(IVR_ImageData).copyTo(FilterInput);
        FilterInput.copyTo(IVR_ImageData);
    }
    return true;
}

void CIVRImageHandler::IVR_CleanUpFilters()
{
    filterArray.clear();
    filterArray.shrink_to_fit();
}
