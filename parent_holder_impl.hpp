/*!
 * Here is an implementation of template methods of parent holder (from parent_holder.hpp)
 */

#pragma once

#include "parent_holder.hpp"


/*!
 * TParentHolder is declared here
 */


namespace NWbMqtt {



template <typename TThis, typename TParent>
TParent *TParentHolderBase<TThis, TParent>::GetParent() 
{
    return this->Parent;
}
     
template <typename TThis, typename TParent>
void TParentHolderBase<TThis, TParent>::SetParent(TParent *newParent) {
    this->Parent = newParent;
}
    
template <typename TThis, typename TParent>
TParentHolderBase<TThis, TParent>::TParentHolderBase(TParent *parent): Parent(parent) {}


/*!
 * This function extends functionality of TParentHolder.
 * It allows to get a root of hierarchy in almost no time.
 * It is equal to member->GetParent()->...->GetParent() (as many GetParent as possible).
 * 
 * It is written apart of class TParentHolder because of complicated dependencies between TParentHolder and derived classes.
 * 
 * \param [in] member - A pointer to member of hierarchy. The only argument you should give.
 * \return Pointer to root of hierarchy.
 */
template <typename T, typename TRoot>
inline TRoot* GetRootOfParentHolder(
        T *member,
        typename std::enable_if<
            !std::is_base_of<TNotParentHolder, T>::value &&
            !std::is_same<T, typename T::TParent>::value
        >::type *dummy
)
{
    return GetRootOfParentHolder(member->Parent);
}

/*!
 * Kind of specification of function of the same name. <br>
 * Handles a case when a hierarchy is degenerated: <br>
 * ... -\> TNode -\> TNode -\> ... (not TDriver -/> TDevice -/> TControl)
 */
template <typename T>
inline T* GetRootOfParentHolder(
        T *it,
        typename std::enable_if<std::is_same<T, typename T::TParent>::value>::type *dummy
)
{
    while (it->Parent != nullptr && it->Parent != it)
        it = it->Parent;
    return it;
}

/*!
 * Kind of specification of function of the same name.
 * Handles a case when a member is actually a root of hierarchy.
 */
template <typename T>
inline T* GetRootOfParentHolder(
        T *member,
        typename std::enable_if<std::is_base_of<TNotParentHolder, T>::value>::type *dummy
)
{
    return member;
}
    
};
