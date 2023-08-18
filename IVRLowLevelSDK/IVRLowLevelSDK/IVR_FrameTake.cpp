#include "IVR_FrameTake.h"

CIVRFrameTake::CIVRFrameTake()
{
   IVR_TakeID       = 0;
   IVR_TakeState    = 0;
   IVR_FrameCounter = 0;
   IVR_FPS          = 0;
   IVR_ImageHandler = nullptr;
}

CIVRFrameTake::~CIVRFrameTake()
{
   IVR_TakeID       = 0;
   IVR_TakeState    = 0;
   IVR_FrameCounter = 0;
   IVR_FPS          = 0;

   if(IVR_RenderQueue)
   {
       delete IVR_RenderQueue;
       IVR_RenderQueue = nullptr;
   }

   if(IVR_DataQueue)
   {
       delete IVR_DataQueue  ;
       IVR_DataQueue   = nullptr;
   }

   if(IVR_EffectsEnabled)
   {
       if(IVR_ImageHandler)
       IVR_ImageHandler->IVR_CleanUpFilters();
       delete IVR_ImageHandler;
       IVR_ImageHandler=nullptr;
   }
}

void CIVRFrameTake::IVR_StartRecord()
{
    IVR_RenderQueue = new ReaderWriterQueue<IVR_RenderBuffer>();
    IVR_DataQueue   = new ReaderWriterQueue<IVR_FrameData>();

    if(!IVR_RenderQueue || !IVR_DataQueue)return;

    //Change the state to recording.
    IVR_TakeState = 1;
    IVR_TakeTimer.start();

    IVR_TakeDTThread = QtConcurrent::run(this,&CIVRFrameTake::IVR_DataLoop);
    IVR_TakeBFThread = QtConcurrent::run(this,&CIVRFrameTake::IVR_ImageLoop);
}

void CIVRFrameTake::IVR_DataLoop()
{

    //qWarning() << "======================";
    //qWarning() << "Data Loop Started";
    //qWarning() << "======================";
    while(IVR_TakeState == 1)
    {
        auto data = IVR_DataQueue->peek();
        if(data!=nullptr)
        {
           CIVRIFFReader *fReader = new CIVRIFFReader();
           fReader->IVR_AppendData(*data,CIVRConfig::IVR_RootFolder,IVR_CameraName,IVR_TakeID);
           IVR_DataQueue->pop();
           delete fReader;
        }
    }

    if(IVR_DataQueue->size_approx()>0)
    {
        while(IVR_DataQueue->size_approx()>0)
        {
            auto data = IVR_DataQueue->peek();
            if(data!=nullptr)
            {
               CIVRIFFReader *fReader = new CIVRIFFReader();
               fReader->IVR_AppendData(*data,CIVRConfig::IVR_RootFolder,IVR_CameraName,IVR_TakeID);
               IVR_DataQueue->pop();
               delete fReader;
            }
        }
    }
}

void CIVRFrameTake::IVR_ImageLoop()
{

    //qWarning() << "======================";
    //qWarning() << "Image Loop Started    ";
    //qWarning() << "======================";
    while(IVR_TakeState == 1)
    {
       //---------------------------------------------------
       //Buffer Processing
       //---------------------------------------------------
       auto buffer = IVR_RenderQueue->peek();
       if(buffer!=nullptr)
       {
          if(buffer->IVR_Width > 0 && buffer->IVR_Height > 0 && buffer->IVR_ColorChannels >0)
          {
             CIVRIIFReader *fReader = new CIVRIIFReader();
             fReader->IVR_AppendData(*buffer,CIVRConfig::IVR_RootFolder,IVR_CameraName,IVR_TakeID,IVR_FrameCounter);
             IVR_FrameCounter++;
             buffer->IVR_Buffer.release();
             buffer->IVR_Buffer = UMat();
             IVR_RenderQueue->pop();
             delete fReader;
          }
       }
    }

    if(IVR_RenderQueue->size_approx()>0)
    {
       while(IVR_RenderQueue->size_approx()>0)
       {
          //---------------------------------------------------
          //Buffer Processing
          //---------------------------------------------------
          auto buffer = IVR_RenderQueue->peek();
          if(buffer!=nullptr)
          {
             if(buffer->IVR_Width > 0 && buffer->IVR_Height > 0 && buffer->IVR_ColorChannels >0)
             {
                CIVRIIFReader *fReader = new CIVRIIFReader();
                fReader->IVR_AppendData(*buffer,CIVRConfig::IVR_RootFolder,IVR_CameraName,IVR_TakeID,IVR_FrameCounter);
                IVR_FrameCounter++;
                buffer->IVR_Buffer.release();
                buffer->IVR_Buffer = UMat();
                IVR_RenderQueue->pop();
                delete fReader;
             }
          }
       }
    }
}

uint CIVRFrameTake::IVR_GetRecFPS()
{

    CIVRIFFReader *fReader = new CIVRIFFReader();
    fReader->IVR_ReadFrameData(CIVRConfig::IVR_RootFolder,IVR_CameraName,IVR_TakeID);

    //We pass here the final Frame file to be deleted later.
    IVR_FrameName = fReader->filePath;

    uint FPSByTime   =0;
    uint FPSByRecMode=0;
    uint FinalFPS    =0;
    switch(IVR_RecordingMode)
    {
        case IVR_Recording_Mode_Film      :{FPSByRecMode = 24  ;}break;
        case IVR_Recording_Mode_TV        :{FPSByRecMode = 30  ;}break;
        case IVR_Recording_Mode_Game      :{FPSByRecMode = 60  ;}break;
        case IVR_Recording_Mode_SlowMotion:{FPSByRecMode = 120 ;}break;
    }

    FPSByTime = fReader->IVR_GetFPSByTimeApprox();

    if(FPSByTime<=FPSByRecMode)FinalFPS=FPSByRecMode;
    else
        FinalFPS=FPSByTime;

    delete fReader;

    return FinalFPS;
}

void CIVRFrameTake::IVR_StartCompile()
{
    if(IVR_TakeState==0)return;

    //Change the state to compiling.
    IVR_TakeState   = 2;
    IVR_ElapsedTime = IVR_TakeTimer.elapsed();

    IVR_TakeCPThread = QtConcurrent::run([this]()
    {
        //qWarning() << "----------IVR-------------";
        //qWarning() << "Starting Compile Take";
        //qWarning() << "----------IVR-------------";

        //-------------------------------------------------------------------------
        //Initial Recording Setup
        //-------------------------------------------------------------------------
        //qWarning() << "----------IVR----------------";
        //qWarning() << "Waiting frame Queues be empty";
        //qWarning() << "----------IVR----------------";
        //Waits for the recording Threads Stop
        while(IVR_TakeBFThread.isRunning());
        while(IVR_TakeDTThread.isRunning());

        //Record the video for each Camera we have
        uint  FrameNumber      = 0;
        uint  rec_mode         = 0;
        int   milisec          = 1.000;
        long  frame_dts        = 0.0f;

        //qWarning() << "----------IVR-----------------------";
        //qWarning() << "Setting Up Recording Paths          ";
        //qWarning() << "----------IVR-----------------------";
        IVR_TakeName  = CIVRConfig::IVR_RootFolder + "/" + IVR_CameraName + "Take" + QString("%1").arg(IVR_TakeID) + ".mp4";
        IVR_FrameName = CIVRConfig::IVR_RootFolder + "/" + IVR_CameraName + "Take" + QString("%1").arg(IVR_TakeID) + ".iff";

        //qWarning() << "----------IVR-----------------------";
        //qWarning() << "Computing Frames/Recording Values   ";
        //qWarning() << "----------IVR-----------------------";
        IVR_FPS = IVR_GetRecFPS();

        //If we not have frames to process stop here..
        if(IVR_FPS==0)
        {
            qWarning() << "----------IVR-----------------------------------";
            qWarning() << "Warning! The FPS calculation are returning Zero!";
            qWarning() << "Setting Up a default 24 FPS recording  setup    ";
            qWarning() << "----------IVR-----------------------------------";
            IVR_FPS = 24;
        }

        //Set recording Mode...
        rec_mode  = IVR_RecordingMode;
        frame_dts = (long)((IVR_FrameCounter / IVR_FPS) * milisec);

        //qWarning() << "----------IVR-----------------------";
        //qWarning() << "Computing Frame Dimensions          ";
        //qWarning() << "----------IVR-----------------------";
        //-------------------------------------------------------------------------
        //Check the Video Capture Parameters and Codecs
        //-------------------------------------------------------------------------
        int frame_width  = static_cast<int>(CIVRConfig::IVR_Width );
        int frame_height = static_cast<int>(CIVRConfig::IVR_Height);

        Size frame_size(frame_width, frame_height);

        //qWarning() << "----------IVR-----------------------";
        //qWarning() << "Setting Up Special Effects          ";
        //qWarning() << "----------IVR-----------------------";
        if(IVR_EffectsEnabled)
        {
            if(!IVR_ImageHandler)
            {
                //qWarning() << "Create New Handler";
                IVR_ImageHandler = new CIVRImageHandler();
            }
            else
            {
               //qWarning() << "Clean-Up Filters";
               IVR_ImageHandler->IVR_CleanUpFilters();
            }

            if(!IVR_EffectsName.isEmpty())
            {
               //qWarning() << "Load The Effect: " << IVR_EffectsName;
               IVR_ImageHandler->IVR_LoadEffect(IVR_EffectsName);
            }
        }

        //qWarning() << "======================================================";
        //qWarning() << "Starting Record Video : " << IVR_TakeName;
        //qWarning() << "======================================================";
        //qWarning() << "The Recording Parameters are:";
        //qWarning() << "Camera Name: " <<  IVR_CameraName;
        //qWarning() << "FPS        : " <<  IVR_FPS;
        //qWarning() << "Rec Mode   : " <<  rec_mode;
        //qWarning() << "Width      : " <<  frame_width;
        //qWarning() << "Height     : " <<  frame_height;
        //qWarning() << "Frames Read: " <<  IVR_FrameCounter+1;
        //qWarning() << "Compression: " <<  IVR_CompressionEnabled;
        //qWarning() << "======================================================";

        //Create and initialize the VideoWriter object
        cv::VideoWriter oVideoWriter(IVR_TakeName.toStdString(),
            cv::VideoWriter::fourcc('3', 'I', 'V', 'D'),
            IVR_FPS,
            frame_size,
            true);

        //If the VideoWriter object is not initialized successfully, exit the program
        if (oVideoWriter.isOpened() == false)
        {
            //It´s not possible record this video!
            qWarning() << "Fail to open Video File!";
            return;
        }

        UMat   uncompressedImage;
        //-------------------------------------------------------------------------
        //Start Record the Video
        //-------------------------------------------------------------------------
        while (true)
        {
            //qWarning() << "======================================================";
            //qWarning() << "Starting Open Buffer : " << FrameNumber;
            //qWarning() << "======================================================";
            //qWarning() << "Root          : " <<  CIVRConfig::IVR_RootFolder;
            //qWarning() << "IVR_CameraName: " <<  IVR_CameraName;
            //qWarning() << "Take ID       : " <<  IVR_TakeID;
            //qWarning() << "Frame         : " <<  FrameNumber;
            //qWarning() << "Compression   : " <<  IVR_CompressionEnabled;
            //qWarning() << "======================================================";

            CIVRIIFReader *localReader = new CIVRIIFReader();
            localReader->IVR_ReadImageData(CIVRConfig::IVR_RootFolder,IVR_CameraName,IVR_TakeID,FrameNumber,IVR_CompressionEnabled);
            if (!localReader->IVR_IsValid())
            {
                qWarning() << "Finishing the Video!";
                //Finish Write the Final Video
                oVideoWriter.release();
                break;
            }
            else
            {
                //qWarning() << "======================================================";
                //qWarning() << "Reading Frame : " << FrameNumber;
                //qWarning() << "======================================================";
                //qWarning() << "IVR_Width : "           <<  localReader->ivrBuffer.IVR_Width;
                //qWarning() << "IVR_Height: "           <<  localReader->ivrBuffer.IVR_Height;
                //qWarning() << "IVR_ColorChannels   : " <<  localReader->ivrBuffer.IVR_ColorChannels;
                //qWarning() << "IVR_ShrinkSize      : " <<  localReader->ivrBuffer.IVR_ShrinkSize;
                //qWarning() << "======================================================";

                if(IVR_EffectsEnabled)
                {
                    //qWarning() << "Loading Frame Image" ;
                    IVR_ImageHandler->IVR_LoadImageUMAT(localReader->ivrBuffer.IVR_Buffer);
                    //qWarning() << "Apply Effect" ;
                    IVR_ImageHandler->IVR_ApplyFilters();
                    //qWarning() << "Copy Back the Result" ;
                    IVR_ImageHandler->IVR_ImageData.copyTo(uncompressedImage);
                }
                else
                {
                    localReader->ivrBuffer.IVR_Buffer.copyTo(uncompressedImage);
                }
                //qWarning() << "======================================================";
                //qWarning() << "Mat Format : " ;
                //qWarning() << "======================================================";
                //qWarning() << "Channels : "   <<  uncompressedImage.channels();
                //qWarning() << "rows     : "   <<  uncompressedImage.rows;
                //qWarning() << "cols     : "   <<  uncompressedImage.cols;
                //qWarning() << "Depth    : "   <<  uncompressedImage.depth();
                //qWarning() << "======================================================";
            }

            if(CIVRConfig::IVR_DebbugMode)
            {
               putText(uncompressedImage, QString("FPS: %1").arg(IVR_FPS).toStdString(),
                        cv::Point(uncompressedImage.cols/12, uncompressedImage.rows/12 + 10), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(0, 0, 0), //font color
                        2);

               QString strRecMode;
               switch(rec_mode)
               {
                   case IVR_Recording_Mode_Film      :{strRecMode = QString("Film Recording");}break;
                   case IVR_Recording_Mode_TV        :{strRecMode = QString("TV Recording");}break;
                   case IVR_Recording_Mode_Game      :{strRecMode = QString("Game Recording");}break;
                   case IVR_Recording_Mode_SlowMotion:{strRecMode = QString("Slow Motion Recording");}break;
               }
               putText(uncompressedImage, QString(QString("Recording Mode: ") + strRecMode).toStdString(),
                        cv::Point(uncompressedImage.cols/12 , uncompressedImage.rows/12 + 50), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(0, 0, 0), //font color
                        2);
            }

            //write the video frame to the file
            oVideoWriter.write(uncompressedImage);

            //Delete the Image File
            localReader->IVR_DeleteFile();

            delete localReader;

            FrameNumber++;

            //std::this_thread::sleep_for(std::chrono::milliseconds(frame_dts));

        }

        //qWarning() << "----------IVR-------------";
        //qWarning() << "Finishing Compilation";
        //qWarning() << "----------IVR-------------";
        //-------------------------------------------------------------------------
        //Final Recording Setup
        //-------------------------------------------------------------------------
        //Clean the buffer memory
        uncompressedImage = UMat();
        IVR_TakeState     = 3;

        delete IVR_RenderQueue;
        delete IVR_DataQueue  ;
        IVR_RenderQueue = nullptr;
        IVR_DataQueue = nullptr;
        return;

    });
}
