#include "IVR_FrameRecorder.h"

CIVRFrameRecorder::CIVRFrameRecorder()
{
   IVR_RecordingMode      = IVR_Recording_Mode_Film;
   IVR_IsRecording        = false;
   IVR_ActualTake         = 0;
   IVR_Compilations       = 0;
   IVR_haveOpenTake       = false;
   IVR_IsStabilized       = false;
   IVR_IsCompiling        = false;
   IVR_IsIddle            = false;
   IVR_CompressionEnabled = true;
   IVR_EffectsEnabled     = false;
   IVR_EffectsName        = "";
   IVR_ElapsedTime        = 0;
   IVR_FrameLimit         = 0;
}

CIVRFrameRecorder::~CIVRFrameRecorder()
{
   for(auto take:IVR_Takes)
   {
       if(take)
       {
           delete  take;
           take=nullptr;
       }
   }
   IVR_Takes.clear();
   IVR_Takes.shrink_to_fit();
}

bool CIVRFrameRecorder::IVR_RecordBuffer(const IVR_RenderBuffer &pIVR_FrameBuffer,const IVR_FrameData &pIVR_FrameData)
{
    if(!IVR_IsRecording )return false;
    if( IVR_IsCompiling )return false;

    auto elapsed = IVR_TakeTimer.elapsed();

    if(elapsed > CIVRConfig::IVR_MSecToWait)
    {
        QtConcurrent::run([this]()
        {
            IVR_Takes[IVR_ActualTake]->IVR_StartCompile();
            CIVRFrameTake *pTake;

            pTake = new CIVRFrameTake();

            pTake->IVR_TakeID = IVR_Takes.size();

            pTake->IVR_CameraName         = IVR_CameraName;
            pTake->IVR_RecordingMode      = IVR_RecordingMode;
            pTake->IVR_EffectsEnabled     = IVR_EffectsEnabled;
            pTake->IVR_EffectsName        = IVR_EffectsName;
            pTake->IVR_CompressionEnabled = IVR_CompressionEnabled;

            IVR_ActualTake = pTake->IVR_TakeID;
            IVR_Takes.push_back(pTake);

            IVR_Takes[IVR_ActualTake]->IVR_StartRecord();

            IVR_haveOpenTake = true;

            IVR_TakeTimer.restart();
            IVR_FrameLimit=0;
        });
    }

    if(IVR_BFThread.isRunning())return false;

    IVR_BFThread = QtConcurrent::run([this,pIVR_FrameBuffer,pIVR_FrameData]()
    {
         IVR_Takes[IVR_ActualTake]->IVR_RenderQueue->enqueue(pIVR_FrameBuffer);
         IVR_Takes[IVR_ActualTake]->IVR_DataQueue  ->enqueue(pIVR_FrameData);
         IVR_FrameLimit++;
    });

    return true;
}

bool CIVRFrameRecorder::IVR_StartRecord()
{
    //If we are already recording and somehow the high-level layer leave it pass... :(
    if(!IVR_haveOpenTake)
    {
        QtConcurrent::run([this]()
        {
            CIVRFrameTake *pTake;

            pTake = new CIVRFrameTake();

            pTake->IVR_TakeID = IVR_Takes.size();

            pTake->IVR_CameraName         = IVR_CameraName;
            pTake->IVR_RecordingMode      = IVR_RecordingMode;
            pTake->IVR_EffectsEnabled     = IVR_EffectsEnabled;
            pTake->IVR_EffectsName        = IVR_EffectsName;
            pTake->IVR_CompressionEnabled = IVR_CompressionEnabled;

            IVR_ActualTake = pTake->IVR_TakeID;
            IVR_Takes.push_back(pTake);

            IVR_Takes[IVR_ActualTake]->IVR_StartRecord();

            IVR_haveOpenTake = true;
        });
    }
    else
    {
        QtConcurrent::run([this]()
        {
            IVR_Takes[IVR_ActualTake]->IVR_StartCompile();

            CIVRFrameTake *pTake;

            pTake = new CIVRFrameTake();

            pTake->IVR_TakeID = IVR_Takes.size();

            pTake->IVR_CameraName         = IVR_CameraName;
            pTake->IVR_RecordingMode      = IVR_RecordingMode;
            pTake->IVR_EffectsEnabled     = IVR_EffectsEnabled;
            pTake->IVR_EffectsName        = IVR_EffectsName;
            pTake->IVR_CompressionEnabled = IVR_CompressionEnabled;

            IVR_ActualTake = pTake->IVR_TakeID;
            IVR_Takes.push_back(pTake);

            IVR_Takes[IVR_ActualTake]->IVR_StartRecord();

            IVR_haveOpenTake = true;
        });
    }

    IVR_IsRecording = true;
    IVR_IsIddle     = false;
    IVR_TakeTimer.start();
    IVR_CameraTimer.start();
    IVR_FrameLimit=0;

    return true;
}

bool CIVRFrameRecorder::IVR_StopRecord()
{
    IVR_IsRecording = false;
    IVR_IsIddle     = false;

    if(IVR_haveOpenTake)
    {
       IVR_Takes[IVR_ActualTake]->IVR_StartCompile();
    }

    IVR_ElapsedTime=IVR_CameraTimer.elapsed();
    return true;
}

bool CIVRFrameRecorder::IVR_CompileTake()
{
    IVR_CPThread = QtConcurrent::run([this]()
    {
        IVR_IsCompiling  = true;

        //qWarning() << "----------IVR-----------------------";
        //qWarning() << "Finishing Camera Compilations       ";
        //qWarning() << "----------IVR-----------------------";
        for(int a=0;a<IVR_Takes.size();a++)
        {
            if(IVR_Takes[a]->IVR_TakeState != 0 && IVR_Takes[a]->IVR_TakeState == 1)
            {
                IVR_Takes[a]->IVR_StartCompile();
            }
        }

        //Wait all compilations finish
        for(int b=0;b<IVR_Takes.size();b++)
        {
            while(IVR_Takes[b]->IVR_TakeCPThread.isRunning());
        }

        //qWarning() << "----------IVR-----------------------";
        //qWarning() << "All Compilations Finished!          ";
        //qWarning() << "----------IVR-----------------------";

        QString IVR_CompileOutput  = CIVRConfig::IVR_RootFolder + "/" + IVR_CameraName + "Recording" + QString("%1").arg(IVR_Compilations) + ".mp4";
        QString IVR_CatBaseName    = CIVRConfig::IVR_RootFolder + "/" + IVR_CameraName + "Cat";
        //Concatenate all video takes
        int VIndex=0;
        for(const auto take:IVR_Takes)
        {
            CIVRMediaHandler* TmpMediaHandler = new CIVRMediaHandler();
            if(VIndex==0)
            {
                TmpMediaHandler->IVR_RemuxMedia(take->IVR_TakeName,IVR_CatBaseName + QString("%1").arg(VIndex) + ".mp4");
                VIndex++;
                continue;
            }

            TmpMediaHandler->IVR_CatVideo(IVR_CatBaseName + QString("%1").arg(VIndex-1) + ".mp4",
                                          take->IVR_TakeName,
                                          IVR_CatBaseName + QString("%1").arg(VIndex)   + ".mp4");
            delete TmpMediaHandler;
            VIndex++;
        }

        CIVRMediaHandler* TmpMediaHandler = new CIVRMediaHandler();
        TmpMediaHandler->IVR_RemuxMedia(IVR_CatBaseName + QString("%1").arg(VIndex-1) + ".mp4",IVR_CompileOutput);
        delete TmpMediaHandler;

        //Delete all frame files
        for(const auto take:IVR_Takes)
        {
            qWarning() << "Deleting the File: " << take->IVR_FrameName ;
            QFile deleteImageFile(take->IVR_FrameName);
            deleteImageFile.remove();
        }

        //Delete all take videos
        for(const auto take:IVR_Takes)
        {
            QFile deleteImageFile(take->IVR_TakeName);
            deleteImageFile.remove();
        }

        //Delete all concatenation videos
        for(int VIndex2=0; VIndex2<IVR_Takes.size();VIndex2++)
        {
            QFile deleteImageFile(IVR_CatBaseName + QString("%1").arg(VIndex2) + ".mp4");
            deleteImageFile.remove();
        }

        //Delete all the Takes itself
        for(auto take:IVR_Takes)
        {
            delete take;
            take = nullptr;
        }

        IVR_Takes.clear();
        IVR_Takes.shrink_to_fit();

        IVR_haveOpenTake = false;

        IVR_Compilations++;
        IVR_IsCompiling   = false;
        IVR_ActualTake    = 0;
        IVR_IsIddle       = true;
    });

    return true;
}

bool CIVRFrameRecorder::IVR_FinishRecording()
{
    if(!IVR_CPThread.isFinished())return false;
    else
        return true;
}
