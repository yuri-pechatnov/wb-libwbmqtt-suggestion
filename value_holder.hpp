/*!
 * TValueHolder is declared here
 * TValueWrapper also
 */


namespace NWbMqtt {

/*! 
 * Class having field std::string Value and accessors.
 */
class TValueHolder {
    template <typename T> friend class TValueWrapper;
  protected:
    std::string Value;
    
    /// Part of advanced setter. Calls SetValue.
    virtual void SetOne(const TArgValue &newValue) final;
    
    /*!
     * SetValue calls this function after value assignment. <br>
     * By default it sends update to mqtt if old and new values are different. <br>
     * You are able to override it. (For example always send updates).
     * \param [in] oldValue - value that was before assignment
     * \param [in] newValue - value that was assigned to value field
     * \todo How to name this? =(
     */
    virtual void OnSetValue(const std::string &oldValue, const std::string &newValue);
    
  public:
    /*!
     * Setter of value
     * Also all updates to mqtt should be done here 
     */
    void SetValue(const std::string &newValue) = 0;
    
    /// Setter, calls SetValue
    void operator=(const std::string &newValue);
    
    /// Getter
    template <typename T = std::string>
    T GetValue();
    
    /*!
     * \brief Returns wrapper over value. That allows to modify it.
     * Use this function for advanced modifications of value.
     * 
     * In fact it returns wrapper over Value, that makes modifications 
     * and calls IValueWrapperMaster::OnModificationFinish
     * 
     * Example of using: <br>
     * 
     * node->As<int>() ^= 1;
     * node->As<int>() = 8;
     * 
     * node->As<float>() *= 0.99;
     * 
     * node->As<int>().ModifyWith([](int n) { return n * n; } );
     * 
     * \return Wrapper over value. That allows to modify it.
     */
    template <typename T>
    TValueWrapper<T> As();
    
    TValueHolder();
    virtual ~TValueHolder();
};


class TNotValueHolder {};



/*!
 * TValueWrapper. Implements advanced modification of value in TValueHolder.
 * 
 * Represents such logic:
 * 
 * you write:
 * 
 * valueHolder[7] += 8;
 * 
 * processes like (pseudocode):
 * 
 * valueWrapper = TValueWrapper<Type>(value, valueHolder.data[7]);
 * valueHolder.data[7] = Type(valueHolder.data[7]) + 8;
 * valueHolder->OnModificationFinish();
 */
template <typename T>
class TValueWrapper 
{
    friend class TValueHolder;

    TValueHolder *Master;
    std::string &Value;
  public:

    /*!
     * Operation +=.
     * - Casts its Value from std::string to T.
     * - Then does operation.
     * - And casts it again to std::string
     * - Make assignment by call of Master->SetValue()
     */
    void operator+=(const T &addValue);

    /*!
     * Operation ^=. Acts as operation +=.
     */
    void operator^=(const T &addValue);
    
    // ...

    /*!
     *  Operator =. Make assignment by call of Master->SetValue()
     */

    void operator=(const T &newValue);

    /*!
     * Applies function to value (casted from string value) and saves result again to string value.
     */
    void ModifyWith(std::function<T(const T &)> func);
 
  protected:
    /*!
     * Constructor
     * \param [in] master - pointer to master that produces this object and is to receive signal of finishing of modification.
     * \param [in] value - reference to value string, that will be modificated. 
     */
    TValueWrapper(IValueWrapperMaster *master, std::string &value): Master(master), Value(value) {}
};






};
