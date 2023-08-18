#include "IVR_LowLevelSDK.h"
#include "IVR_LauncherUI.h"

CIVRLauncherUI   IVR_UILauncher;

IVRLowLevelSDK::IVRLowLevelSDK()
{
   IVR_DebbugMode  = false;
}

IVRLowLevelSDK::IVRLowLevelSDK(bool bOpenOfflineThread)
{
   Q_UNUSED(bOpenOfflineThread);
   IVR_DebbugMode  = false;
}

IVRLowLevelSDK::~IVRLowLevelSDK()
{

}

bool IVRLowLevelSDK::IVR_SetRootPath  (QString &pDAIAbsolutePath)
{
    IVR_RootFolder = QString(pDAIAbsolutePath);
    CIVRConfig::IVR_RootFolder =  QString(pDAIAbsolutePath);

    if(IVR_RootFolder.isEmpty())return false;

    return true;
}

bool IVRLowLevelSDK::IVR_SetConfPath  (QString &pDAIAbsolutePath)
{
    IVR_ConfigFolder = QString(pDAIAbsolutePath);

    CIVRConfig::IVR_ConfigFolder = QString(pDAIAbsolutePath);

    if(IVR_ConfigFolder.isEmpty())return false;

    return true;
}

void IVRLowLevelSDK::IVR_SetDebugMode(bool pIVR_DebbugMode)
{
    IVR_DebbugMode = pIVR_DebbugMode;
    CIVRConfig::IVR_DebbugMode = pIVR_DebbugMode;
}

void IVRLowLevelSDK::IVR_SetWaitRecord(uint pIVR_MsecToWait     )
{
    IVR_MSecToWait = pIVR_MsecToWait;
    CIVRConfig::IVR_MSecToWait = pIVR_MsecToWait;
}

void IVRLowLevelSDK::IVR_SetResolution(int  pIVR_Width,int  pIVR_Height)
{
    IVR_Width              = pIVR_Width;
    IVR_Height             = pIVR_Height;
    CIVRConfig::IVR_Width  = pIVR_Width;
    CIVRConfig::IVR_Height = pIVR_Height;
}

void IVRLowLevelSDK::IVR_SpawnStage()
{
    IVR_QtApp->postEvent( &IVR_UILauncher, new QEvent( QEvent::User ) );
    if(IVR_UILauncher.hasValidUI())
    {
       IVR_UILauncher.attachInterfaceInstance(this);
       IVR_UILauncher.moveToThread( IVR_QtApp->instance()->thread() );
    }
}

CIVRFrameRecorder *IVRLowLevelSDK::IVR_CreateFrameRecorder(uint              pIVR_RecordingMode     ,
                                                           QString          &pIVR_RecorderName      ,
                                                           qint64            pIVR_Timestamp)
{
    CIVRFrameRecorder *pFrameRecorder = new CIVRFrameRecorder();

    pFrameRecorder->moveToThread( IVR_QtApp->instance()->thread() );

    pFrameRecorder->IVR_CameraName    = pIVR_RecorderName;
    pFrameRecorder->IVR_RecordingMode = pIVR_RecordingMode;
    pFrameRecorder->IVR_Timestamp     = pIVR_Timestamp;
    pFrameRecorder->IVR_RootFolder    = IVR_RootFolder;

    return pFrameRecorder;
}

CIVRShrinker *IVRLowLevelSDK::IVR_CreateShrinker()
{
    CIVRShrinker *pShrinker = new CIVRShrinker();
    pShrinker->moveToThread( IVR_QtApp->instance()->thread() );
    return pShrinker;
}

void IVRLowLevelSDK::IVR_DestroyFrameRecorder(CIVRFrameRecorder* pIVR_Recorder)
{
    pIVR_Recorder->moveToThread( IVR_QtApp->instance()->thread() );
    pIVR_Recorder->deleteLater();
    delete pIVR_Recorder;
    pIVR_Recorder=nullptr;
}

void IVRLowLevelSDK::IVR_DestroyShrinker(CIVRShrinker* pIVR_Shrinker)
{
    pIVR_Shrinker->moveToThread( IVR_QtApp->instance()->thread() );
    pIVR_Shrinker->deleteLater();
    delete pIVR_Shrinker;
    pIVR_Shrinker=nullptr;
}

//------------------------------------------------------------
//GUI Thread Session
//------------------------------------------------------------
void IVRLowLevelSDK::IVR_FireUpGUIThread()
{
    // Code placed here will run in the game thread
    int   argc = 1;
    char* argv[2] = { (char*)"dummy", {} };

    IVR_QtApp = new QApplication(argc, argv);
    if(IVR_QtApp)
    {
        IVR_GUIActive = true;
        IVR_GUIThread = QtConcurrent::run(this,&IVRLowLevelSDK::IVR_FireGUIThread);
    }
}

void IVRLowLevelSDK::IVR_FireGUIThread()
{
    while(IVR_GUIActive)
    {
        IVR_QtApp->processEvents();
    }
}

void IVRLowLevelSDK::IVR_StopGUIThread()
{
    IVR_GUIActive = false;
    IVR_GUIThread.cancel();
}

bool IVRLowLevelSDK::IVR_IsGUIActive()
{
    return IVR_GUIActive;
}
