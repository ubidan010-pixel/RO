#ifndef _ITF_FLEXIBLEVALUE_H_
#define _ITF_FLEXIBLEVALUE_H_

#ifndef _ITF_CORETYPES_H_   
#include "core/types.h"
#endif //_ITF_CORETYPES_H_   



namespace ITF
{

class String;
class Blob;

/// Enumeration of supported types
enum FlexibleType
{
    NOT_SET = 0,
    INTEGER_32,
    INTEGER_64,
    FLOAT_32,
    FLOAT_64,
    UNSIGNED_INTEGER_32,
    UNSIGNED_INTEGER_64,
    STRING,
    BLOB,
    ENUM_FORCE_SIZE_32(FlexibleType)
};

///
/// Utility  class that can store values of any type
///
class FlexibleValue
{
public:
    
    ///
    /// Constructor
    ///
    FlexibleValue();

    ///
    /// Constructors.
    /// @param type the type of the flexible value to create
    /// @param value the value of the flexible value.
    ///
    explicit FlexibleValue(i32 value);
    explicit FlexibleValue(u32 value);
    explicit FlexibleValue(i64 value);
    explicit FlexibleValue(u64 value);
    explicit FlexibleValue(f32 value);
    explicit FlexibleValue(f64 value);
    explicit FlexibleValue(const String& value);
    explicit FlexibleValue(const Blob& value);

    ///
    /// Copy constructor
    ///
    FlexibleValue(const FlexibleValue&);

    ///
    /// operator =
    ///
    const FlexibleValue& operator = ( const FlexibleValue& oValue );

    ///
    /// Destructor (will delete the hosted value if needed)
    ///
    ~FlexibleValue();

    ///
    /// Comparison of flexible values, asserts if type is not the same or false in release.
    ///
    bool operator==(const FlexibleValue& rhs) const;
    bool operator>(const FlexibleValue& rhs) const;
    bool operator<(const FlexibleValue& rhs) const;
    bool operator>=(const FlexibleValue& rhs) const;
    bool operator<=(const FlexibleValue& rhs) const;
    bool operator!=(const FlexibleValue& rhs) const;

    ///
    /// operator +=
    ///
    FlexibleValue& operator+=(const FlexibleValue& rhs);

    ///
    /// @return the type of hosted value
    ///
    FlexibleType getType() const {return m_type;}

    ///
    /// Sets the type for the value to host
    /// @param type the desired type
    ///
    void setType(FlexibleType type);

    ///
    /// Assigns a 32 bits integer value
    /// @param value the figure to host
    /// @throws assert if the FlexibleValue's type is not INTEGER
    ///
    void setInt32Value(i32 value);

    ///
    /// Assigns a unsigned 32 bits integer value
    /// @param value the figure to host
    /// @throws assert if the FlexibleValue's type is not UNSIGNED_INTEGER_32
    ///
    void setUnsignedInt32Value(u32 value);

    ///
    /// Assigns a 64 bits unsigned integer value
    /// @param value the figure to host
    /// @throws assert if the FlexibleValue's type is not UNSIGNED_INTEGER_64
    ///
    void setUnsignedInt64Value(u64 value);

    ///
    /// Assigns a 64 bits integer value
    /// @param value the figure to host
    /// @throws assert if the FlexibleValue's type is not INTEGER_64
    ///
    void setInt64Value(i64 value);

    ///
    /// Assigns a String
    /// @param value the String to host
    /// @throws assert if the FlexibleValue's type is not STRING
    ///
    void setStringValue(const String& value);

    ///
    /// Assigns a 32 bits float value
    /// @param value the figure to host
    /// @throws assert if the FlexibleValue's type is not FLOAT_32
    ///
    void setFloat32Value(f32 value);

    ///
    /// Assigns a 64 bits float value
    /// @param value the figure to host
    /// @throws assert if the FlexibleValue's type is not FLOAT_64
    ///
    void setFloat64Value(f64 value);

    ///
    /// Assigns a blob
    /// @param value the blob to host
    /// @throws assert if the FlexibleValue's type is not Blob
    ///
    void setBlobValue(const Blob& value);

    ///
    /// clears the type
    /// @param clearType indicates if the type of the value is to be cleared
    ///
    void clear(bool clearType = true);

    ///
    /// @return the hosted 32 bits integer
    /// @throws assert if the FlexibleValue's type is not INTEGER
    ///
    i32 getInt32Value() const;

    ///
    /// @return the hosted unsigned 32 bits integer
    /// @throws assert if the FlexibleValue's type is not UNSIGNED_INTEGER_32
    ///
    u32 getUnsignedInt32Value() const;

    /// @return the hosted 64 bits integer
    /// @throws assert if the FlexibleValue's type is not INTEGER_64
    ///
    i64 getInt64Value() const;

    ///
    /// @return the hosted 32 bits float
    /// @throws assert if the FlexibleValue's type is not FLOAT_32
    ///
    f32 getFloat32Value() const;

    ///
    /// @return the hosted 64 bits float
    /// @throws assert if the FlexibleValue's type is not FLOAT_64
    ///
    f64 getFloat64Value() const;

    ///
    /// @return the hosted 64 bits unsigned integer
    /// @throws assert if the FlexibleValue's type is not UNSIGNED_INTEGER_64
    ///
    u64 getUnsignedInt64Value() const;

    ///
    /// negate the current signed value.
    /// @throws assert if the FlexibleValue's type is not numerical signed
    ///
    void negate();

    ///
    /// @return the hosted String
    /// @throws assert if the FlexibleValue's type is not STRING
    ///
    const String& getStringValue() const;

    ///
    /// @return the hosted Blob
    /// @throws assert if the FlexibleValue's type is not BLOB
    ///
    Blob& getBlobValue() const;

    ///
    /// return a string representing the value.  
    /// NOTE:  this function formats the string in the U.S. locals
    /// @return the String
    ///
    const String asStringValue() const;

    ///
    /// return an integer representing the value  
    /// @return the returned integer
    ///
    i32 asInt32Value() const;

    ///
    /// return an integer representing the value  
    /// @return the returned integer
    ///
    u32 asUnsignedInt32Value() const;

    ///
    /// return an integer representing the value  
    /// @return the returned integer
    ///
    i64 asInt64Value() const;

    ///
    /// return an integer representing the value  
    /// @return the returned integer
    ///
    u64 asUnsignedInt64Value() const;

    ///
    /// returns a float representing the value
    /// @return the returned integer
    ///
    f32 asFloat32Value() const;

    ///
    /// returns a float representing the value
    /// @return the returned integer
    ///
    f64 asFloat64Value() const;

    ///
    /// return a flexible value which represents this type converted the newType
    /// @param newType is the expected type of the return value
    /// @return the new flexible value.
    ///
    const FlexibleValue asValue(const FlexibleType newType) const;

    ///
    /// serializes a flexible value into a blob
    /// @param offset where to serialize the data in the blob
    /// @param blob the blob in which to serialize the value
    ///
    void serialize(u32 offset, Blob& blob) const;

    ///
    /// unserializes a blob into a flexible value
    /// @param blob the blob from where to deserialize the value
    ///
    void unserialize(Blob& blob);

private:

    union
    {
        i32    m_int32;
        u32   m_uint32;
        String*        m_pString;
        i64    m_int64;
        u64   m_uint64;
        f32    m_float32;
        f64   m_float64;
        Blob*        m_pBlob;
    };
    FlexibleType    m_type;
};



} // namespace ITF

#endif // _ITF_FLEXIBLEVALUE_H_
