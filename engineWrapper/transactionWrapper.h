#pragma once

#include "blobWrapper.h"

public ref class transactionWrapper
{

public:
    transactionWrapper() {};

    bool wait(blobWrapper^ _blob,blobWrapper^ _result); 


}; 