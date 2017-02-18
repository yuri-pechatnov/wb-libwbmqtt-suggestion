/*!
 *  This file represents a hierarchy with enabled wildcards
 *  I. e. Driver -> Device -> Control
 *  And usage:
 *  driver.AnyChild()["temperature"].Subscribe();
 *  driver.AnyDescendant().Subscribe();
 *  driver["oregon_rx_1234"].AnyChild().Subscribe();
 *  driver.AnyChild()["meta"].AnyChild().Subscribe();
 * 
 *  driver['+']["meta"]["driver"].Subscribe();
 *  driver['#'].Subscribe();
 *  driver["oregon_rx_1234"]['+'].Subscribe();
 *  driver.AnyChild()["meta"].AnyChild().Subscribe();
 * 
 */


//include headers with declarations

// Firstly declare all classes of hierarchy
class TDriverWc;
class TDeviceWc;
class TControlWc;

/*!
 *  Class of driver in simple model
 *  How to read name: T - type, Driver - driver, Wc - SM - simple model
 */
class TDriverWc : public NWbMqtt::TDriverBase<TDriverWc, TDeviceWc> {
  protected:
    virtual TDeviceWc *CreateChildByName(const String &nameOfChild);
  public:
    
    /*! 
     * Constructor.
     */
    TDriverWc(const std::string &driverName);
    /*!
     *  Destructor
     */
    virtual ~TDriverWc();
};

/*!
 *  Class of devices in simple model
 *  How to read name: T - type, Device - device, Wc - SM - simple model
 */
class TDeviceWc : public NWbMqtt::TDeviceBase<TDriverWc, TDeviceWc, TControlWc> {
  protected:
    virtual TControlWc *CreateChildByName(const String &nameOfChild) override;
  public:
    /*! 
     * Constructor.
     */
    TDeviceWc(TDriver *driver, const std::string &deviceName);
    virtual ~TDeviceWc();
};


/*!
 *  Class of controls in simple model
 *  How to read name: T - type, Control - control, Wc - SM - simple model
 */
class TControlWc : public NWbMqtt::TControlBase<TDeviceWc, TControlWc> {
  public:    
    /*! 
     * Constructor.
     */
    template <typename ...Args>
    TControlWc(TDevice *device, Args... args): TControlBase<TDeviceWc>(device, args...) {}
    virtual ~TControlWc();
};

// usage begin

class TMyControlWc : public TControlWc {
  public:
    bool IsChanged;
    
    virtual void OnValueChange(const TTopic &topic, const String &newValue) override {
        SetValue(newValue);
        cout << "Received message" << endl;
        IsChanged = true;
        return MESSAGE_ACCEPTED;
    }
    
    template <typename ...Args>
    TMyControlWc(TDevice* device, Args... args): TControlWc(device, args..., TArgReadonly(false)) {}
    virtual ~TMyControlWc();
}
 

void usage_1() {
    TDriverWc *driver = new TDriverWc("rfsniffer");
    
    // EmplaceChild calls TDeviceWc constructor with arguments: pointer to driver, "noolite_rx_1234"
    TDeviceWc *device = driver->EmplaceChild<TDeviceWc>("noolite_rx_1234");
    
    device->EmplaceChild<TControlWc>("temperature", TControlType::Temperature, TArgReadonly(true));
    (*driver)["noolite_rx_1234"]["temperature"] = "17.0";
    (*driver)["noolite_rx_1234"]["temperature"].As<double>() = 17.0;
    (*driver)["noolite_rx_1234"]["temperature"].As<double>() += 0.1;
    (*driver)["noolite_rx_1234"].EmplaceChild<TControlWc>(
        // notice, default name will be taken
        TControlType::RelativeHumidity, 
        TArgOrder(2)
    );
    
    // how to process income messages (from mqtt) in control?
    // override methods ProcessMessageFromParent*
    // add object
    (*driver)["noolite_rx_1234"].EmplaceChild<TMyControlWc>(
            "do_measure_temperature", 
            TControlType::Switch,
            TArgReadonly(false),
            TArgSubscribeImmediately(true)
    ); 
    // and run processing saved messages
    driver->WaitAndProcessMessages(1000);
    
    
    // Create control that already exists in mqtt
    // So we write nothing there. Just subscribe.
    
    //                              V here CreateChildByName of "mega_switch" (we override this method)     V not publish anything to mqtt     V subscribe to topic immediately
    TControlWc &control = *(*driver)["mega_switch"].EmplaceChild<TControlWc>("state",                       TArgCreatingInMqtt(false),         TArgSubscribeImmediately(true));
    //                                              ^ here explicit creation of "state" because we should give flag "TArgCreatingInMqtt(false)"
   
    // control.Subscribe(); commented because we use "TArgSubsribeImmediately(true)" flag
   
    while (!control.IsChanged)
        driver->WaitAndProcessOneMessage(1000);
    cout << "Control state is: " << (*driver)["mega_switch"]["state"].GetValue() << endl;
    cout << "The same output: " << (*driver).GetChildReference("mega_switch").GetChildReference("state").GetValue() << endl;
    cout << "The same output: " << driver->GetChild("mega_switch")->GetChild("state")->GetValue() << endl;
    cout << "The same output: " << control.GetValue() << endl;
}


// usage end
