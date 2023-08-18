#include "IVR_Shrinker.h"

CIVRShrinker::CIVRShrinker()
{
    IVR_ShrinkedData   = nullptr;
    IVR_ShrinkedSize   = 0;
    IVR_IsShrinked     = false;
    IVR_CompressorType = IVR_Compressor_Type_QuickLZ;

    state_compress   = (qlz_state_compress *)malloc(sizeof(qlz_state_compress));
    state_decompress = (qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
}

CIVRShrinker::~CIVRShrinker()
{
    if(state_compress!=nullptr)
    {
        // Zero out both states.
        memset(state_compress  , 0, sizeof(state_compress));
        free(state_compress);
        state_compress=nullptr;
    }
    if(state_decompress!=nullptr)
    {
        memset(state_decompress, 0, sizeof(state_decompress));
        free(state_decompress);
        state_decompress=nullptr;
    }

    if(IVR_ShrinkedData!=nullptr)
    {
        delete IVR_ShrinkedData;
        IVR_ShrinkedData   = nullptr;
    }

    IVR_ShrinkedSize   = 0;
    IVR_IsShrinked     = false;
    IVR_CompressorType = IVR_Compressor_Type_QuickLZ;

}

bool CIVRShrinker::Shrink(char *pData, ulong pSize)
{
    if(IVR_ShrinkedSize>0 || IVR_ShrinkedData!=nullptr)
    {
        delete IVR_ShrinkedData;
        IVR_ShrinkedData = nullptr;
        IVR_ShrinkedSize = 0;
        IVR_IsShrinked   = false;
    }

    // Zero out both states.
    memset(state_compress  , 0, sizeof(state_compress));
    memset(state_decompress, 0, sizeof(state_decompress));

    IVR_OriginalSize = pSize;

    IVR_ShrinkedSize = pSize + 400;
    IVR_ShrinkedData = new char[IVR_ShrinkedSize];
    IVR_ShrinkedSize = qlz_compress((char*)pData, (char*)IVR_ShrinkedData, pSize , state_compress);

    //If everything goes fine...
    if(IVR_ShrinkedSize==0)
    {
        IVR_IsShrinked   = false;
        return false;
    }

    IVR_IsShrinked = true;
    return true;
}

bool CIVRShrinker::DeShrink(char *pData,ulong pOriginalSize)
{
    if(IVR_ShrinkedSize>0 || IVR_ShrinkedData!=nullptr)
    {
        delete IVR_ShrinkedData;
        IVR_ShrinkedData = nullptr;
        IVR_ShrinkedSize = 0;
        IVR_IsShrinked   = false;
    }

    // Zero out both states.
    memset(state_compress  , 0, sizeof(state_compress));
    memset(state_decompress, 0, sizeof(state_decompress));

    IVR_OriginalSize = pOriginalSize;
    IVR_ShrinkedSize = IVR_OriginalSize;
    IVR_ShrinkedData = new char[IVR_OriginalSize];

    IVR_ShrinkedSize = qlz_decompress((char*)pData, IVR_ShrinkedData , state_decompress);
    if(IVR_ShrinkedSize==0)
    {
        return false;
    }

    IVR_IsShrinked = false;

    return true;
}

char * CIVRShrinker::GetData()
{
    return IVR_ShrinkedData;
}

ulong CIVRShrinker::GetSize()
{
    return IVR_ShrinkedSize;
}

bool CIVRShrinker::IsShrinked()
{
    return IVR_IsShrinked;
}
