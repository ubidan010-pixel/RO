#ifndef _ITF_NAMEGENERATOR_H_
#define _ITF_NAMEGENERATOR_H_

namespace ITF
{

    class NameGenerator
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief  Gets a generated name. 
        /// @param  _prefix The prefix.
        /// @return The generated name.
        static String getName(const String& _prefix = "");

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief  Gets a unique name.
        /// @param  _prefix The prefix.
        /// @return The generated unique name.
        static String getUniqueName(const String& _prefix = "");
    };

} // namespace ITF

#endif // _ITF_NAMEGENERATOR_H_