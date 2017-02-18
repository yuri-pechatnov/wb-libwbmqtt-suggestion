/*!
 * TControl is declared here
 * Synonyms in documentation: TControl, Control, сущность контрол/поле, контрол.
 */
 
 
namespace NWbMqtt {

/*!
 *  Class representing control.
 *  It's useful to know exactly type of control holder (supposed Device).
 *  So using template.
 */
template <typename TControlHolder_, typename TControl_>
class TControlBase : public THierarchyMemberBase<TControlHolder_, TControl_, TArgNoChildOrParent, TArgHavingValue> {
  public:
    /// Supposed to be Device or ControlGroup type 
    typedef TControlHolder_ TControlHolder;
    typedef TControl_ TControl;
  protected:
    
    /// Description of control.
    /// I hope it will be supported somewhen.
    TArgDescription ControlDescription;
    
    /// Type of control (temperature, humidity, ...)
    const TControlType *ControlType; 
    
    /// Order of control in homeui
    TArgOrder Order;
    
    /// Max value
    TArgMaxValue MaxValue;
    
    /// This flag defines will control do updates to mqtt while contruction and destruction or not
    TArgCreatingInMqtt isCreatingInMqtt;
    
    
    
    virtual void TryDelegateMessageToChildren(const TMessage &message) override { return MESSAGE_REJECTED; }
    
    /*!
     *  When message is got
     *  changes value on control.
     */
    virtual void OnValueChange(const TTopic &topic, const String &newValue) override;


    /*!
     *  Sets control type.
     *  If control name is empty, then it assigns it with DefaultName.
     *  If control description is empty it assigns it with DefaultDescription.
     *  If control order is uninitialized then is assigns it with DefaultOrder
     */
    void SetOne(const TControlType *controlType);
    
    /// Sets control description
    void SetOne(const TArgDescription &description); 
    
    /// Sets control order
    void SetOne(const TArgOrder &order); 
    
    /// Sets control readonly flag
    void SetOne(const TArgReadonly &readonly); 
    
    /// Sets control isAcceptingMessages flag
    void SetOne(const TArgAcceptingMessages &isAcceptingMessages);
    
    /// Do nothing
    void Set();
    
    
  public:
    
    /*!
     * Declared in TMessagingNode (immediately update in mqtt)
     */
    virtual void SetError(const String &errorMessage) override;
    /*!
     * Declared in TMessagingNode (immediately update in mqtt)
     */
    virtual void CleanError() override;
  
    
    /// Cleaning device in mqtt tree
    void PurgeControlBranch();
    
    /// Set parent, supposed to be called from AddChild(AddControl, ...) method 
    void SetParent(TControlHolder *parentDevice);
    
    /// Update/create all info in mqtt
    void UpdateAll();
    
    /*!
     *  \brief Set some fields according to parameters.
     *  For more details see SetOne functions (of base classes too), constructor, 
     *  and derived TThis (static polymorphism is used).
     */
    template <typename T, typename ...Args>
    void Set(T arg, Args ...args) {
        static_cast<TThis>(this)->SetOne(arg);
        Set(args...);
    }
    
    /*!
     * Constructor. All parameters but first can be listed in random order (some of them may be omitted).
     *  \param [in] parent - pointer to parent in hierarchy
     * 
     *  Next parameters may be given in random order (order doesn't matter)
     * 
     *  \param [in] controlType - object of TControlType (must not construct, give static object from TControlType)
     * 
     *  Next parameters are optional. See functions SetOne to better understanding.
     * 
     *  \param [in] controlDescription - object of TArgDescription (wrapper over std::string), sets shown description
     *  \param [in] isAcceptingMessages - object of TArgAcceptingMessages (wrapper over bool), sets accepting messages policy.
     *  \param [in] isReadonly - object of TArgReadonly (wrapper over bool), sets control readonly or not.
     * 
     *  Usage: TControlBase("temperature", TArgDescription("Water temperature"), TArgReadonly(true));
     * 
     *  Note: arguments will be checked for duplicates, sorted and then applied
     */
    template <typename ...Args>
    TControlBase(TParent *parent, const std::string &controlName, Args ...args);
    
    
    /*!
     * Constructor. See previous one.
     * The only thing to add is that 'controlName' field is taken by default from controlType.
     */
    template <typename ...Args>
    TControlBase(TParent *parent, Args ...args);
    
    /*!
     * Destructor
     */
    virtual ~TControlBase();
}
    
    
    
}; 
