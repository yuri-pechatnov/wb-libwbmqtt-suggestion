/*!
 * TParentHolder is declared here
 */


namespace NWbMqtt {


/*!
 * \brief This class holds Parent field.
 * Holds Parent field and provides getter and setter.
 */
template <typename TThis_, typename TParent_>
class TParentHolderBase {
  public:
    typedef TThis_ TThis;
    typedef TParent_ TParent;
  protected:
    TParent *Parent; ///< Parent field
  public:
    /*! 
     * Parent getter
     * 
     * \note There is a template function  that allows to obtain pointer to the root of hierarchy very fast.
     */
    TParent *GetParent();
     
    /*! 
     * Parent setter
     */
    void SetParent(TParent *newParent);
    
    TParentHolderBase(TParent *newParent);
};

class TNotParentHolder {};


class TArgNoChildOrParent;
class TNotParentHolder;


/*!
 * This class allows to recursively get a type of root of hierarchy. <br>
 * Supports: <br>
 * Standart hierarchy: TDriver -/> TDevice -/> TControl (obtains TDriver) <br>
 * Degenerated hierarchy: ... -\> TNode -\> TNode -\> ... (obtains TNode)
 */
template<typename T, typename TParent = typename T::TParent>
struct get_root_type : std::conditional<
            std::is_same<TArgNoChildOrParent, TParent>::value,
            T,
            typename get_root_type<TParent>::type
        >
{};

template <typename T>
struct get_root_type<T, T> {
    typedef T type;
};

template <>
struct get_root_type<TArgNoChildOrParent, TArgNoChildOrParent> {
    typedef TArgNoChildOrParent type;
};





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
template <typename T, typename TRoot = typename get_root_type<T>::type>
inline TRoot* GetRootOfParentHolder(
        T *member,
        typename std::enable_if<
            !std::is_base_of<TNotParentHolder, T>::value &&
            !std::is_same<T, typename T::TParent>::value
        >::type *dummy = 0
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
        typename std::enable_if<std::is_same<T, typename T::TParent>::value>::type *dummy = 0
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
        typename std::enable_if<std::is_base_of<TNotParentHolder, T>::value>::type *dummy = 0
)
{
    return member;
}
    
};
