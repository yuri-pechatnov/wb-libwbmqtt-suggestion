/*!
 *  This file represents very simple (raw) model 
 *  
 *  Also it is an example of creating hierarchies
 *  Moreover this model will be along with libwbmqtt
 */
 
 

//include headers with declarations

// Firstly declare all classes of hierarchy
class TMqttNode;

/*!
 *  Class of driver in simple model
 *  How to read name: T - type, Driver - driver, Sm - SM - simple model
 */
class TMqttNode : public NWbMqtt::THierarchyMemberBase<TMqttNode, TMqttNode, TMqttNode, TArgHavingValue> {
  protected:
    virtual TMqttNode *CreateChildByName(const String &nameOfChild) override;
  public:
    void AddHandler(THandler handler);
    
    virtual void OnValueChange(const TTopic &topic, const std::string &newValue) override {
        // call handlers
    }
    
    /*! 
     * Constructor.
     */
    TMqttNode(TMqttNode *parent, const std::string &name);
    /*!
     *  Destructor
     */
    virtual ~TMqttNode();
};

class TMqttMainNode : public TMqttNode, private TMqttWrapper {
    virtual void OnConnect(int rc) override;
    virtual void OnMessage(const struct mosquitto_message *message) override; 
    virtual void OnSubscribe(int mid, int qos_count, const int *granted_qos) override;
  public:
    virtual void ProcessMessageFromChild(const TMessage &message) override;
    TMqttMainNode(): TMqttNode(this, "") {}
}
 

void usage() {
    TMqttMainNode mainNode(...);
    
    mainNode["devices"]["buzzer"]["switch"] = "1";
    mainNode["devices"]["buzzer"]["switch"]["meta"]["type"] = "switch";
    
    mainNode["devices"]["motion_sensor"]["state"].AddHandler([](TMqttNode *node) { 
        if (node->GetValue<bool>())
            cout << "There is a motion!" << endl; 
        else
            cout << "No motion!" << endl; 
    });    
    
    mainNode["devices"]["buzzer"]["switch"].Delete(); // delete from mqtt
    
}



// usage end

