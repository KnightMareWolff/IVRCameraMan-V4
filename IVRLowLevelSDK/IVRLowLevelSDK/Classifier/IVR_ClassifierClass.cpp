#include "IVR_ClassifierClass.h"

CIVRClassifierClass::CIVRClassifierClass()
{

}

bool CIVRClassifierClass::AddClassFeature(QString pIVR_FeatDesc, float pIVR_FeatVal)
{
    int featureIndex=0;
    CIVRClassifierFeature* tFeature;

    tFeature = new CIVRClassifierFeature();

    IVR_ClassFeatures.push_back(tFeature);

    featureIndex = IVR_ClassFeatures.size()-1;

    IVR_ClassFeatures[featureIndex]->IVR_FeatureID          = IVR_ClassFeatures.size();
    IVR_ClassFeatures[featureIndex]->IVR_FeatureDescription = pIVR_FeatDesc;
    IVR_ClassFeatures[featureIndex]->IVR_FeatureValue       = pIVR_FeatVal;

    return true;
}

bool CIVRClassifierClass::IVR_FindFeature(int IVR_FeatureID,int &IVR_FeatureIndex)
{
    bool foundFeature=false;
    int featureIndex=0;
    for(auto problemFeature : IVR_ClassFeatures)
    {
        if(problemFeature->IVR_FeatureID == IVR_FeatureID)
        {
            IVR_FeatureIndex = featureIndex;
            foundFeature = true;
            break;
        }
        featureIndex++;
    }

    if(foundFeature)return true;
    return false;
}

bool CIVRClassifierClass::IVR_Exists(QString pIVR_FeatDesc, int &IVR_FeatureID)
{
    bool foundFeature=false;
    int featureIndex=0;
    for(auto problemFeature : IVR_ClassFeatures)
    {
        if(problemFeature->IVR_FeatureDescription == pIVR_FeatDesc)
        {
            IVR_FeatureID = problemFeature->IVR_FeatureID;
            foundFeature = true;
            break;
        }
        featureIndex++;
    }

    if(foundFeature)return true;
    return false;
}
