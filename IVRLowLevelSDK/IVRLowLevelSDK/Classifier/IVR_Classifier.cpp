#include "IVR_Classifier.h"

CIVRClassifier::CIVRClassifier()
{
   IVR_Problem = nullptr;
   IVR_NumberOfFeatures=0;
}

bool CIVRClassifier::IVR_InitializeClassifier(int pIVR_ClassifierCategory)
{
    IVR_SVM = SVM::create();
    IVR_SVM->setType(SVM::C_SVC);
    IVR_SVM->setKernel(SVM::INTER);
    IVR_SVM->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    IVR_SVM->setC(5);
    IVR_SVM->setGamma(3);
    //IVR_SVM->setNu(0.5);

    //We Spawn a Problem to Solve...
    IVR_Problem = new CIVRClassifierProblem();

    //Setup the Type of Classifier:
    //Can be any number at creation and can be used to identify your classifier
    IVR_ClassifierCategory = pIVR_ClassifierCategory;

    IVR_NumberOfFeatures=0;

    return true;
}

bool CIVRClassifier::IVR_SaveModel(QString FileName, QString RootFolder)
{
    QString FilePath;

    FilePath = RootFolder + FileName + ".model";

    IVR_SVM->save(FilePath.toStdString());

    return true;
}

bool CIVRClassifier::IVR_LoadModel(QString FileName, QString RootFolder)
{
    QString FilePath;
    FilePath = RootFolder + FileName + ".model";
    IVR_SVM = SVM::load(FilePath.toStdString());
    return true;
}

bool CIVRClassifier::IVR_LoadModel(QString AbsolutePath)
{
    IVR_SVM = SVM::load(AbsolutePath.toStdString());
    if(IVR_SVM->empty())return false;
    return true;
}

bool CIVRClassifier::IVR_Train()
{
    IVR_SVM->train(IVR_Features, ROW_SAMPLE, IVR_Labels);
    return true;
}

bool CIVRClassifier::IVR_Predict(float &predictedResult,const vector<CIVRClassifierFeature *> IVR_ClassFeatures)
{
    vector<float> tBufferData;

    float result=999.999f;

    //If the SVM is not trained, fails...
    if (!IVR_SVM->isTrained())return false;
    //If we have the same number of features at prediction, fails...
    if (IVR_ClassFeatures.size()!=IVR_NumberOfFeatures)return false;

    for(auto feature : IVR_ClassFeatures)
    {
        tBufferData.push_back(feature->IVR_FeatureValue);
    }

    IVR_PredictBuffer = Mat(tBufferData.size() / IVR_NumberOfFeatures , IVR_NumberOfFeatures , CV_32FC1, &tBufferData[0]);

    //Will be used when we need test many input classes
    //tBufferLabels.push_back(999);
    //tPredictionResult = Mat((int)tBufferLabels.size(),1, CV_32SC1, &tBufferLabels[0]);
    //if (tPredictionResult.size == 0)return false;
    //result = tPredictionResult.at<float>(0,0);

    result = IVR_SVM->predict(IVR_PredictBuffer);

    searchResults.push_back(result);

    predictedResult = result;

    return true;
}


bool CIVRClassifier::IVR_AddClassInstance(QString IVR_ClassDescription)
{
    //*******************************************************************************************************
    //If we extract the feature properly, we can fill the ML Data inside of the Classifier.
    //Warning!
    //We are only filling the IDs and Descriptions starting with 1 , since in future if we need record a file
    //for the original SVM Algorithm, we dont need worry about fix it..since in the original SVM distribution
    //All the features start with 1 in the following format:
    // ------------------------------------------------------------------------------------------------------
    // 0|1:999|2:999|3:999| --> Where the Zero is the class ID and the 999 is the feature Value. :)
    // ------------------------------------------------------------------------------------------------------
    //In our Implementation we are using the OpenCV SVM (Same from the original) , but filling an Mat Object
    //Starting from Zero(0)...(See the IVR_BuildXSpace function).
    //*******************************************************************************************************
    int classID=0;
    bool ret=false;
    if(!IVR_Problem->IVR_Exists(IVR_ClassDescription,classID))
    {
        ret = IVR_Problem->AddProblemClass(IVR_ClassDescription);
    }

    if(!ret)return false;
    return true;
}

bool CIVRClassifier::IVR_FindClass(int IVR_ClassID,int &IVR_ClassIndex)
{
    bool foundClass=false;
    int classIndex=0;
    for(auto problemClass : IVR_Problem->IVR_ProblemClasses)
    {
        if(problemClass->IVR_ClassID == IVR_ClassID)
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

bool CIVRClassifier::IVR_AddClassFeature(int IVR_ClassID,QString IVR_FeatureDescription,float IVR_FeatureValue)
{
    int  classIndex=0;
    int  featureIndex=0;
    bool featureIncluded=false;

    //If we not found the related class, a feature cannot be included...
    if(!IVR_FindClass(IVR_ClassID,classIndex))return false;

    //*******************************************************************************************************
    //If we extract the feature properly, we can fill the ML Data inside of the Classifier.
    //Warning!
    //We are only filling the IDs and Descriptions starting with 1 , since in future if we need record a file
    //for the original SVM Algorithm, we dont need worry about fix it..since in the original SVM distribution
    //All the features start with 1 in the following format:
    // ------------------------------------------------------------------------------------------------------
    // 0|1:999|2:999|3:999| --> Where the Zero is the class ID and the 999 is the feature Value. :)
    // ------------------------------------------------------------------------------------------------------
    //In our Implementation we are using the OpenCV SVM (Same from the original) , but filling an Mat Object
    //Starting from Zero(0)...(See the IVR_BuildXSpace function).
    //*******************************************************************************************************

    //We insert a new feature only if it not exists..
    if(!IVR_Problem->IVR_ProblemClasses[classIndex]->IVR_Exists(IVR_FeatureDescription,featureIndex))
    {
        if(IVR_Problem->IVR_ProblemClasses[classIndex]->AddClassFeature("F1" , IVR_FeatureValue))
        {
            featureIncluded = true;
        }
    }

    if(!featureIncluded)return false;
    return true;
}

bool CIVRClassifier::IVR_BuildXSpace()
{
    vector<float> tTrainData;
    vector<int>   tLabelData;

    qWarning() << "----------IVR-----------------------";
    qWarning() << "Building XSpace for SVM             ";
    qWarning() << "----------IVR-----------------------";

    for (int i = 0; i < IVR_Problem->IVR_ProblemClasses.size(); i++)
    {
        for (int j = 0; j < IVR_Problem->IVR_ProblemClasses[i]->IVR_ClassFeatures.size(); j++)
        {
            float featVal = IVR_Problem->IVR_ProblemClasses[i]->IVR_ClassFeatures[j]->IVR_FeatureValue;
            tTrainData.push_back(featVal);
        }
        int classLabel = IVR_Problem->IVR_ProblemClasses[i]->IVR_ClassID;
        tLabelData.push_back(classLabel);
    }

    IVR_ComputeFeaturesNbr();

    qWarning() << "----------IVR-----------------------";
    qWarning() << "Fill the XSpace with Data           ";
    qWarning() << "----------IVR-----------------------";
    IVR_Features = Mat(tTrainData.size() / IVR_NumberOfFeatures, IVR_NumberOfFeatures , CV_32FC1, &tTrainData[0]);
    IVR_Labels   = Mat((int)tLabelData.size() , 1,CV_32SC1,&tLabelData[0]);

    return true;
}

bool CIVRClassifier::IVR_ComputeFeaturesNbr()
{
    vector<int> featuresNb;

    qWarning() << "----------IVR-----------------------";
    qWarning() << "Computing feature Numbers           ";
    qWarning() << "----------IVR-----------------------";

    struct
    {
         bool operator()(int a, int b) const
         {
             return a > b;
         }
    } GreaterFeatures;

    for (int i = 0; i < IVR_Problem->IVR_ProblemClasses.size(); i++)
    {
        featuresNb.push_back(0);
        for (int j = 0; j < IVR_Problem->IVR_ProblemClasses[i]->IVR_ClassFeatures.size(); j++)
        {
            featuresNb[featuresNb.size()-1]++;
        }
    }

    std::sort(featuresNb.begin(),featuresNb.end(),GreaterFeatures);

    IVR_NumberOfFeatures = featuresNb[0];

    return true;
}

bool CIVRClassifier::IVR_CrossValidation()
{
    int hitNumber=0;
    int mistakeNumber=0;

    qWarning() << "----------IVR-----------------------";
    qWarning() << "Computing Cross Validation          ";
    qWarning() << "----------IVR-----------------------";
    for (uint i = 0; i < searchResults.size(); i++)
    {
        if (searchResults[i] == -1)mistakeNumber++;
        else
            hitNumber++;
    }

    hitPercentage = 100.0 * hitNumber / searchResults.size();

    return true;
}
