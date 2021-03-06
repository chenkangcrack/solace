#ifndef DELTASOLACE_H_
#define DELTASOLACE_H_

#include "k.h"

#ifdef _WIN32
#define EXP __declspec(dllexport)
#else
#define EXP
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialises Solace middlware
 *
 * @param options Should be a dict type. Pair of symbol name/values can be passed that are valid solace session 
 * options i.e. https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/group___session_props.html
 * Important options are
 * SESSION_HOST, SESSION_VPN_NAME, SESSION_USERNAME, SESSION_PASSWORD
 * Example code
 * soloptions:`SESSION_HOST`SESSION_VPN_NAME`SESSION_USERNAME`SESSION_PASSWORD`SESSION_RECONNECT_RETRIES!(`hostname;`vpnname;`username;`password;`3);
 * solace.init_solace[soloptions];
 *
 */
EXP K init_solace(K options);

/**
 * Provides a dictionary of solace API version details. 
 */
EXP K version_solace(K unused);

/**
 * Should be called prior to init_solace. Provides notifications on session events
 * @param callbackFunction The kdb function to call for session events. The callback function will use 3 params - eventtype, responsecode, eventinfo.
 *  EventType is an int (https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/sol_client_8h.html#a6b74c1842f26be7bc10f6f4dd6995388 
 *  e.g. 0 is SOLCLIENT_SESSION_EVENT_UP_NOTICE ), 
 *  responsecode is an int (returned for some events, otherwise zero) and eventinfo is a string (further information about the event, when available)
 */
EXP K setsessioncallback_solace(K callbackFunction);

/**
 * Should be called prior to init_solace. Provides notifications on flow events (e.g. subscription made)
 * @param callbackFunction The kdb function to call for session events. The callback function will use 5 params - eventtype, responsecode, eventinfo, destType, destName
 * EventType is an int (https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/sol_client_8h.html#a992799734a2bbe3dd8506ef332e5991f )
 * e.g. 0 is SOLCLIENT_FLOW_EVENT_UP_NOTICE 
 * responsecode is an int (returned for some events, otherwise zero)
 * eventinfo is a string (further information about the event, when available)
 * destType is an int (https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/sol_client_8h.html#aa7d0b19a8c8e8fbec442272f3e05b485 )
 * e.g. 0 is SOLCLIENT_NULL_DESTINATION 
 * destName is a string (name of topic/queue)
 */
EXP K setflowcallback_solace(K callbackFunction);

/**
 * Finds whether boolean type capabilities for the solace connection are supported 
 * by the connected message router. This does not enable any feature or change how messages are sent.
 * This can be useful for debugging
 *
 * @param capabilityName Can be a symbol of character array. Any capability name on http://docs.solace.com/API-Developer-Online-Ref-Documentation/c/index.html 
 * that returns a boolean e.g. "SESSION_CAPABILITY_PUB_GUARANTEED"
 */
EXP K iscapable_solace(K capabilityName);

/**
 * Provides the value of a capability from the Solace Router. The returned type may be symbol, int, bool, etc, depending on the type of
 * capability requested e.g. requesting SESSION_PEER_SOFTWARE_VERSION will return a symbol
 * @param capabilityName Can be a symbol of character array. Any capability name on http://docs.solace.com/API-Developer-Online-Ref-Documentation/c/index.html
 */
EXP K getcapability_solace(K capabilityName);

/**
 *  Creates a durable endpoint on the solace router. Returns 0 if queue created. Otherwise returns solace error code. Typical examples are
 *  75 = SOLCLIENT_SUBCODE_ENDPOINT_ALREADY_EXISTS
 *  76 = SOLCLIENT_SUBCODE_PERMISSION_NOT_ALLOWED
 *  79 = SOLCLIENT_SUBCODE_ENDPOINT_PROPERTY_MISMATCH
 *  @param options Dictionary of properties (symbol types) for solClient_session_endpointProvision
 *  @param provFlags Integer defining provision flags for the endpoint as defined in solClient_session_endpointProvision
 */
EXP K createendpoint_solace(K options,K provFlags);

/**
 * The opposite of createendpoint_solace. This destroys a durable endpint. Care must be taken, if items are still on the endpoint to be processed.
 * An endpoint may have been created by a 3rd party, so you may have to leave it untouched. Returns 0 if an endpoint created. Otherwise returns solace error code. 
 * Typical examples are
 * 61 = SOLCLIENT_SUBCODE_UNKNOWN_QUEUE_NAME
 * 62 = SOLCLIENT_SUBCODE_UNKNOWN_TE_NAME
 * 76 = SOLCLIENT_SUBCODE_PERMISSION_NOT_ALLOWED
 * @param options Dictionary of properties (symbol types) for solClient_session_endpointDeprovision
 * @param provFlags Integer defining provision flags for the endpoint as defined in solClient_session_endpointDeprovision
 */
EXP K destroyendpoint_solace(K options, K provFlags);

/**
 * Add a Topic subscription to an existing endpoint. Can be added to queues or remote clients. 
 * Ref: https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/sol_client_8h.html#a75159f729405cb331e64466ac9cd7f41
 * @param options Dictionary of properties (symbol types) for solClient_session_endpointTopicSubscribe
 * @param provFlags Integer defining provision flags for the endpoint as defined in solClient_session_endpointTopicSubscribe
 * @param topic Symbol or string that defines the topic in which to add a subscription
 */
EXP K endpointtopicsubscribe_solace(K options, K provFlags, K topic);

/**
 * The opposite of endpointtopicsubscribe_solace. Refer to endpointtopicsubscribe_solace for params
 */
EXP K endpointtopicunsubscribe_solace(K options, K provFlags, K topic);

/**
 * Send data over Solace, using direct messages
 *
 * @param topic Should be a string. The Topic to send data to.
 * @param data Can be a symbol or string or byte array. The payload of the message.
 */
EXP K senddirect_solace(K topic, K data);

/**
 * Sends a topic request message. This expects an end-to-end reply from the client that receives the message
 * 
 * @param topic Should be a string. The Topic to send data to.
 * @param data Can be a symbol or string or byte array. The payload of the message.
 * @param timeout Integer type representing milliseconds to wait/block, must be greater than 0
 * @param replyType Should be an int. -1 for null, 0 for topic, 1 for queue, 2 for temp topic, 3 for tmp queue. The topic/queue that you wish a reply to this message to go to
 * @param replyDest Should be a symbol. The topic/queue that you wish a reply to this message to go to (empty for default session topic)
 * @return Returns a byte list of message received, containing the payload. Otherwise will be an int
 * to indicate the return code. If value 7, the reply wasnt received. 
 */
 EXP K senddirectrequest_solace(K topic, K data, K timeout, K replyType, K replydest);

/**
 * Sets the KDB+ function that should be called on receipt of each direct msg created via
 * subscribetopic_solace. The function should accept 3 params, symbol destination, byte list for
 * payload and a dict of msg values. If the dict contains a value of true for the key 'isRequest',
 * the sender is request a reply. In order to reply, return either a symbol/string or byte list
 * for the reply msg contents.
 * @param cb Can be a symbol or string, which is the name of the q function to call on receipt of
 * each direct message
 */
EXP K callbacktopic_solace(K cb);

/**
 * Subscribe to a topic 
 *
 * @param topic Should be a string. The Topic to subscribe to
 * @param bool True to block until confirm or true to get session event callback on sub activation
 */
EXP K subscribetopic_solace(K topic, K isBlocking);

/**
 * Unsubscribe from a topic subscription 
 *
 * @param topic Should be a string. The Topic to unsubscribe from
 */
EXP K unsubscribetopic_solace(K topic);

/**
 * Send data over Solace, using persistent/guaranteed messages 
 *
 * @param destType Should be an int. -1 for null, 0 for topic, 1 for queue, 2 for temp topic, 3 for tmp queue. The topic/queue that you want this message to go to
 * @param dest Should be a symbol. The destination send data to (e.g. endpoint name of queue, topic name)
 * @param data Can be a symbol of character array. The payload of the message.
 * @param correlationId Can be a symbol of character array. The Solace Correlation ID
 */
EXP K sendpersistent_solace(K destType, K dest, K data, K correlationId);

/**
 * Sends a guaranteed request message. This expects an end-to-end reply from the client that receives the message
 * 
 * @param destType Should be an int. -1 for null, 0 for topic, 1 for queue, 2 for temp topic, 3 for tmp queue. The topic/queue that you want this message to go to
 * @param dest Should be a symbol. The destination send data to (e.g. endpoint name of queue, topic name)
 * @param data Can be a symbol of character array. The payload of the message.
 * @param timeout Integer type representing milliseconds to wait/block, must be greater than 0
 * @param replyType Should be an int. -1 for null, 0 for topic, 1 for queue, 2 for temp topic, 3 for tmp queue. The topic/queue that you wish a reply to this message to go to
 * @param replyDest Should be a symbol. The topic/queue that you wish a reply to this message to go to (empty for default session topic)
 * @return Returns a byte list of message received, containing the payload. Otherwise will be an int
 * to indicate the return code. If value 7, the reply wasnt received. 
 * 
 */
EXP K sendpersistentrequest_solace(K destType, K dest, K data, K timeout, K replyType, K replydest);


/**
 * Sets the KDB+ function that should be called on receipt of each direct msg created via
 * bindqueue_solace. 
 * @param cb Can be a symbol or string, which is the name of the q function to call on receipt of
 * each message. The callback function will use 3 params, queue destination name, byte array payload and a dict - 
 * Dict consists of reply destination type, reply destination name, correlationid, msgid. 
 * Destination Type is an int (-1 for null,0 for topic,1 for queue,2 for tmp topic,3 for tmp queue), destination name is a string (subscription name), 
 * Reply Destination Type is an int (-1 for null,0 for topic,1 for queue,2 for tmp topic,3 for tmp queue), reply destination name is a string,
 * correlationid is a string. msgid is a long
 */
EXP K callbackqueue_solace(K cb);

/**
 * Subscribes/Binds to an endpoint (e.g. queue). The callback function provided will be called by the API whenever a message is received on the subscription
 *
 * @param bindProps TODO reference for properties. A dictionary of flow configuration properties with values.
 */
EXP K bindqueue_solace(K bindProps);

/**
 * Sends an acknowledgment on the specified Flow.
 * @param endpointname The endpoint that the msg was originally received on. Type symbol or string.
 * @param msgid The msg id of the mesage to be acknowledged. Type long.
 * @return A integer with value 0 if ok, -1 for failure
 */
EXP K sendack_solace(K endpointname, K msgid);

/**
 * Unsubscribes from a queue 
 *
 * @param endpointname The endpoint name
 */
EXP K unbindqueue_solace(K endpointname);

/**
 * Should be called after init_solace, when nothing else needs to be sent
 */
EXP K destroy_solace(K a);

#ifdef __cplusplus
}
#endif

#endif /* DELTASOLACE_H_ */
