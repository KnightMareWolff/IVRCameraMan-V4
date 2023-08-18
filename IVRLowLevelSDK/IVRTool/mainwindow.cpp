#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    IVRLowLevelSDK* pInterf = new IVRLowLevelSDK();

    /*
    uint CamId;
    uint CamType=0;
    QString pCamName = "Test";

    LockFreeQueue<IVR_RenderBuffer,10000> IVR_CMRenderQueue;
    IVR_RenderBuffer pBuffer;
    IVR_RenderBuffer pBuffer1;
    IVR_RenderBuffer pBuffer2;
    IVR_RenderBuffer pBuffer3;

    pBuffer.IVR_Width          = 1024;
    pBuffer.IVR_Height         = 768;
    pBuffer.IVR_ColorChannels  = 4;

    //pBuffer.IVR_Buffer         = QByteArray("000000000000000000000000000000000000000000000000000000000000000000000000000000000");


    pBuffer1.IVR_Width          = 1024;
    pBuffer1.IVR_Height         = 768;
    pBuffer1.IVR_ColorChannels  = 4;

    //pBuffer1.IVR_Buffer         = QByteArray("000000000000000000000000000000000000000000000000000000000000000000000000000000000");


    pBuffer2.IVR_Width          = 1024;
    pBuffer2.IVR_Height         = 768;
    pBuffer2.IVR_ColorChannels  = 4;

    //pBuffer2.IVR_Buffer         = QByteArray("000000000000000000000000000000000000000000000000000000000000000000000000000000000");


    pBuffer3.IVR_Width          = 1024;
    pBuffer3.IVR_Height         = 768;
    pBuffer3.IVR_ColorChannels  = 4;

    //pBuffer3.IVR_Buffer         = QByteArray("000000000000000000000000000000000000000000000000000000000000000000000000000000000");

    pInterf->IVR_AddVirtualCam(CamId,CamType,0,pCamName,0);

    IVR_CMRenderQueue.push(pBuffer);
    IVR_CMRenderQueue.push(pBuffer1);
    IVR_CMRenderQueue.push(pBuffer2);
    IVR_CMRenderQueue.push(pBuffer3);

    auto p1 = IVR_CMRenderQueue.front();
    IVR_CMRenderQueue.pop();

    auto p2 = IVR_CMRenderQueue.front();
    IVR_CMRenderQueue.pop();

    auto p3 = IVR_CMRenderQueue.front();
    IVR_CMRenderQueue.pop();

    auto p4 = IVR_CMRenderQueue.front();
    IVR_CMRenderQueue.pop();

    auto p5 = IVR_CMRenderQueue.front();
    IVR_CMRenderQueue.pop();

    auto p6 = IVR_CMRenderQueue.front();
    IVR_CMRenderQueue.pop();

    pInterf->IVR_SpawnStage();
    qWarning() << "Huhuuu!";

    */

    //QString pInputVideoPath      ("media/CustomizingObjects.mp4");
    //QString pInputAuxVideoPath   ("media/RoomCustomization.mp4");
    //QString pOutputVideoPath     ("media/TestVideoOut.mp4");
    //QString pOutputVideoMixedPath   ("media/TestVideoOutMixed.mp4");
    //QString pOutputVideoMixedAuxPath("media/TestVideoAuxMixed.mp4");
    //QString pRemuxedVideoPath    ("media/TestVideoOutRemuxed.mkv");
    //QString pConcatVideoPath    ("media/TestVideoOutConcatenated.mp4");
    //QString pDemuxedVideoPath    ("media/TestVideoOutDemuxed.mp4");

    //QString pInputSoundPath("media/samba_01.mp3");
    //QString pRemuxedSoundPath ("media/samba.wav");
    //QString pRemuxedSound1Path("media/samba_01.wav");
    //QString pRemuxedSound2Path("media/samba_02.mp3");
    //QString pOutputSoundPath("media/OutputSound.wav");

    QString pInputImagePath("media/caricatura.jpg");
    QString pOutputImagePath("media/caricaturaOut.jpg");

    //QString pInputImageTransp("media/ParemetersBackground.png");
    //QString pOutputImageTransp("media/ParemetersBackgroundOut.png");

    //CIVRMediaHandler* mediaHandler = new CIVRMediaHandler();
    CIVRImageHandler* imageHandler = new CIVRImageHandler();
    //CIVRShrinker* imageShrinker = new CIVRShrinker();

    //mediaHandler->IVR_RemuxMedia(pInputSoundPath   ,pRemuxedSoundPath);
    //mediaHandler->IVR_RemuxMedia(pRemuxedSoundPath ,pRemuxedSound1Path);
    //mediaHandler->IVR_RemuxMedia(pRemuxedSound1Path,pRemuxedSound2Path);
    //mediaHandler->IVR_MuxMedia(pInputVideoPath ,pInputSoundPath,pOutputVideoMixedPath);
    //mediaHandler->IVR_DemuxMedia(pOutputVideoMixedPath,pOutputSoundPath,pDemuxedVideoPath);
    //mediaHandler->IVR_MuxMedia(pInputVideoPath ,pInputSecondarySoundPath,pOutputVideoMixedAuxPath);
    //mediaHandler->IVR_RemuxMedia(pOutputVideoMixedPath,pRemuxedVideoPath);
    //mediaHandler->IVR_CatMedia(pInputAuxVideoPath,pInputVideoPath,pOutputVideoPath);

    imageHandler->IVR_LoadImage(pInputImagePath);

    //Musg Cool Effect
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Gotham);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Sharpen);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Detail);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Stylize);

    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Cartoon);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Binary);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Dilate);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_OldFilm);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_PencilSkt);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Sepia);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Emboss);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_DuoTone);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Warm);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Cold);
    imageHandler->IVR_AddFilter(IVR_Image_Filter_Gotham);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Sharpen);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Detail);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Invert);
    //imageHandler->IVR_AddFilter(IVR_Image_Filter_Ghost);
    imageHandler->IVR_ApplyFilters();

    //int buffertam = sizeof(IVR_RenderBuffer);
    /*
    int originalSize = imageHandler->IVR_Size.x() * imageHandler->IVR_Size.y() * 4;

    if(imageShrinker->Shrink(imageHandler->IVR_ImageData.data,originalSize,1))
    {
       int    shrinkedSize  = imageShrinker->GetSize();
       uchar* pCompactImage = new uchar[shrinkedSize];

       memcpy(pCompactImage,imageShrinker->GetData(),shrinkedSize);

       if(imageShrinker->DeShrink(pCompactImage,shrinkedSize,originalSize))
       {
           //now the size have the original
           shrinkedSize  = imageShrinker->GetSize();
           uchar* pDeCompactedImage = new uchar[shrinkedSize];

           memcpy(pDeCompactedImage,imageShrinker->GetData(),shrinkedSize);

           imageHandler->IVR_LoadImageBUF((char*)pDeCompactedImage,imageHandler->IVR_Size.x(),imageHandler->IVR_Size.y(),4);
       }
    }
    */
    imageHandler->IVR_SaveImage(pOutputImagePath);
    imageHandler->IVR_CleanUpFilters();


}

MainWindow::~MainWindow()
{
}

