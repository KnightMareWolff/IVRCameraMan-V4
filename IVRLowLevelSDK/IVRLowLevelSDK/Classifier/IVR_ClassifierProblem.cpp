#include "IVR_ClassifierProblem.h"

CIVRClassifierProblem::CIVRClassifierProblem()
{

}

bool CIVRClassifierProblem::AddProblemClass(QString pIVR_ClassDesc)
{
    int classIndex=0;
    CIVRClassifierClass* tClass;

    //We only insert a new Class if it not exists...
    if(!IVR_Exists(pIVR_ClassDesc,classIndex))
    {
        tClass = new CIVRClassifierClass();

        IVR_ProblemClasses.push_back(tClass);

        classIndex = IVR_ProblemClasses.size()-1;

        //The classes in SVM start from zero. ;)
        IVR_ProblemClasses[classIndex]->IVR_ClassID          = classIndex;
        IVR_ProblemClasses[classIndex]->IVR_ClassDescription = pIVR_ClassDesc;

        return true;
    }

    return false;
}

bool CIVRClassifierProblem::IVR_FindClass(int IVR_ClassID , int &IVR_ClassIndex  )
{
    bool foundClass=false;
    int classIndex=0;
    for(auto problemFeature : IVR_ProblemClasses)
    {
        if(problemFeature->IVR_ClassID == IVR_ClassID)
        {
            IVR_ClassIndex = classIndex;
            foundClass = true;
            break;
        }
        classIndex++;
    }

    if(foundClass)return true;
    return false;
}

bool CIVRClassifierProblem::IVR_Exists(QString pIVR_ClassDesc, int &IVR_ClassID)
{
    bool foundClass=false;
    int featureIndex=0;
    for(auto problemClass : IVR_ProblemClasses)
    {
        if(problemClass->IVR_ClassDescription == pIVR_ClassDesc)
        {
            IVR_ClassID = problemClass->IVR_ClassID;
            foundClass = true;
            break;
        }
        featureIndex++;
    }

    if(foundClass)return true;
    return false;
}
