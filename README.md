# IVRCameraMan-V4
Version 4.0 of the Insane Video Recorder - Camera Man

Insane Video Recorder (IVR) – Camera Man, is a code plug-in designed to help developers, artists and virtual production professionals produce videos in real time using Unreal, without the need to use a video production interface / UI for the creation of Cinematics.

For many years this type of professional, to get a video ready to be released, needs to use a UI to record their games first and use a series of other tools to cut and edit all frames of the video, as well as generating all the cinematics involved in the production.

With the evolution of Virtual Video Production methods, more and more such processes are no longer used, to make room for the generation of videos in real time, where the reproductive material is “Collected”, to be used in the production of the video.

Taking into account this evolution occurring in virtual production environments, the IVR Recorder was developed to allow these conceptual evolutions of virtual production to be available on Unreal Engine quickly and easily.

The name “Camera Man” was inspired in recording operations commanded by a Character that takes possession of the Player, just like in a Real Time Game!

The entire Plugin was developed in C ++ and the functionalities can be easily used in Blueprints, where instead of just specifying usage parameters, all the Plugin's usability was created to meet some "Use Cases", commonly used for recording videos.

&nbsp;
<ul>
 	<li>Automatic Trigger Controls for all Cameras – IVR_CameraMan , On-Crane , On-Rail Cameras.</li>
 	<li>Automatic management of recording cameras – Just include several objects and automatically the Plugin manages one by one.</li>
 	<li>Recording of several videos at the same time.</li>
 	<li>On-Crane Camera with UCurve Parameterization to control the Crane in Real-Time.</li>
 	<li>On-Rail Camera with custom velocity and Loop Management.</li>
 	<li>Camera Tracking to Follow realtime actors and objects.</li>
 	<li>Debug Rendering for the simulations and Debbug Recording with Performance Informations.</li>
</ul>

Now all the compiled third party libraries used in the Plugin ThirdParty Folder and in the LowLevel SDK are being distributed as packages to avoid missing libraries during distribution so:

For the Plugin:
Unzip the package inside of IVRCameraMan\Binaries\ThirdParty\IVRLowLevelSDK\Win64 folder to get the plugin working(Do not forget follow the intalation procedures in the website).

For the Low Level SDK:
Unzip the package on the IVRLowLevelSDK folder.

If you want create a new release of the LowLevel SDK, after compile a release version on Qt:&nbsp;
01 - Go to the IVRLowLevelSDK\IVRTool\bin folder &nbsp;
02 - Edit the cp2unr.bat and deploy.bat files with valid paths acoordingly with your installation locations. &nbsp;
03 - Execute the Deploy.bat file (This file will copy all the necessary dependencies in the bin folder for a release). &nbsp;
04 - Execute the cp2unr.bat file (this will copy the dependencies into the new released UE PluginDirectory). &nbsp;

&nbsp;
Warning!
I only recomend try rebuild and install the LowLevel compilation if you are trying distribute the Plugin in an Unsupported enviroment like Linux or OSX. 
For Windows use the packaged files and everything will be fine, since you will need recompile all other dependencies too, like OpenCV and FFmpeg.