#include "IVR_IFFReader.h"

CIVRIFFReader::CIVRIFFReader()
{

}



void CIVRIFFReader::IVR_AppendData(IVR_FrameData &pData,QString path)
{
    //---------------------------------------------------------------------
    //Append in the frames file a single realtime frame
    //---------------------------------------------------------------------
    frameFile = fopen(path.toStdString().c_str(),"ab");
    if (frameFile != NULL)
    {
        fwrite(&pData.IVR_CameraID  , sizeof(uint  ), 1, frameFile);
        fwrite(&pData.IVR_FrameFPS  , sizeof(uint  ), 1, frameFile);
        fwrite(&pData.IVR_FrameDT   , sizeof(float ), 1, frameFile);
        fwrite(&pData.IVR_Timestamp , sizeof(qint64), 1, frameFile);
    }

    fclose(frameFile);
}

void CIVRIFFReader::IVR_AppendData(IVR_FrameData &pData,QString root,QString cam,uint take)
{
    //---------------------------------------------------------------------
    //Append in the frames file a single realtime frame
    //---------------------------------------------------------------------
    frameFile = fopen(QString(root + "/" + cam + "Take" + QString("%1").arg(take) + ".iff").toStdString().c_str(),"ab");
    if (frameFile != NULL)
    {
        fwrite(&pData.IVR_CameraID  , sizeof(uint  ), 1, frameFile);
        fwrite(&pData.IVR_FrameFPS  , sizeof(uint  ), 1, frameFile);
        fwrite(&pData.IVR_FrameDT   , sizeof(float ), 1, frameFile);
        fwrite(&pData.IVR_Timestamp , sizeof(qint64), 1, frameFile);
    }

    fclose(frameFile);
}

bool CIVRIFFReader::IVR_ReadFrameData(QString path)
{
    int FrameIndex=0;
    bool enOfFile = false;

    filePath = path;
    //---------------------------------------------------------------------
    //Open the frame file and fill the camera Frames
    //---------------------------------------------------------------------
    frameFile = fopen(filePath.toStdString().c_str(),"r");

    if (frameFile != NULL)
    {
        while(!enOfFile)
        {
           IVR_FrameData tFrame;

           if(fread(&tFrame.IVR_CameraID   , sizeof(uint  ), 1, frameFile) == 1 &&
              fread(&tFrame.IVR_FrameFPS   , sizeof(uint  ), 1, frameFile) == 1 &&
              fread(&tFrame.IVR_FrameDT    , sizeof(float ), 1, frameFile) == 1 &&
              fread(&tFrame.IVR_Timestamp  , sizeof(qint64), 1, frameFile) == 1 )
           {
               tFrame.IVR_FrameID    = FrameIndex;

               IIFrameData.push_back(tFrame);

               FrameIndex++;
           }
           else
           {
               fclose(frameFile);
               enOfFile = true;
           }
        }
    }
    else
    {
        qWarning() << "Problems to open the Frame File!";
        return false;
    }

    return true;
}

bool CIVRIFFReader::IVR_ReadFrameData(QString root,QString cam,uint take)
{
    int FrameIndex=0;
    bool enOfFile = false;


    filePath  = root + "/" + cam + "Take" + QString("%1").arg(take) + ".iff";

    qWarning() << "Reading the File: " << filePath ;

    if(IIFrameData.size()>0)IIFrameData.clear();
    //---------------------------------------------------------------------
    //Open the frame file and fill the camera Frames
    //---------------------------------------------------------------------
    frameFile = fopen(filePath.toStdString().c_str(),"r");

    if (frameFile != NULL)
    {
        while(!enOfFile)
        {
           IVR_FrameData tFrame;

           if(fread(&tFrame.IVR_CameraID   , sizeof(uint  ), 1, frameFile) == 1 &&
              fread(&tFrame.IVR_FrameFPS   , sizeof(uint  ), 1, frameFile) == 1 &&
              fread(&tFrame.IVR_FrameDT    , sizeof(float ), 1, frameFile) == 1 &&
              fread(&tFrame.IVR_Timestamp  , sizeof(qint64), 1, frameFile) == 1 )
           {
               tFrame.IVR_FrameID    = FrameIndex;

               IIFrameData.push_back(tFrame);

               FrameIndex++;
           }
           else
           {
               fclose(frameFile);
               enOfFile = true;
           }
        }
    }
    else
    {
        qWarning() << "Problems to open the Frame File!";
        return false;
    }

    return true;
}

uint CIVRIFFReader::IVR_GetAverageFPS()
{

    int  FrameIndex = 0;
    uint tFramesFPS = 0;

    for(auto &frame : IIFrameData)
    {
        tFramesFPS += frame.IVR_FrameFPS;
        FrameIndex++;
    }

    tFramesFPS = tFramesFPS / (FrameIndex + 1);

    qWarning() << "Averaged FPS Identifyed: " << tFramesFPS ;

    return tFramesFPS;
}

uint CIVRIFFReader::IVR_GetBestFPS()
{
    // sort by the less distance
    struct
    {
         bool operator()(IVR_FrameData a, IVR_FrameData b) const
         {
             return a.IVR_FrameFPS > b.IVR_FrameFPS;
         }
    } GreaterOccours;

    vector<IVR_FrameData> fpsFrameData = IIFrameData;

    //Sort the final vector taking account the FPS occurencies
    std::sort(fpsFrameData.begin(), fpsFrameData.end(), GreaterOccours);

    //The Best FPS :)
    return fpsFrameData[0].IVR_FrameFPS;
}

uint CIVRIFFReader::IVR_GetFPSByTimeApprox()
{
    uint FPS     = 60;
    uint nFrames = 1;
    int  milisec = 1.000;

    QDateTime pIVR_TakeInitialTime;
    QDateTime pIVR_TakeFinalTime;

    pIVR_TakeInitialTime = QDateTime::fromMSecsSinceEpoch(IIFrameData[0].IVR_Timestamp);
    pIVR_TakeFinalTime   = QDateTime::fromMSecsSinceEpoch(IIFrameData[IIFrameData.size()-2].IVR_Timestamp);

    QDateTime pIVR_Elapsed;
    quint64   pIVR_TakeElapsedTime = 0;

    pIVR_Elapsed.fromMSecsSinceEpoch(pIVR_TakeInitialTime.msecsTo(pIVR_TakeFinalTime));

    pIVR_TakeElapsedTime = pIVR_Elapsed.toSecsSinceEpoch()/1000;

    nFrames = IIFrameData.size();
    FPS     = nFrames / pIVR_TakeElapsedTime;

    //Check if the timestamped time match with the calculated time and try approximate them
    qWarning() << "Final FPS Identifyed: " << FPS << "Final Elapsed Time: " << pIVR_TakeElapsedTime << "Number of Frames: " << nFrames;

    return FPS;
}

float CIVRIFFReader::IVR_GetFrameMiliseconds(uint FPS)
{
    uint  nFrames = 1;
    int   milisec = 1.000;
    float frame_dts = 0.0f;

    nFrames = IIFrameData.size();

    qWarning() << "FPS Received for Time Approx: " << FPS ;
    qWarning() << "Number of Frames Identifyed : " << nFrames ;

    //This is the system actual resultant video Time(How much longer it play)
    if(FPS != 0 && nFrames > 0)
    {
       frame_dts = (float)((nFrames / FPS) * milisec);
    }

    qWarning() << "Calculated Time For each Frame: " << frame_dts ;

    return frame_dts;
}
