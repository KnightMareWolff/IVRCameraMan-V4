del ".\IVRTool.exp"
del ".\IVRTool.lib"
del ".\IVRTool.ilk"
del ".\IVRTool.pdb"
mkdir ".\sdk"
mkdir ".\sdk\include"
mkdir ".\sdk\include\opencv2"
mkdir ".\sdk\include\QtConcurrent"
mkdir ".\sdk\include\QtCore"
mkdir ".\sdk\include\QtGui"
mkdir ".\sdk\include\QtWidgets"
mkdir ".\sdk\include\ffmpeg"
mkdir ".\sdk\include\quicklz"
mkdir ".\sdk\include\Filters"
mkdir ".\sdk\include\Classifier"
mkdir ".\sdk\include\FreeLockQueue"
mkdir ".\sdk\lib"

xcopy "..\..\libs\IVRLowLevelSDK\IVRLowLevelSDK.lib" ".\sdk\lib"
xcopy "..\..\libs\ocv470\x64\vc17\lib\*.lib" ".\sdk\lib"
xcopy "..\..\libs\Qt\vc16\Qt5Concurrent.lib" ".\sdk\lib"
xcopy "..\..\libs\Qt\vc16\Qt5Core.lib" ".\sdk\lib"
xcopy "..\..\libs\Qt\vc16\Qt5Gui.lib" ".\sdk\lib"
xcopy "..\..\libs\Qt\vc16\Qt5Widgets.lib" ".\sdk\lib"
xcopy "..\..\libs\ffmpeg\lib\*.lib" ".\sdk\lib"
xcopy "..\..\libs\quicklz\lib\*.lib" ".\sdk\lib"


xcopy "..\..\IVRLowLevelSDK\*.h" ".\sdk\include"
xcopy "..\..\IVRLowLevelSDK\Filters\*.h" ".\sdk\include\Filters"
xcopy "..\..\IVRLowLevelSDK\Classifier\*.h" ".\sdk\include\Classifier"
xcopy "..\..\IVRLowLevelSDK\FreeLockQueue\*.h" ".\sdk\include\FreeLockQueue"
xcopy "..\..\libs\ocv470\include\opencv2\*" /s ".\sdk\include\opencv2"
xcopy "..\..\libs\Qt\vc16\QtConcurrent" /s ".\sdk\include\QtConcurrent"
xcopy "..\..\libs\Qt\vc16\QtCore" /s ".\sdk\include\QtCore"
xcopy "..\..\libs\Qt\vc16\QtGui" /s ".\sdk\include\QtGui"
xcopy "..\..\libs\Qt\vc16\QtWidgets" /s ".\sdk\include\QtWidgets"
xcopy "..\..\libs\ffmpeg\include\*" /s ".\sdk\include\ffmpeg"
xcopy "..\..\libs\quicklz\include\*" /s ".\sdk\include\quicklz"

D:\ProgramData\Qt\5.15.2\msvc2019_64\bin\windeployqt --release .\IVRTool.exe

xcopy "..\..\ThirdParty\*.*" "."
xcopy "..\..\ThirdParty\*.*" ".\sdk"
xcopy "..\..\libs\IVRLowLevelSDK\IVRLowLevelSDK.dll" "."
xcopy "..\..\libs\IVRLowLevelSDK\IVRLowLevelSDK.dll" ".\sdk"
xcopy ".\D3Dcompiler_47.dll" ".\sdk"
xcopy ".\libEGL.dll" ".\sdk"
xcopy ".\libGLESv2.dll" ".\sdk"
xcopy ".\opengl32sw.dll" ".\sdk"
xcopy "..\..\libs\Qt\vc16\Qt5Concurrent.dll" ".\sdk"
xcopy "..\..\libs\Qt\vc16\Qt5Concurrent.dll" "."
xcopy ".\Qt5Core.dll" ".\sdk"
xcopy ".\Qt5Gui.dll" ".\sdk"
xcopy ".\Qt5Svg.dll" ".\sdk"
xcopy ".\Qt5Widgets.dll" ".\sdk"

