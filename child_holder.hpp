/*!
 * TChildHolder is declared here
 */

#include <list>
#include <memory>
#include <unordered_map>

namespace NWbMqtt {
    
/*!
 * \brief This class holds children container.
 * Provides children container, accessors, aliases.
 */
template <typename TThis_, typename TChild_>
class TChildHolderBase {
  public:
    typedef TThis_ TThis;
    typedef TChild_ TChild;
  protected:
  
    /*! 
     * Storage for children, it is assumed that children are created by new
     * and then delegated to TChildHolder (or it's subclass), which will delete them during destruction
     */
    std::list<std::unique_ptr<TChild>> Children; ///< Children's container
    
  private:
    /*! 
     * Map of names (or aliases) to controls.
     * All control names (got by virtual GetChildName()) are added to aliases by default
     * 
     * May be rewritten later to faster container.
     */
    std::unordered_map<std::string, TChild*> aliasesOfChildren;
  protected:
    
    
    /*! 
     *  This method is supposed to work like a constructor of TChild.
     * 
     *  It used by GetChildReference
     *  
     *  It's introduced because it may be overrided and even be a center of creation of children
     *  (by the name determine what subclass of TChild to create and return)  
     * 
     *  This method throws exception by default.
     * 
     *  \attention Pay especial attention to names "+" and "#". Throw exceptions when got it at least.
     */
    virtual TChild *CreateChildByName(const std::string &nameOfChild);

    
  public:
  
    /*! 
     * Add child to children list,
     * add its name to aliases <br>
     * Advice: try to use EmplaceChild
     * \param [in] child - new child to add to device
     * NOTE: this child will be deleted while THierarchyMember destruction
     *       or while RemoveChild call
     * \return Input parameter. 
     * \throw If own name is "#" then throw exception. If unpredictable problem happens then throw exception.
     * \attention If exception thrown, you should care about deletion of child by you own! (Try to use EmplaceChild)
     */ 
    TChild *AddChild(TChild *child);
    
    /*!
     *  Creates new child of type TChildSubclass,
     *  calls new TChildSubclass(static_cast<TThis*>(this), args...).
     *  Adds child's pointer to internal container.
     *  Adds child's name to aliases.
     *  \param args - parameters that will be given to constructor after pointer: static_cast<TThis*>(this)
     *  \return Pointer to created child.
     *  \throw If own name is "#" then throw exception. If unpredictable problem happens then throw exception.
     */
    template <typename TChildSubclass, typename ...Args>
    TChildSubclass *EmplaceChild(Args ...args);
  
    /*!
     * This method does something like this: <br>
     *      1) Delete child <br>
     *      2) Remove child from children list and remove all it's aliases <br>
     * \param [in] control - control to remove
     */ 
    void RemoveChild(TChild *child);
    
    /*!
     * Same as RemoveChild(TChild *child), but
     * \param [in] aliaseOfChild - aliase of child that you want to delete
     */
    void RemoveChild(const std::string &aliaseOfChild);
    
    /*!
     * \param [in] aliaseOfChild - aliase of child, pointer of which you want to get.
     * \return Pointer to the child, nullptr if there is no such child
     */
    TChild* GetChild(const std::string &aliaseOfChild);
    
    /*!
     * \param [in] aliaseOfChild - aliase of child, pointer of which you want to get.
     * \return Pointer to the child.
     *  If there is no such child creates new TChild by method CreateChildByName and return it
     */
    TChild* GetChildForce(const std::string &aliaseOfChild);
    
    /*!
     *  \param [in] aliaseOfChild - aliase of child, reference of which you want to return
     *  \return Dereferenced result of GetChildForce.
     */
    TChild &GetChildReference(const std::string &aliaseOfChild);
    
    /*!
     * Same as: <br> 
     * TChild &GetChildReference(const String &aliaseOfChild);
     */
    TChild &operator[](const std::string &aliaseOfChild);
    
    /*!
     *  Add aliase of control to aliases map
     */
    void AddAliase(TChild *control, const std::string &newAliase);
    
    /*!
     *  Constructor
     */
    TChildHolderBase();
    
    /*!
     * Destructor
     */
    virtual ~TChildHolderBase();
};

class TNotChildHolder {};

}
