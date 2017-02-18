/*!
 * TDevice is declared here
 * 
 * Simplified message system:
 *      topic, relativeTopic - strings like "/device_name/control_name/state"
 *          always from "/"
 *      message - non-empty string to set value, empty to publish null (erase topic from mqtt)
 */

 
namespace NWbMqtt {
 
template <typename TDriver, typename TChild_>
class TDriverBase : public THierarchyMemberBase<TNoChildOrParent, TDriver, TChild_>, private TMqttWrapper {
 
    virtual void OnConnect(int rc) override;
    /* struct mosquitto_message{
                int mid;
                char *topic;
                void *payload;
                int payloadlen;
                int qos;
                bool retain;
            };
        Используются ли где-то mid, qos, retain?
     * note: struct "message" will be freed by mosquitto library
    */
    
    /*!
     * The method that receives messages from mqtt
     * All received messages are written to messageContainer
     * \param [in] message - message (topic and exactly message), that will be written to message container
     * mid, qos, retain are ignored
     * 
     */
    virtual void OnMessage(const struct mosquitto_message *message) override; ///< Delegates message to ProcessMessageFromParent 
    
    virtual void OnSubscribe(int mid, int qos_count, const int *granted_qos) override;
    
  public:
    /*! 
     * Supposed to be a Device type or DeviceGroup type
     */
    typedef TDriver_ TDriver;
    typedef TChild_ TChild; 
  protected:
    
    /// thread-safe container for TMessage
    TAsyncMessagesContainer MessageContainer;
    
    
    
    /*!
     * By default cut "/devices" from topic.
     */
    virtual void ProcessMessageFromParent(const TMessage &message) override;
    virtual void ProcessMessageFromChild(const TMessage &message) override;
    
  public:
  
    /*!
     * This method processes all saved messages,
     *  giving them to TMessagingNodeBase::ProcessMessageFromParent <br>
     * Then clears MessageContainer <br>
     * Useful to run in endless loop
     */
    virtual void ProcessSavedMessages();
    
    /*!
     *  Sleep and wait for message from mqtt. <br>
     *  After return from this function MessageContainer isn't empty.
     *  \return Count of messages stored in MessageContainer, negative value if error happened
     */
    virtual size_t WaitMessage(int maxTimeUsec);
    
    
    /*!
     *  Sleeps and waits for message from mqtt.
     *  Then processes it.
     */
    virtual void WaitAndProcessOneMessage(int maxTimeUsec);
    
    /*!
     *  Sleeps and waits for messages from mqtt and processes them immediately.
     */
    virtual void WaitAndProcessMessages(int timeUsec);
    
    
    /*!
     * Driver constructor
     * \param [in] deviceName - name of driver
     * \param [in] deviceDescription - description of driver
     */
    TDriverBase(const std::string &driverName);
    virtual ~TDriverBase();
}

}
