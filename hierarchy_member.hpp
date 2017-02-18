/*!
 * THierarchyMemberBase is declared here
 */



namespace NWbMqtt {


#define DOXYGEN

/*!
 * This is a class representing member of hierarchy (TDriver -\> TDevice -\> TControl)
 * \tparam TParent_ - type of parent (e.g. TDriver for TDevice)
 * \tparam TThis_ - type of subclass (e.g. TDevice for TDevice)
 * \tparam TChild - type of children (e.g. TControl for TDevice)
 * 
 * If it's an extreme member of hierarchy, then give TArgNoChildOrParent as parameter.
 * 
 * You can create two variants of hierarchy:
 * 
 * Standart hierarchy: TDriver -/> TDevice -/> TControl (obtains TDriver) <br>
 * Degenerated hierarchy: ... -\> TNode -\> TNode -\> ... (obtains TNode) <br>
 * 
 * \attention Never create more complicated degenerated hierarchy, e.g.: 
 * ... -\> TNode1 -\> TNode2 -\> TNode1 -\> TNode2 -\> ...
 * This framework DOES NOT support such structure.
 */
template <typename TParent_, typename TThis_, typename TChild_, typename ...TFlags>
class THierarchyMemberBase : 
#ifdef DOXYGEN
        public TParentHolderBase<TThis_, TParent_>,
        public TChildHolderBase<TThis_, TParent_>,
        public TValueHolder
#else
        public typename std::conditional<
            std::is_same<TParent_, TArgNoChildOrParent>::value,
            TParentHolderBase<TThis_, TParent_>,
            TNotParentHolder
        >::type,
        public typename std::conditional<
            std::is_same<TChild_, TArgNoChildOrParent>::value,
            TChildHolderBase<TThis_, TParent_>,
            TNotChildHolder
        >::type,
        public typename std::conditional<
            is_one_of<TArgHavingValue, TFlags...>::value, 
            TValueHolder, 
            TNotValueHolder
        >::type
#endif
{
  public:
    typedef TThis_ TThis;
    typedef TParent_ TParent;
    typedef TChild_ TChild;
  private:
    /*!
     *  This function is useless in runtime.
     *  It contains some static asserts.
     */
    static void StaticAsserts();
  protected:
    /*! 
     *  \brief Name of hierarchy member. Possible value: "+", "#" or string without characters: ('+', '#', '$', '/')
     *  \note See limitations on value.
     */
    const std::string Name;
    
    std::string Error; ///< Stores own error
    std::string *ParentError; ///< Stores error of parent of null pointer

  public:
    /// Getter for Name.
    /// If name is null returns empty string
    const std::string &GetName();

  protected: 
  
    /*!
     * SetValue calls this function after value assignment (in your code). <br>
     * By default it sends update to mqtt if old and new values are different. <br>
     * You are able to override it. (For example always send updates).
     * \param [in] oldValue - value that was before assignment
     * \param [in] newValue - value that was assigned to value field <br>
     * See difference with OnChangeValue!
     */
    virtual void OnSetValue(const std::string &oldValue, const std::string &newValue) override;
  
    //virtual TChild *CreateChildByName(const std::string &nameOfChild);
   
    /*!
     *  \brief Transfer message up (control -> device -> driver -> mqtt).
     *  It's supposed that message.value will not change, 
     *  but topic will be becoming longer going the way from relative to absolute.
     * 
     *  Supposed that this method is called by control to deliver message "up" 
     *  Supposed that this method will call Parent->ProcessMessageFromChild
     * 
     *  Logic smth like that <br>
     *  control->MessageToParent("/meta/order", "1") -> <br>
     *  device->MessageToParent("/Temperature/meta/order", "1") -> <br>
     *  driver->MessageToParent("/noolite_rx_149f/Temperature/meta/order", "1") -> <br>
     *  driver: (smth like TMQTTWrapper)->Publish(NULL, "/devices/noolite_rx_149f/Temperature/meta/order", "1", 0, true); <br>
     */
    virtual void MessageToParent(const TMessage &message);
    
    /*! 
     * See MessageToParent (do exactly that by default)
     * 
     * You may need to override this if you want to do something special, 
     * for example publish something through wildcards.
     */
    virtual void ProcessMessageFromChild(const TMessage &message);
    
    /*!
     *  \brief Transfer message down (mqtt -> driver -> device -> control). 
     *  Consists of 2 parts: <br>
     *     1) Try to delegate message to children. 
     *     2) Try to use own handlers (OnValueChange, OnSubscribe). <br>
     *  \param [in] message - input message
     */
    virtual void ProcessMessageFromParent(const TMessage &message);
    
    /*
     *!
     *   \brief Checks if message relates with this messaging node, removes one level of hierarchy from topic.
     *   \param [in] message - input message
     *   \param [out] message - input message with removed level of hierarchy (usually cut prefix from topic)
     *   \return MESSAGE_ACCEPTED if message fits and MESSAGE_REJECTED otherwise
     *    This function automatically returns MESSAGE_REJECTED if flag IsAcceptingMessages is false
     *
    virtual void CheckAndCutMessageFromParent(const TMessage &message, TMessage &cutMessage);
     */
    
    /*!
     *   Tries to delegate processing to children. <br>
     *      0) Cut's the first part of the topic, let's name it PREFIX.<br>
     *      In next points message is sent without first part.
     *      1) See if there is "+" or/and "#" in aliases. If it is, then send message to child/children with this alias.<br>
     *      2) See if there PREFIX in aliases. If it is, then send messsage to child with this alias.<br>
     * 
     *   \param [in] message - message
     *   \return If managed to delegate message to at least one child
     *      (child->ProcessMessageFromParent(message) returns MESSAGE_ACCEPTED)  
     *      returns MESSAGE_ACCEPTED and MESSAGE_REJECTED otherwise
     */
    virtual void TryDelegateMessageToChildren(const TMessage &message);
    
  public:    
    /*!
     *   Empty function by default. Override it to handle income (down) messages that change value on topic.
     *   \param [in] topic - relative (for this level of hierarchy) topic
     *   \param [in] value - value, that published somewhere
     *   \param [in] MESSAGE_REJECTED by default <br>
     *   See difference with OnSetValue!
     */
    virtual void OnValueChange(const TTopic &topic, const String &newValue);
    
    /*!
     *   Empty function by default. Override it to handle income (down) messages that tells about subscription.
     *   \param [in] topic - relative (for this level of hierarchy) topic
     *   \param [in] MESSAGE_REJECTED by default
     */
    virtual void OnSubscribe(const TTopic &topic);
    
    /*!
     * Publish value to topic (acts via call of MessageToParent)
     *  \param [in] topic - relative (for this level of hierarchy) topic
     *  \param [in] value - value to publish
     */
    void Publish(const TTopic &topic, const String &value); 
    
    /*!
     *  Delete topic (acts via call of MessageToParent)
     *  \param [in] topic - relative (for this level of hierarchy) topic
     */
    void Delete(const TTopic &topic = "");
    
    /*!
     *  Subscribe to topic (acts via call of MessageToParent)
     *  \param [in] topic - relative (for this level of hierarchy) topic
     */ 
    void Subscribe(const TTopic &topic = ""); 
    
    /*!
     *  Unsubscribe to topic (acts via call of MessageToParent)
     *  \param [in] topic - relative (for this level of hierarchy) topic
     */
    void Unsubscribe(const TTopic &topic = ""); 
    
    /*!
     *  Actually shows error state (publish some error info)
     */
    virtual void SetError(const TTopic &errorMessage);
    
    /*!
     *  Reverses action of SetError 
     */
    virtual void CleanError(const TTopic &errorMessage);
    
    /*!
     *  Sets node and all her children recursively to error state.
     *  \param [in] errorMessage - message to publish to /meta/error
     *  empty string means call CleanError()
     */
    void SetErrorRecursive(const TTopic &errorMessage, bool isParentError = false);
    
    /*!
     *  Reverses action of SetErrorRecursive
     */
    void CleanErrorRecursive(bool isParentError = false);
    
    /*!
     * \param [in] name - name of member (name of Driver, Device, ...)
     * Notice: this constructor disabled if member has no parent.
     */
    THierarchyMemberBase(const std::string &name);
    /*!
     * \param [in] parent - pointer to parent in hierarchy
     * \param [in] name - name of member (name of Driver, Device, ...)
     */
    THierarchyMemberBase(TParent *parent, const std::string &name);
    
    /*! A destructor
     *  Unsubscribe here
     */
    virtual ~THierarchyMemberBase();
};


}
/*


struct TArgNoChildOrParent {
    typedef TArgNoChildOrParent TThis;
    typedef TArgNoChildOrParent TParent;
    typedef TArgNoChildOrParent TChild;
};


template <typename TThis_, typename TParent_>
struct TParentHolder {
    typedef TThis_ TThis;
    typedef TParent_ TParent;
    TParent *Parent;
    TParentHolder(TParent *newParent): Parent(newParent) {}
};

template<typename TThis_>
struct TParentHolder<TThis_, TArgNoChildOrParent>{
    typedef TThis_ TThis;
    TParentHolder() {}
};

template <typename TThis_, typename TChild_>
struct TChildHolder {
    typedef TThis_ TThis;
    typedef TChild_ TChild;
    TChild *Child;

    template <typename TChildSubclass = TChild_, typename ...TArgs>
    TChildSubclass *EmplaceChild(TArgs ...args);

    TChildHolder() {}
};


template<typename TThis_>
struct TChildHolder<TThis_, TArgNoChildOrParent>  {
    typedef TThis_ TThis;
    TChildHolder() {}
};

template <typename TParent_, typename TThis_, typename TChild_>
class THierarchyMemberBase : public TParentHolder<TThis_, TParent_>, public TChildHolder<TThis_, TChild_>
{
    void __StaticAsserts();
  public:

    typedef TThis_ TThis;
    typedef TParent_ TParent;
    typedef TChild_ TChild;

    static void *tt;


    THierarchyMemberBase();
    THierarchyMemberBase(TParent *newParent);
};

template <typename TParent_, typename TThis_, typename TChild_>
void THierarchyMemberBase<TParent_, TThis_, TChild_>::__StaticAsserts() {

    static_assert(
        std::is_same<TParent, TArgNoChildOrParent>::value ||
        std::is_base_of<TChildHolder<TParent, TThis>, TParent>::value,
        "Parent's child have a type different from this's type!!!"
    );
    static_assert(
        std::is_same<TChild, TArgNoChildOrParent>::value ||
        std::is_base_of<TParentHolder<TChild, TThis>, TChild>::value,
        "Child's parent have a type different from this's type!!!"
    );
};


template <typename TParent_, typename TThis_, typename TChild_>
THierarchyMemberBase<TParent_, TThis_, TChild_>::THierarchyMemberBase(TParent *newParent): TParentHolder<TThis_, TParent_>(newParent) {
    __StaticAsserts();
};

template <typename TParent_, typename TThis_, typename TChild_>
THierarchyMemberBase<TParent_, TThis_, TChild_>::THierarchyMemberBase() {
    __StaticAsserts();
};

template <typename TThis_, typename TChild_>
template <typename TChildSubclass, typename ...TArgs>
TChildSubclass *TChildHolder<TThis_, TChild_>::EmplaceChild(TArgs ...args) {
    Child = new TChildSubclass(static_cast<TThis*>(this), args...);
    return Child;
}



template <typename TThis_, typename TChild_>
struct TDriverBase : public THierarchyMemberBase<TArgNoChildOrParent, TThis_, TChild_> {
    TDriverBase(): THierarchyMemberBase<TArgNoChildOrParent, TThis_, TChild_>() {};
};


template <typename TParent_, typename TThis_, typename TChild_>
struct TDeviceBase : public THierarchyMemberBase<TParent_, TThis_, TChild_> {
    TDeviceBase(TParent_ *parent): THierarchyMemberBase<TParent_, TThis_, TChild_>(parent) {}
};


template <typename TParent_, typename TThis_>
struct TControlBase : public THierarchyMemberBase<TParent_, TThis_, TArgNoChildOrParent>  {

    TControlBase(TParent_ *parent): THierarchyMemberBase<TParent_, TThis_, TArgNoChildOrParent>(parent) {}
};



class TDriver;
class TDevice;
class TControl;

class TNode;



struct TDriver : public TDriverBase<TDriver, TDevice> {
    void printDown(int a);
    void printUp(int a);

    TDriver(): TDriverBase<TDriver, TDevice>() {}
};

struct TDevice : public TDeviceBase<TDriver, TDevice, TControl> {
    void printDown(int a);
    void printUp(int a);
    TDevice(TDriver *driver): TDeviceBase<TDriver, TDevice, TControl>(driver) {}
};

struct TControl : public TControlBase<TDevice, TControl> {
    void printDown(int a);
    void printUp(int a);

    TControl(TDevice *device): TControlBase<TDevice, TControl>(device) {}
};


struct TNode;

struct TNode : public THierarchyMemberBase<TNode, TNode, TNode> {
    string Info;
    virtual void printDown(int a) {
        if (Child == nullptr)
            cout << "From up to down: " << a << "(" << Info << ")" << endl;
        else
            Child->printDown(a);
    }
    virtual void printUp(int a) {
        if (Parent == nullptr)
            cout << "From down to up: " << a << "(" << Info << ")" << endl;
        else
            Parent->printUp(a);
    }

    TNode(TNode *parent, string info): THierarchyMemberBase<TNode, TNode, TNode>(parent), Info(info) {}
};



void TDriver::printDown(int a) { Child->printDown(a); }
void TDevice::printDown(int a) { Child->printDown(a); }
void TControl::printDown(int a) { cout << "print down " << a << endl;; }


void TDriver::printUp(int a) { cout << "print up " << a << endl;; }
void TDevice::printUp(int a) { Parent->printDown(a); }
void TControl::printUp(int a) { Parent->printDown(a); }


struct TControlGenericType;

struct TControlType {
    /// What to write to meta/units. "" if nothing.
    const string Units;
    const string Meta;

    string ToString() const {
        return "ControlType: meta=" + Meta + " units=" + Units;
    }
  private:
    friend class TControlGenericType;

    static int CurrentMaxOrder;

    TControlType(string meta, string units = "ones"): Meta(meta), Units(units) {}

  public:
    static const TControlType Temperature;
    static const TControlType RelativeHumidity;
    // ...
    static const TControlGenericType Generic;
};

const TControlType TControlType::Temperature = TControlType("temperature");

struct TControlGenericType {
    typedef std::unique_ptr<TControlType> TControlTypePtr;
    static std::unordered_map<std::string, TControlTypePtr> GenericTypes;
  public:
    const TControlType &operator()(string units) const {
        cerr << "D: " << GenericTypes.size() << endl;
        auto it = GenericTypes.find(units);
        if (it == GenericTypes.end()) {
            TControlType *controlType = new TControlType("generic", units);
            GenericTypes[units] = std::unique_ptr<TControlType>(controlType);
            return *controlType;
        }
        return *it->second.get();
    };
    TControlGenericType() {};
};

const TControlGenericType TControlType::Generic;

std::unordered_map<std::string, TControlGenericType::TControlTypePtr> TControlGenericType::GenericTypes;



template <class T1, class ...TArgs>
struct is_one_of : public std::false_type {};

template <class T1, class T2, class ...TArgs>
struct is_one_of<T1, T2, TArgs...> : std::conditional<std::is_same<T1, T2>::value, std::true_type, is_one_of<T1, TArgs...> >::type {};

template <class ...TArgs>
struct all_different : public std::true_type {};

template <class T1, class ...TArgs>
struct all_different<T1, TArgs...> : std::conditional<is_one_of<T1, TArgs...>::value, std::false_type, all_different<TArgs...> >::type {};

static void test_is_one_of() {
    static_assert(
        !is_one_of<int>::value &&
        !is_one_of<int, float>::value &&
        !is_one_of<bool, int, double>::value &&
        is_one_of<int, int>::value &&
        is_one_of<int, double, int>::value &&
        is_one_of<int, double, int, double>::value,
        "is_one_of failed"
    );
}

static void test_all_different() {
    static_assert(
        all_different<>::value &&
        all_different<int>::value &&
        all_different<int, float>::value &&
        all_different<bool, double, int>::value &&
        !all_different<int, int>::value &&
        !all_different<int, double, int>::value &&
        !all_different<int, int, float>::value &&
        !all_different<int, bool, int, char>::value,
        "all_different"
    );
}


template <typename FuncT, typename ...TArgs>
class call_with_arguments {
    call_with_arguments(FuncT func, TArgs ...args) {
        func(args...);
    };
};



template <typename T>
class TTypeWrapper {
    const T Value;
  public:
    operator const T &() {
        return Value;
    }
    TTypeWrapper(const T &value): Value(value) {};
};


int min(int a, int b) {
    return std::min(a, b);
}

template <int argumentOrder>
struct TArgumentOrderHolder {
    static const int ArgumentOrder = argumentOrder;
};

template <typename T>
struct TOrderOf {
};

template<>
struct TOrderOf<char> : public TArgumentOrderHolder<1> {};
template<>
struct TOrderOf<int> : public TArgumentOrderHolder<2> {};
template<>
struct TOrderOf<double> : public TArgumentOrderHolder<3> {};


template <typename T, typename ...TArgs>
struct TFirstOnPlace : public std::true_type {};

template <typename T1, typename T2, typename ...TArgs>
struct TFirstOnPlace<T1, T2, TArgs...> : public std::conditional<TOrderOf<T1>::ArgumentOrder <= TOrderOf<T2>::ArgumentOrder, TFirstOnPlace<T1, TArgs...>, std::false_type>::type {};

template <typename T>
void print_all(T arg) {
    cout << arg << " ";
}

void print_stupid() {
    cout << " << STUPID" << endl;
}

template <typename T, typename ...TArgs>
void print_stupid(T arg, TArgs ...args) {
    cout << typeid(arg).name() << arg << " ";
    print_stupid(args...);
}

template <typename T, typename ...TArgs>
void print_all(T arg, TArgs ...args) {
    //print_stupid(arg, args...);
    //cout << TFirstOnPlace<TArgs...>::value << endl;
    if (TFirstOnPlace<T, TArgs...>::value) {
        print_all(arg);
        print_all(args...);
    }
    else
        print_all(args..., arg);

}



int main(int argc, char ** argv)
{
    TNode *node1 = new TNode(nullptr, "1");
    TNode *node2 = node1->EmplaceChild("2");
    TNode *node3 = node1->EmplaceChild("3");

    node1->printDown(12);
    node3->printUp(13);

    TDriver *driver = new TDriver();
    TDevice *device = driver->EmplaceChild<TDevice>();
    TControl *control = device->EmplaceChild<TControl>();

    device->printDown(42);
    control->printUp(63);
    return 0;
}
*/
