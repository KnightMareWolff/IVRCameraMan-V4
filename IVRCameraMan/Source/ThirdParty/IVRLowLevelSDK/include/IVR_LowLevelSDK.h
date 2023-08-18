#ifndef IVRLOWLEVELSDK_H
#define IVRLOWLEVELSDK_H

#include "IVRLowLevelSDK_global.h"
#include "IVR_ImageHandler.h"
#include "Filters/IVR_ImageFilter.h"
#include "IVR_RealCamera.h"
#include "IVR_VideoReader.h"
#include "IVR_FrameRecorder.h"
#include "IVR_FrameTake.h"
#include "IVR_MediaHandler.h"
#include "Classifier/IVR_Classifier.h"
#include "IVR_Shrinker.h"


class IVRLOWLEVELSDK_EXPORT IVRLowLevelSDK : public QObject
{
public:
    IVRLowLevelSDK(                       );
    IVRLowLevelSDK(bool bOpenOfflineThread);
   ~IVRLowLevelSDK(                       );

    bool IVR_SetRootPath  (QString &pDAIAbsolutePath);
    bool IVR_SetConfPath  (QString &pDAIAbsolutePath);
    void IVR_SetDebugMode (bool pIVR_DebbugMode     );
    void IVR_SetWaitRecord(uint pIVR_MsecToWait     );
    void IVR_SetResolution(int  pIVR_Width,int  pIVR_Height);
    void IVR_SpawnStage   (                         );

    CIVRFrameRecorder* IVR_CreateFrameRecorder(uint              pIVR_RecordingMode     ,
                                               QString          &pIVR_RecorderName      ,
                                               qint64            pIVR_Timestamp   );

    CIVRShrinker*      IVR_CreateShrinker();

    void               IVR_DestroyFrameRecorder(CIVRFrameRecorder* pIVR_Recorder);
    void               IVR_DestroyShrinker     (CIVRShrinker     * pIVR_Shrinker);

    //"Out of the Box" GUI
    void IVR_FireUpGUIThread();
    void IVR_StopGUIThread();
    bool IVR_IsGUIActive();

    //Thread Functions
    void IVR_FireGUIThread();

    //Configuration Folders
    QString IVR_RootFolder;
    QString IVR_ConfigFolder;
    //Width of the Image
    int     IVR_Width       ;
    //Height of the Image
    int     IVR_Height      ;

    //GUI Thread
    QFuture<void>         IVR_GUIThread;

    QApplication*  IVR_QtApp;
    bool           IVR_GUIActive;
    bool           IVR_PipeActive;

    bool           IVR_DebbugMode;
    uint           IVR_MSecToWait;

    QMutex          mutex;
    QWaitCondition  newdataAvailable;

    //Internal Static Config for LowLevel classes
    CIVRConfig     IVR_Config;
};

#endif // IVRLOWLEVELSDK_H
