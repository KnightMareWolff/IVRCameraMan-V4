
TEMPLATE = lib
DEFINES += IVRLOWLEVELSDK_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS
CONFIG  += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#defines the output location
DESTDIR = $$PWD/../libs/IVRLowLevelSDK

SOURCES += \
    IVR_BackStageUI.cpp \
    Classifier/IVR_Classifier.cpp \
    Classifier/IVR_ClassifierClass.cpp \
    Classifier/IVR_ClassifierFeature.cpp \
    Classifier/IVR_ClassifierProblem.cpp \
    IVR_Config.cpp \
    IVR_Frame.cpp \
    IVR_FrameRecorder.cpp \
    IVR_FrameTake.cpp \
    IVR_IFFReader.cpp \
    IVR_IIFReader.cpp \
    IVR_ImageHandler.cpp \
    IVR_LauncherUI.cpp \
    IVR_LowLevelSDK.cpp \
    IVR_MediaHandler.cpp \
    IVR_RealCamera.cpp \
    IVR_Shrinker.cpp \
    IVR_VideoReader.cpp \
    Filters/IVR_BinaryFilter.cpp \
    Filters/IVR_CartoonFilter.cpp \
    Filters/IVR_ColdFilter.cpp \
    Filters/IVR_DetailFilter.cpp \
    Filters/IVR_DilateFilter.cpp \
    Filters/IVR_DuoToneFilter.cpp \
    Filters/IVR_EmbossFilter.cpp \
    Filters/IVR_GhostFilter.cpp \
    Filters/IVR_GothamFilter.cpp \
    Filters/IVR_ImageFilter.cpp \
    Filters/IVR_InvertFilter.cpp \
    Filters/IVR_OldFilmFilter.cpp \
    Filters/IVR_PencilSketchFilter.cpp \
    Filters/IVR_SepiaFilter.cpp \
    Filters/IVR_SharpenFilter.cpp \
    Filters/IVR_StylizeFilter.cpp \
    Filters/IVR_WarmFilter.cpp

HEADERS += \
    FreeLockQueue/atomicops.h \
    FreeLockQueue/readerwritercircularbuffer.h \
    FreeLockQueue/readerwriterqueue.h \
    IVRLowLevelSDK_global.h \
    IVR_BackStageUI.h \
    Classifier/IVR_Classifier.h \
    Classifier/IVR_ClassifierClass.h \
    Classifier/IVR_ClassifierFeature.h \
    Classifier/IVR_ClassifierProblem.h \
    IVR_Config.h \
    IVR_Frame.h \
    IVR_FrameRecorder.h \
    IVR_FrameTake.h \
    IVR_IFFReader.h \
    IVR_IIFReader.h \
    IVR_ImageHandler.h \
    IVR_LauncherUI.h \
    IVR_LowLevelSDK.h \
    IVR_MediaHandler.h \
    IVR_RealCamera.h \
    IVR_Shrinker.h \
    IVR_VideoReader.h \
    Filters/IVR_BinaryFilter.h \
    Filters/IVR_CartoonFilter.h \
    Filters/IVR_ColdFilter.h \
    Filters/IVR_DetailFilter.h \
    Filters/IVR_DilateFilter.h \
    Filters/IVR_DuoToneFilter.h \
    Filters/IVR_EmbossFilter.h \
    Filters/IVR_GhostFilter.h \
    Filters/IVR_GothamFilter.h \
    Filters/IVR_ImageFilter.h \
    Filters/IVR_InvertFilter.h \
    Filters/IVR_OldFilmFilter.h \
    Filters/IVR_PencilSketchFilter.h \
    Filters/IVR_SepiaFilter.h \
    Filters/IVR_SharpenFilter.h \
    Filters/IVR_StylizeFilter.h \
    Filters/IVR_WarmFilter.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

#OpenCV 4.5.3 - Compiled with MSVC2019 64Bits(With "d" for debug version)
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/ocv470/x64/vc17/lib/ -lopencv_world470
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/ocv470/x64/vc17/lib/ -lopencv_world470d

INCLUDEPATH += $$PWD/../libs/ocv470/include
DEPENDPATH += $$PWD/../libs/ocv470/include

INCLUDEPATH += $$PWD/../libs/Qt/vc16
DEPENDPATH += $$PWD/../libs/Qt/vc16
#QtCore
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Cored
else:unix: LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Core

#QtGUI
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Gui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Guid
else:unix: LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Gui

#Qt Widgets
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Widgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Widgetsd
else:unix: LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Widgets

#Qt Concurrent
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Concurrent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Concurrentd
else:unix: LIBS += -L$$PWD/../libs/Qt/vc16/ -lQt5Concurrent

#FFMPEG API
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lavcodec
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lavutil
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lavdevice
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lavfilter
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lavformat
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lswresample
win32: LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lswscale

INCLUDEPATH += $$PWD/../libs/ffmpeg/include
DEPENDPATH += $$PWD/../libs/ffmpeg/include

#quickLZ API
unix|win32: LIBS += -L$$PWD/../libs/quicklz/lib/ -lquicklz150_64_1

INCLUDEPATH += $$PWD/../libs/quicklz/include
DEPENDPATH += $$PWD/../libs/quicklz/include

QT += widgets









