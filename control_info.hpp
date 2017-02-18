/*!
 * Classes storing control info declared here
 */
 
 
namespace NWbMqtt {


/*!
 * Info about control types
 * Firstly it's meta/type, meta/units
 * Secondly a set of default settings.
 * 
 * Usage: give to TControlBase constructor argument like
 * TControlType::Temperature or TControlType::Generic("cm")
 */
class TControlType {
  public:
    /// What to write to meta/type. "" if nothing.
    const std::string MetaType;
    /// What to write to meta/units. "" if nothing.
    const std::string Units;
    /// Default name for control with this type
    const std::string DefaultName;
    /// Default description for control with this type
    const std::string DefaultDescription;
    /// Default order for control with this type
    const int DefaultOrder;
  private:
    friend class TControlGenericType;
    
    static std::set< std::unique_ptr <TControlType*> > ControlTypes; 
    static int CurrentMaxOrder;
    
    TControlType(const std::string &metaType, const std::string &DefaultName, 
                 const std::string &DefaultDescription, const std::string &units = "");
                 
  public:
    static const TControlType *Temperature;
    static const TControlType *RelativeHumidity;
    // ...

    static const TControlGenericType Generic; ///< Use as Generic("cm")
};

/*!
 *  \brief Type of TControlType::Generic
 *  There is a meta-type that requires meta/units.
 *  It's impossible to create template variable (WB g++ is old enough)
 *  So TControlType::Generic have a special type. 
 *  By call TControlType::Generic("cm") you can get a Control Type with specified meta/units.
 */
struct TControlGenericType {
  private:
    static std::unordered_map<std::string, *TControlType> GenericTypes;
  public:
    const TControlType *operator()(string units) const {};
};



template <typename T>
class TTypeWrapper {
    const T Value;
  public:
    operator const T &() {
        return Value;
    }
    explicit TTypeWrapper(const T &value): Value(value) {};
};

/// Wrapper over bool to pass readonly flag to TControlBase
struct TArgReadonly : TTypeWrapper<bool> {
    explicit TArgReadonly(bool value);
};

/// Wrapper over bool to pass isAcceptingMessages flag to TControlBase
struct TArgAcceptingMessages : TTypeWrapper<bool> {
    explicit TArgAcceptingMessages(bool isAcceptingMessages);
};

/// Wrapper over bool to pass isCreatingInMqtt flag to TControlBase
struct TArgCreatingInMqtt : TTypeWrapper<bool> {
    explicit TArgCreatingInMqtt(bool isCreatingInMqtt);
};

/// Wrapper over bool to pass Description flag to TControlBase
struct TArgDescription : std::string {
    explicit TArgDescription(const std::string &description);
};

/// Wrapper over int to pass Order to TControlBase
struct TArgOrder : TTypeWrapper<int> {
    explicit TArgOrder(const int order);
};

/// Wrapper over bool to pass isSuscribingImmediately flag to TControlBase
struct TArgSubscribeImmediately : TTypeWrapper<bool> {
    explicit TArgSubscribeImmediately(bool isSuscribingImmediately);
};


/// Struct to pass max value to control
struct TArgMaxValue {
    unsigned int32_t HasMax : 1, Max : 31;
    explicit TArgMaxValue(int maxValue);
};


/*!
 * C++ std-style checker that first template parameter is equal to one of last template perameters.
 */
template <class T1, class ...TArgs>
struct is_one_of : public std::false_type {};

template <class T1, class T2, class ...TArgs>
struct is_one_of<T1, T2, TArgs...> : std::conditional<std::is_same<T1, T2>::value, std::true_type, is_one_of<T1, TArgs...> >::type {};

/*!
 * C++ std-style checker that all template paramters are different
 */
template <class ...TArgs>
struct all_different : public std::true_type {};

template <class T1, class ...TArgs>
struct all_different<T1, TArgs...> : std::conditional<is_one_of<T1, TArgs...>::value, std::false_type, all_different<TArgs...> >::type {};

/// test for is_one_of
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

/// test for all_different
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

/* example of sorting arguments

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
*/

};
