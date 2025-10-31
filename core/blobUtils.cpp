#include "precompiled_core.h"


#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_


namespace ITF
{

 void testTemplateBlob()
 {
#ifdef USE_BLOBTESTTEMPLATE
     ITF_VECTOR<String> myVString;
     ITF_VECTOR<u64> myV64;
     ITF_VECTOR<i32> myVI32;
     ITF_VECTOR<i8> myVI8;
     ITF_VECTOR<i16> myVI16;
     ITF_VECTOR<u8> myVU8;
     ITF_VECTOR<u16> myVU16;
     ITF_VECTOR<f32> myVF32;
     ITF_VECTOR<f64> myVF64;
     ITF_VECTOR<bbool> myVFBool;


     ITF_VECTOR<String> myVStringRes;
     ITF_VECTOR<u64> myV64Res;
     ITF_VECTOR<i32> myVI32Res;
     ITF_VECTOR<i8> myVI8Res;
     ITF_VECTOR<i16> myVI16Res;
     ITF_VECTOR<u8> myVU8Res;
     ITF_VECTOR<u16> myVU16Res;
     ITF_VECTOR<f32> myVF32Res;
     ITF_VECTOR<f64> myVF64Res;
     ITF_VECTOR<bbool> myVFBoolRes;

     Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);

     myVString.push_back(String("test"));
     myVString.push_back(String("test2"));
     myVString.push_back(String("test3"));

     myVU8.push_back(2);
     myVU8.push_back(5);
     myVU8.push_back(8);

     pushVector(&b,myVString);
     pushVector(&b,myV64);
     pushVector(&b,myVI32);
     pushVector(&b,myVI8);
     pushVector(&b,myVI16);
     pushVector(&b,myVU8);
     pushVector(&b,myVU16);
     pushVector(&b,myVF32);
     pushVector(&b,myVF64);
     pushVector(&b,myVFBool);

     Blob res(BLOB_READONLY);
     res.setData(b.getData(),b.getSize());

     extractVector(&b,myVStringRes);
     extractVector(&b,myV64Res);
     extractVector(&b,myVI32Res);
     extractVector(&b,myVI8Res);
     extractVector(&b,myVI16Res);
     extractVector(&b,myVU8Res);
     extractVector(&b,myVU16Res);
     extractVector(&b,myVF32Res);
     extractVector(&b,myVF64Res);
     extractVector(&b,myVFBoolRes);
#endif //USE_BLOBTESTTEMPLATE
 }

}//namespace ITF
