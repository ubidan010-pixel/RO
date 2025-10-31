#ifndef _ITF_COMMAND_INL_
#define _ITF_COMMAND_INL_

namespace ITF
{

ITF_INLINE bbool Command::succeeded()const
{
    return m_bSucceed;
}

ITF_INLINE void Command::setCommandList(CommandList *_pCommandList)
{ 
    m_pCommandList = _pCommandList; 
}

ITF_INLINE CommandList* Command::getCommandList()const
{
    return m_pCommandList;
}

ITF_INLINE bbool Command::isTypeOf(const Type::Enum _type)const
{
    return _type == m_Type;
}

}

#endif // _ITF_COMMAND_INL_