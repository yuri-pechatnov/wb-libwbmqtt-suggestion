/*!
 *  This file represents simple model where logic is inside of hierarchy classes
 *  I. e. Driver -> Device -> Control
 *  Also it is an example of creating hierarchies
 * 
 */
 
 

//include headers with declarations

// Firstly declare all classes of hierarchy
class TDriverSm;
class TDeviceSm;
class TControlSm;

/*!
 *  Class of driver in simple model
 *  How to read name: T - type, Driver - driver, Sm - SM - simple model
 */
class TDriverSm : public NWbMqtt::TDriverBase<TDriverSm, TDeviceSm> {
  protected:
    virtual TDeviceSm *CreateChildByName(const String &nameOfChild) override;
  public:
    
    /*! 
     * Constructor.
     */
    TDriverSm(const std::string &driverName);
    /*!
     *  Destructor
     */
    virtual ~TDriverSm();
};

/*!
 *  Class of devices in simple model
 *  How to read name: T - type, Device - device, Sm - SM - simple model
 */
class TDeviceSm : public NWbMqtt::TDeviceBase<TDriverSm, TDeviceSm, TControlSm> {
  protected:
    virtual TControlSm *CreateChildByName(const String &nameOfChild) override;
  public:
    /*! 
     * Constructor.
     */
    TDeviceSm(TDriver *driver, const std::string &deviceName);
    virtual ~TDeviceSm();
};


/*!
 *  Class of controls in simple model
 *  How to read name: T - type, Control - control, Sm - SM - simple model
 */
class TControlSm : public NWbMqtt::TControlBase<TDeviceSm, TControlSm> {
  public:    
    /*! 
     * Constructor.
     */
    template <typename ...Args>
    TControlSm(TDevice *device, Args... args): TControlBase<TDeviceSm>(device, args...) {}
    virtual ~TControlSm();
};

// usage begin

class TMyControlSm : public TControlSm {
  public:
    bool IsChanged;
    
    virtual void OnValueChange(const TTopic &topic, const String &newValue) override {
        SetValue(newValue);
        cout << "Received message" << endl;
        IsChanged = true;
        return MESSAGE_ACCEPTED;
    }
    
    template <typename ...Args>
    TMyControlSm(TDevice* device, Args... args): TControlSm(device, args..., TArgReadonly(false)) {}
    virtual ~TMyControlSm();
}
 

void usage_1() {
    TDriverSm *driver = new TDriverSm("rfsniffer");
    
    // EmplaceChild calls TDeviceSm constructor with arguments: pointer to driver, "noolite_rx_1234"
    TDeviceSm *device = driver->EmplaceChild<TDeviceSm>("noolite_rx_1234");
    
    device->EmplaceChild<TControlSm>("temperature", TControlType::Temperature, TArgReadonly(true));
    (*driver)["noolite_rx_1234"]["temperature"] = "17.0";
    (*driver)["noolite_rx_1234"]["temperature"].As<double>() = 17.0;
    (*driver)["noolite_rx_1234"]["temperature"].As<double>() += 0.1;
    (*driver)["noolite_rx_1234"].EmplaceChild<TControlSm>(
        // notice, default name will be taken
        TControlType::RelativeHumidity, 
        TArgOrder(2)
    );
    
    // how to process income messages (from mqtt) in control?
    // override methods ProcessMessageFromParent*
    // add object
    (*driver)["noolite_rx_1234"].EmplaceChild<TMyControlSm>(
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
    TControlSm &control = *(*driver)["mega_switch"].EmplaceChild<TControlSm>("state",                       TArgCreatingInMqtt(false),         TArgSubscribeImmediately(true));
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
