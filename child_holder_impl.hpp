/*!
 * Here is an implementation of template methods of child holder (from child_holder.hpp)
 */

#pragma once

#include <vector>

#include "child_holder.hpp"
#include "exception.hpp"

namespace NWbMqtt {
    

     
template <typename TThis, typename TChild>
TChild *TChildHolderBase<TThis, TChild>::CreateChildByName(const std::string &nameOfChild) {
    throw WBMQTT_EXCEPTION("You must override this method according to doc");
}


template <typename TThis, typename TChild>     
TChild *TChildHolderBase<TThis, TChild>::AddChild(TChild *newChild) {
    newChild->SetParent(static_cast<TThis*>(this));
    Children.push_back(std::unique_ptr<TChild>(newChild));
    AddAliase(newChild, newChild->Name);
    return newChild; 
}
    
template <typename TThis, typename TChild>     
template <typename TChildSubclass, typename ...Args>
TChildSubclass *TChildHolderBase<TThis, TChild>::EmplaceChild(Args ...args) {
    TChildSubclass *newChild = new TChildSubclass(static_cast<TThis*>(this), args...);
    Children.push_back(std::unique_ptr<TChild>(newChild));
    AddAliase(newChild, newChild->Name);
    return newChild; 
}

template <typename TThis, typename TChild> 
void TChildHolderBase<TThis, TChild>::RemoveChild(TChild *child) {
    Children.remove(child); // ???
    std::vector<std::string> aliasesToDelete;
    for (auto iter : aliasesOfChildren)
        if (iter.second == child)
            aliasesOfChildren.push_back(iter.first);
    for (const std::string &alias : aliasesToDelete)
        aliasesOfChildren.erase(alias);
}
    
template <typename TThis, typename TChild> 
void TChildHolderBase<TThis, TChild>::RemoveChild(const std::string &aliaseOfChild) {
    auto iter = aliasesOfChildren.find(aliaseOfChild);
    if (iter != aliasesOfChildren.end())
        RemoveChild(iter->second);
    else
        throw WBMQTT_EXCEPTION("Trying to remove a child with non-existing alias");
}
    
template <typename TThis, typename TChild> 
TChild* TChildHolderBase<TThis, TChild>::GetChild(const std::string &aliaseOfChild) {
    auto iter = aliasesOfChildren.find(aliaseOfChild);
    if (iter != aliasesOfChildren.end())
        return iter->second;
    else
        return nullptr;
}
    
template <typename TThis, typename TChild> 
TChild* TChildHolderBase<TThis, TChild>::GetChildForce(const std::string &aliaseOfChild) {
    TChild* ret = GetChild(aliaseOfChild);
    if (ret != nullptr)
        return ret;
    TChild* newChild = CreateChildByName(aliaseOfChild);
    AddChild(newChild);
    return newChild;
    
}
    
template <typename TThis, typename TChild> 
TChild &TChildHolderBase<TThis, TChild>::GetChildReference(const std::string &aliaseOfChild) {
    return *GetChildForce();
}
    
template <typename TThis, typename TChild> 
TChild &TChildHolderBase<TThis, TChild>::operator[](const std::string &aliaseOfChild) {
    return GetChildReference(aliaseOfChild);
}
    
template <typename TThis, typename TChild> 
void TChildHolderBase<TThis, TChild>::AddAliase(TChild *control, const std::string &newAliase) {
    if (aliasesOfChildren.count(newAliase) != 0)
        throw WBMQTT_EXCEPTION("Trying to add alias that already exist");
    aliasesOfChildren[newAliase] = control;
}
    
template <typename TThis, typename TChild> 
TChildHolderBase<TThis, TChild>::TChildHolderBase() {}
    
template <typename TThis, typename TChild> 
TChildHolderBase<TThis, TChild>::~TChildHolderBase() {}


    
    
};
