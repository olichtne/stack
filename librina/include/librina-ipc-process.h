/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef LIBRINA_IPC_PROCESS_H
#define LIBRINA_IPC_PROCESS_H

#ifdef __cplusplus

#include "librina-common.h"
#include "librina-application.h"

#define RINA_DEFAULT_POLICY_NAME "default"
#define RINA_DEFAULT_POLICY_VERSION 0

namespace rina {

/**
 * The IPC Manager requests the IPC Process to become a member of a
 * DIF, and provides de related information
 */
class AssignToDIFRequestEvent: public IPCEvent {

	/** The information of the DIF the IPC Process is being assigned to*/
	DIFInformation difInformation;

public:
	AssignToDIFRequestEvent(const DIFInformation& difInformation,
			unsigned int sequenceNumber);
	const DIFInformation& getDIFInformation() const;
};

/**
 * The IPC Manager requests the IPC Process to update the configuration
 * of the DIF he is currently a member of
 */
class UpdateDIFConfigurationRequestEvent: public IPCEvent {

        /** The new configuration of the DIF*/
        DIFConfiguration difConfiguration;

public:
        UpdateDIFConfigurationRequestEvent(
                        const DIFConfiguration& difConfiguration,
                        unsigned int sequenceNumber);
        const DIFConfiguration& getDIFConfiguration() const;
};

/**
 * The IPC Manager requests the IPC Process to enroll to a DIF,
 * through neighbour neighbourName, which can be reached by allocating
 * a flow through the supportingDIFName
 */
class EnrollToDIFRequestEvent: public IPCEvent {

        /** The DIF to enroll to */
        ApplicationProcessNamingInformation difName;

        /** The N-1 DIF name to allocate a flow to the member */
        ApplicationProcessNamingInformation supportingDIFName;

        /** The neighbor to contact */
        ApplicationProcessNamingInformation neighborName;

public:
        EnrollToDIFRequestEvent(
                const ApplicationProcessNamingInformation& difName,
                const ApplicationProcessNamingInformation& supportingDIFName,
                const ApplicationProcessNamingInformation& neighbourName,
                unsigned int sequenceNumber);
        const ApplicationProcessNamingInformation& getDifName() const;
        const ApplicationProcessNamingInformation& getNeighborName() const;
        const ApplicationProcessNamingInformation&
                getSupportingDifName() const;
};

/**
 * Supporting class for IPC Process DIF Registration events
 */
class IPCProcessDIFRegistrationEvent: public IPCEvent {

	/** The name of the IPC Process registered to the N-1 DIF */
	ApplicationProcessNamingInformation ipcProcessName;

	/** The name of the N-1 DIF where the IPC Process has been registered*/
	ApplicationProcessNamingInformation difName;

	/** True if the IPC Process has been registered in a DIF, false otherwise */
	bool registered;

public:
	IPCProcessDIFRegistrationEvent(
	                const ApplicationProcessNamingInformation& ipcProcessName,
			const ApplicationProcessNamingInformation& difName,
			bool registered,
			unsigned int sequenceNumber);
	const ApplicationProcessNamingInformation& getIPCProcessName() const;
	const ApplicationProcessNamingInformation& getDIFName() const;
	bool isRegistered() const;
};

/**
 * The IPC Manager queries the RIB of the IPC Process
 */
class QueryRIBRequestEvent: public IPCEvent {

	/** The class of the object being queried*/
	std::string objectClass;

	/** The name of the object being queried */
	std::string objectName;

	/**
	 * The instance of the object being queried. Either objectname +
	 * object class or object instance have to be specified
	 */
	long objectInstance;

	/** Number of levels below the object_name the query affects*/
	int scope;

	/**
	 * Regular expression applied to all nodes affected by the query
	 * in order to decide whether they have to be returned or not
	 */
	std::string filter;

public:
	QueryRIBRequestEvent(const std::string& objectClass,
			const std::string& objectName, long objectInstance, int scope,
			const std::string& filter, unsigned int sequenceNumber);
	const std::string& getObjectClass() const;
	const std::string& getObjectName() const;
	long getObjectInstance() const;
	int getScope() const;
	const std::string& getFilter() const;
};

/**
 * The Kernel components of the IPC Process report about the result of a
 * create EFCP connection operation
 */
class CreateConnectionResponseEvent: public IPCEvent {

        /** The port-id where the connection will be bound to */
        int portId;

        /**
         * The source connection-endpoint id if the connection was created
         * successfully, or a negative number indicating an error code in
         * case of failure
         */
        int cepId;

public:
        CreateConnectionResponseEvent(int portId, int cepId,
                        unsigned int sequenceNumber);
        int getCepId() const;
        int getPortId() const;
};

/**
 * The Kernel components of the IPC Process report about the result of a
 * create EFCP connection operation
 */
class UpdateConnectionResponseEvent: public IPCEvent {

        /** The port-id where the connection will be bound to */
        int portId;

        /**
         * The result of the operation (0 successful)
         */
        int result;

public:
        UpdateConnectionResponseEvent(int portId, int result,
                        unsigned int sequenceNumber);
        int getResult() const;
        int getPortId() const;
};

/**
 * The Kernel components of the IPC Process report about the result of a
 * create EFCP connection arrived operation
 */
class CreateConnectionResultEvent: public IPCEvent {

        /** The port-id where the connection will be bound to */
        int portId;

        /**
         * The source connection-endpoint id if the connection was created
         * successfully, or a negative number indicating an error code in
         * case of failure
         */
        int sourceCepId;

        /** The destination cep-id of the connection */
        int destCepId;

public:
        CreateConnectionResultEvent(int portId, int sourceCepId,
                        int destCepId, unsigned int sequenceNumber);
        int getSourceCepId() const;
        int getDestCepId() const;
        int getPortId() const;
};

class DestroyConnectionResultEvent: public IPCEvent {

        /** The port-id where the connection will be bound to */
        int portId;

        /** The destination cep-id of the connection */
        int result;

public:
        DestroyConnectionResultEvent(int portId, int result,
                        unsigned int sequenceNumber);
        int getResult() const;
        int getPortId() const;
};

/**
 * Thrown when there are problems notifying the IPC Manager about the
 * result of an Assign to DIF operation
 */
class AssignToDIFResponseException: public IPCException {
public:
	AssignToDIFResponseException():
		IPCException("Problems informing the IPC Manager about the result of an assign to DIF operation"){
	}
	AssignToDIFResponseException(const std::string& description):
		IPCException(description){
	}
};

/**
 * Thrown when there are problems notifying the IPC Manager about the
 * result of a register application operation
 */
class RegisterApplicationResponseException: public IPCException {
public:
	RegisterApplicationResponseException():
		IPCException("Problems informing the IPC Manager about the result of a register application response operation"){
	}
	RegisterApplicationResponseException(const std::string& description):
		IPCException(description){
	}
};

/**
 * Thrown when there are problems notifying the IPC Manager about the
 * result of an unregister application operation
 */
class UnregisterApplicationResponseException: public IPCException {
public:
	UnregisterApplicationResponseException():
		IPCException("Problems informing the IPC Manager about the result of an unegister application response operation"){
	}
	UnregisterApplicationResponseException(const std::string& description):
		IPCException(description){
	}
};

/**
 * Thrown when there are problems notifying the IPC Manager about the
 * result of an allocate flow operation
 */
class AllocateFlowResponseException: public IPCException {
public:
	AllocateFlowResponseException():
		IPCException("Problems informing the IPC Manager about the result of an unegister application response operation"){
	}
	AllocateFlowResponseException(const std::string& description):
		IPCException(description){
	}
};

/**
 * Thrown when there are problems notifying the IPC Manager about the
 * result of a query RIB operation
 */
class QueryRIBResponseException: public IPCException {
public:
	QueryRIBResponseException():
		IPCException("Problems informing the IPC Manager about the result of a query RIB response operation"){
	}
	QueryRIBResponseException(const std::string& description):
		IPCException(description){
	}
};

/**
 * Thrown when there are problems allocating a remote flow to a
 * local application
 */
class AllocateFlowRequestArrivedException: public IPCException {
public:
	AllocateFlowRequestArrivedException():
		IPCException("Problems allocating a remote flow to a local application"){
	}
	AllocateFlowRequestArrivedException(const std::string& description):
		IPCException(description){
	}
};


/**
 * Thrown when there are problems notifying the application about the
 * result of a deallocate operation
 */
class DeallocateFlowResponseException: public IPCException {
public:
	DeallocateFlowResponseException():
		IPCException("Problems informing the application about the result of a deallocate operation"){
	}
	DeallocateFlowResponseException(const std::string& description):
		IPCException(description){
	}
};

/**
 * Thrown when there are problems requesting the Kernel to create an EFCP connection
 */
class CreateConnectionException: public IPCException {
public:
        CreateConnectionException():
                IPCException("Problems creating an EFCP connection"){
        }
        CreateConnectionException(const std::string& description):
                IPCException(description){
        }
};

/**
 * Thrown when there are problems requesting the Kernel to update an EFCP connection
 */
class UpdateConnectionException: public IPCException {
public:
        UpdateConnectionException():
                IPCException("Problems updating an EFCP connection"){
        }
        UpdateConnectionException(const std::string& description):
                IPCException(description){
        }
};

/**
 * Thrown when there are problems requesting the Kernel to destroy an EFCP connection
 */
class DestroyConnectionException: public IPCException {
public:
        DestroyConnectionException():
                IPCException("Problems destroying an EFCP connection"){
        }
        DestroyConnectionException(const std::string& description):
                IPCException(description){
        }
};

/**
 * Thrown when there are problems requesting the Kernel to allocate or deallocate a
 * port-id
 */
class PortAllocationException: public IPCException {
public:
        PortAllocationException():
                IPCException("Problems requesting the allocation/deallocation of a port-id"){
        }
        PortAllocationException(const std::string& description):
                IPCException(description){
        }
};

/**
 * Thrown when there are problems requesting the Kernel to modify
 * the PDU Forwarding table
 * port-id
 */
class PDUForwardingTableException: public IPCException {
public:
        PDUForwardingTableException():
                IPCException("Problems requesting modification of PDU Forwarding Table"){
        }
        PDUForwardingTableException(const std::string& description):
                IPCException(description){
        }
};


/**
 * Class used by the IPC Processes to interact with the IPC Manager. Extends
 * the basic IPC Manager in librina-application with IPC Process specific
 * functionality
 */
class ExtendedIPCManager: public IPCManager {
	/** The ID of the IPC Process */
	unsigned short ipcProcessId;

	/** The portId of the IPC Manager */
	unsigned int ipcManagerPort;

	/**
	 * True if the IPC Process has been initialized,
	 * false otherwise
	 */
	bool ipcProcessInitialized;

	/** The current configuration of the IPC Process */
	DIFInformation currentDIFInformation;

public:
	static const std::string error_allocate_flow;
	ExtendedIPCManager();
	~ExtendedIPCManager() throw();
	const DIFInformation& getCurrentDIFInformation() const;
	void setCurrentDIFInformation(const DIFInformation& currentDIFInformation);
	unsigned short getIpcProcessId() const;
	void setIpcProcessId(unsigned short ipcProcessId);
	void setIPCManagerPort(unsigned int ipcManagerPort);

	/**
	 * Notify the IPC Manager about the successful initialization of the
	 * IPC Process Daemon. Now it is ready to receive messages.
	 * @param name the name of the IPC Process
	 * @throws IPCException if the process is already initialized or
	 * an error occurs
	 */
	void notifyIPCProcessInitialized(
	                const ApplicationProcessNamingInformation& name)
	throw (IPCException);

	/**
	 * True if the IPC Process has been successfully initialized, false
	 * otherwise
	 * @return
	 */
	bool isIPCProcessInitialized() const;

	/**
	 * The IPC Process has been registered to an N-1 DIF
	 * @param appName
	 * @param DIFName
	 * @return
	 */
	ApplicationRegistration * appRegistered(
	                        const ApplicationProcessNamingInformation& appName,
	                        const ApplicationProcessNamingInformation& DIFName)
	        throw (ApplicationRegistrationException);

	/**
	 * The IPC Process has been unregistered from the DIF called DIFName,
	 * update the internal data structrues
	 * @param appName
	 * @param DIFName
	 */
	void appUnregistered(const ApplicationProcessNamingInformation& appName,
	                const ApplicationProcessNamingInformation& DIFName)
	                                throw (ApplicationUnregistrationException);

	/**
	 * Reply to the IPC Manager, informing it about the result of an "assign
	 * to DIF" operation
	 * @param event the event that trigered the operation
	 * @param result the result of the operation (0 successful)
	 * @throws AssignToDIFResponseException
	 */
	void assignToDIFResponse(const AssignToDIFRequestEvent& event, int result)
		throw (AssignToDIFResponseException);

	/**
	 * Reply to the IPC Manager, informing it about the result of an "enroll
	 * to DIF" operation
	 * @param event the event that trigerred the operation
	 * @param result the result of the operation (0 successful)
	 * @param newNeighbors the new neighbors after the enrollment operation
	 * @param DIFInforamtion the DIF configuration after enrollment
	 * @throws EnrollException if there are problems communicating with the
	 * IPC Manager
	 */
	void enrollToDIFResponse(const EnrollToDIFRequestEvent& event,
	                int result, const std::list<Neighbor> & newNeighbors,
	                const DIFInformation& difInformation)
	throw (EnrollException);

	/**
	 * Inform the IPC Manager about new neighbors being added or existing
	 * neighbors that have been removed
	 * @param added true if the neighbors have been added, false if removed
	 * @param neighbors
	 * @throws EnrollException if there are problems communicating with the
	 * IPC Manager
	 */
	void notifyNeighborsModified(bool added,
	                const std::list<Neighbor> & neighbors)
	throw (EnrollException);

	/**
	 * Reply to the IPC Manager, informing it about the result of a "register
	 * application request" operation
	 * @param event
	 * @param result
	 * @throws RegisterApplicationResponseException
	 */
	void registerApplicationResponse(
			const ApplicationRegistrationRequestEvent& event, int result)
		throw (RegisterApplicationResponseException);

	/**
	 * Reply to the IPC Manager, informing it about the result of a "unregister
	 * application request" operation
	 * @param event
	 * @param result
	 * @throws UnregisterApplicationResponseException
	 */
	void unregisterApplicationResponse(
			const ApplicationUnregistrationRequestEvent& event, int result)
		throw (UnregisterApplicationResponseException);

	/**
	 * Reply to the IPC Manager, informing it about the result of a "allocate
	 * flow response" operation
	 * @param event
	 * @param result
	 * @throws AllocateFlowResponseException
	 */
	void allocateFlowRequestResult(const FlowRequestEvent& event, int result)
		throw (AllocateFlowResponseException);

	/**
	 * Tell the IPC Manager that an allocate flow request targeting a local
	 * application registered in this IPC Process has arrived. The IPC manager
	 * will contact the application and ask it if it accepts the flow. IF it
	 * does it, it will assign a port-id to the flow. Either way it will reply
	 * the IPC Process
	 * @param localAppName
	 * @param remoteAppName
	 * @param flowSpecification
	 * @param portId the portId for the flow
	 * @returns a handler to correlate the response
	 * @throws AllocateFlowRequestArrivedException if there are issues during
	 * the operation or the application rejects the flow
	 */
	unsigned int allocateFlowRequestArrived(
			const ApplicationProcessNamingInformation& localAppName,
			const ApplicationProcessNamingInformation& remoteAppName,
			const FlowSpecification& flowSpecification,
			int portId)
		throw (AllocateFlowRequestArrivedException);

        /**
         * Overrides IPCManager's operation
         * Requests the allocation of a Flow
         *
         * @param localAppName The naming information of the local application
         * @param remoteAppName The naming information of the remote application
         * @param flowSpecifiction The characteristics required for the flow
         * @return A handler to be able to identify the proper response event
         * @throws FlowAllocationException if there are problems during the flow allocation
         */
        unsigned int requestFlowAllocation(
                        const ApplicationProcessNamingInformation& localAppName,
                        const ApplicationProcessNamingInformation& remoteAppName,
                        const FlowSpecification& flow) throw (FlowAllocationException);

        /**
         * Overrides IPCManager's operation
         * Requests the allocation of a flow using a speficif dIF
         * @param localAppName The naming information of the local application
         * @param remoteAppName The naming information of the remote application
         * @param flowSpecifiction The characteristics required for the flow
         * @param difName The DIF through which we want the flow allocated
         * @return A handler to be able to identify the proper response event
         * @throws FlowAllocationException if there are problems during the flow allocation
         */
        unsigned int requestFlowAllocationInDIF(
                        const ApplicationProcessNamingInformation& localAppName,
                        const ApplicationProcessNamingInformation& remoteAppName,
                        const ApplicationProcessNamingInformation& difName,
                        const FlowSpecification& flow) throw (FlowAllocationException);

        /**
         * Overrides IPCManager's operation
         * Confirms or denies the request for a flow to this application.
         *
         * @param flowRequestEvent information of the flow request
         * @param result 0 means the flow is accepted, a different number
         * indicates the deny code
         * @param notifySource if true the source IPC Process will get
         * the allocate flow response message back, otherwise it will be ignored
         * @return Flow If the flow is accepted, returns the flow object
         * @throws FlowAllocationException If there are problems
         * confirming/denying the flow
         */
        Flow * allocateFlowResponse(const FlowRequestEvent& flowRequestEvent,
                        int result, bool notifySource)
        throw (FlowAllocationException);

	/**
	 * Invoked by the IPC Process to respond to the Application Process that
	 * requested a flow deallocation
	 * @param flowDeallocateEvent Object containing information about the flow
	 * deallocate request event
	 * @param result 0 indicates success, a negative number an error code
	 * @throws DeallocateFlowResponseException if there are issues
	 * replying ot the application
	 */
	void notifyflowDeallocated(const FlowDeallocateRequestEvent flowDeallocateEvent,
			int result)
		throw (DeallocateFlowResponseException);

	/**
	 * Invoked by the ipC Process to notify that a flow has been remotely
	 * unallocated
	 * @param portId
	 * @param code
	 * @throws DeallocateFlowResponseException
	 */
	void flowDeallocatedRemotely(int portId, int code)
		throw (DeallocateFlowResponseException);

	/**
	 * Reply to the IPC Manager, providing 0 or more RIB Objects in response to
	 * a "query RIB request"
	 * @param event
	 * @param result
	 * @param ribObjects
	 * @throws QueryRIBResponseException
	 */
	void queryRIBResponse(const QueryRIBRequestEvent& event, int result,
			const std::list<RIBObject>& ribObjects)
		throw (QueryRIBResponseException);

	/**
	 * Request an available portId to the kernel
	 * @param The id of the IPC Process that will be using the flow
	 * associated to the port-id requested (0 if is an application)
	 * @param appName The name of the application that requested
	 * the flow (could be an IPC Process or a regular application)
	 * @return the port-id
	 * @throws PortAllocationException if something goes wrong
	 */
	int allocatePortId(const ApplicationProcessNamingInformation& appName)
	        throw (PortAllocationException);

	/**
	 * Request the kernel to free a used port-id
	 * @param portId the port-id to be freed
	 * @throws PortAllocationException if something goes wrong
	 */
	void deallocatePortId(int portId) throw (PortAllocationException);
};

/**
 * Make Extended IPC Manager singleton
 */
extern Singleton<ExtendedIPCManager> extendedIPCManager;

/**
 * The DTCP window based flow control configuration
 */
class DTCPWindowBasedFlowControlConfig {

        /**
         * Integer that the number PDUs that can be put on the
         * ClosedWindowQueue before something must be done.
         */
        int maxclosedwindowqueuelength;

        /** initial sequence number to get right window edge. */
        int initialcredit;

        /**
         * Invoked when a Transfer PDU is received to give the receiving PM an
         * opportunity to update the flow control allocations.
         */
        PolicyConfig rcvrflowcontrolpolicy;

        /**
         * Allows some discretion in when to send a Flow Control PDU when there
         * is no Retransmission Control.
         */
        PolicyConfig receivingflowcontrolpolicy;

public:
        DTCPWindowBasedFlowControlConfig();
        int getInitialcredit() const;
        void setInitialcredit(int initialcredit);
        int getMaxclosedwindowqueuelength() const;
        void setMaxclosedwindowqueuelength(int maxclosedwindowqueuelength);
        const PolicyConfig& getRcvrflowcontrolpolicy() const;
        void setRcvrflowcontrolpolicy(
                        const PolicyConfig& rcvrflowcontrolpolicy);
        const PolicyConfig& getReceivingflowcontrolpolicy() const;
        void setReceivingflowcontrolpolicy(
                        const PolicyConfig& receivingflowcontrolpolicy);
};

/**
 * The DTCP rate-basd flow control configuration
 */
class DTCPRateBasedFlowControlConfig {

        /**
         * the number of PDUs that may be sent in a TimePeriod. Used with
         * rate-based flow control.
         */
        int sendingrate;

        /**
         * length of time in microseconds for pacing rate-based flow control.
         */
        int timeperiod;

        /** used to momentarily lower the send rate below the rate allowed */
        PolicyConfig norateslowdownpolicy;

        /**
         * Allows rate-based flow control to exceed its nominal rate.
         * Presumably this would be for short periods and policies should
         * enforce this.  Like all policies, if this returns True it creates
         * the default action which is no override.
         */
        PolicyConfig nooverridedefaultpeakpolicy;

        /**
         * Allows an alternate action when using rate-based flow control and
         * the number of free buffers is getting low.
         */
        PolicyConfig ratereductionpolicy;

public:
        DTCPRateBasedFlowControlConfig();
        const PolicyConfig& getNooverridedefaultpeakpolicy() const;
        void setNooverridedefaultpeakpolicy(
                        const PolicyConfig& nooverridedefaultpeakpolicy);
        const PolicyConfig& getNorateslowdownpolicy() const;
        void setNorateslowdownpolicy(
                        const PolicyConfig& norateslowdownpolicy);
        const PolicyConfig& getRatereductionpolicy() const;
        void setRatereductionpolicy(const PolicyConfig& ratereductionpolicy);
        int getSendingrate() const;
        void setSendingrate(int sendingrate);
        int getTimeperiod() const;
        void setTimeperiod(int timeperiod);
};

/**
 * The flow control configuration of a DTCP instance
 */
class DTCPFlowControlConfig {

        /** indicates whether window-based flow control is in use */
        bool windowbased;

        /** the window-based flow control configuration */
        DTCPWindowBasedFlowControlConfig windowbasedconfig;

        /** indicates whether rate-based flow control is in use */
        bool ratebased;

        /** the rate-based flow control configuration */
        DTCPRateBasedFlowControlConfig ratebasedconfig;

        /**
         * The number of free bytes below which flow control should slow or
         * block the user from doing any more Writes.
         */
        int sentbytesthreshold;

        /**
         * The percent of free bytes below, which flow control should slow or
         * block the user from doing any more Writes.
         */
        int sentbytespercentthreshold;

        /**
         * The number of free buffers below which flow control should slow or
         * block the user from doing any more Writes.
         */
        int sentbuffersthreshold;

        /**
         * The number of free bytes below which flow control does not move or
         * decreases the amount the Right Window Edge is moved.
         */
        int rcvbytesthreshold;

        /**
         * The number of free buffers at which flow control does not advance
         * or decreases the amount the Right Window Edge is moved.
         */
        int rcvbytespercentthreshold;

        /**
         * The percent of free buffers below which flow control should not
         * advance or decreases the amount the Right Window Edge is moved.
         */
        int rcvbuffersthreshold;

        /**
         * Used with flow control to determine the action to be taken when the
         * receiver has not extended more credit to allow the sender to send more
         * PDUs. Typically, the action will be to queue the PDUs until credit is
         * extended. This action is taken by DTCP, not DTP.
         */
        PolicyConfig closedwindowpolicy;

        /**
         * Determines what action to take if the receiver receives PDUs but the
         * credit or rate has been exceeded
         */
        PolicyConfig flowcontroloverrunpolicy;

        /**
         * Invoked when both Credit and Rate based flow control are in use and
         * they disagree on whether the PM can send or receive data. If it
         * returns True, then the PM can send or receive; if False, it cannot.
         */
        PolicyConfig reconcileflowcontrolpolicy;

public:
        DTCPFlowControlConfig();
        const PolicyConfig& getClosedwindowpolicy() const;
        void setClosedwindowpolicy(const PolicyConfig& closedwindowpolicy);
        const PolicyConfig& getFlowcontroloverrunpolicy() const;
        void setFlowcontroloverrunpolicy(
                        const PolicyConfig& flowcontroloverrunpolicy);
        bool isRatebased() const;
        void setRatebased(bool ratebased);
        const DTCPRateBasedFlowControlConfig& getRatebasedconfig() const;
        void setRatebasedconfig(
                        const DTCPRateBasedFlowControlConfig& ratebasedconfig);
        int getRcvbuffersthreshold() const;
        void setRcvbuffersthreshold(int rcvbuffersthreshold);
        int getRcvbytespercentthreshold() const;
        void setRcvbytespercentthreshold(int rcvbytespercentthreshold);
        int getRcvbytesthreshold() const;
        void setRcvbytesthreshold(int rcvbytesthreshold);
        const PolicyConfig& getReconcileflowcontrolpolicy() const;
        void setReconcileflowcontrolpolicy(
                        const PolicyConfig& reconcileflowcontrolpolicy);
        int getSentbuffersthreshold() const;
        void setSentbuffersthreshold(int sentbuffersthreshold);
        int getSentbytespercentthreshold() const;
        void setSentbytespercentthreshold(int sentbytespercentthreshold);
        int getSentbytesthreshold() const;
        void setSentbytesthreshold(int sentbytesthreshold);
        bool isWindowbased() const;
        void setWindowbased(bool windowbased);
        const DTCPWindowBasedFlowControlConfig& getWindowbasedconfig() const;
        void setWindowbasedconfig(
                        const DTCPWindowBasedFlowControlConfig&
                        windowbasedconfig);
};

/**
 * The configuration of the retransmission control functions of a
 * DTCP instance
 */
class DTCPRtxControlConfig{

        /**
         * the number of times the retransmission of a PDU will be attempted
         * before some other action must be taken.
         */
        int datarxmsnmax;

        /**
         * indicates the maximum time that a receiver will wait before sending
         * an Ack. Some DIFs may wish to set a maximum value for the DIF.
         */
        int initialATimer;

        /**
         * Executed by the sender to estimate the duration of the retx timer.
         * This policy will be based on an estimate of round-trip time and the
         * Ack or Ack List policy in use
         */
        PolicyConfig rttestimatorpolicy;

        /**
         * Executed by the sender when a Retransmission Timer Expires. If this
         * policy returns True, then all PDUs with sequence number less than
         * or equal to the sequence number of the PDU associated with this
         * timeout are retransmitted; otherwise the procedure must determine
         * what action to take. This policy must be executed in less than the
         * maximum time to Ack
         */
        PolicyConfig rtxtimerexpirypolicy;

        /**
         * Executed by the sender and provides the Sender with some discretion
         * on when PDUs may be deleted from the ReTransmissionQ. This is useful
         * for multicast and similar situations where one might want to delay
         * discarding PDUs from the retransmission queue.
         */
        PolicyConfig senderackpolicy;

        /**
         *  Executed by the Sender and provides the Sender with some discretion
         *  on when PDUs may be deleted from the ReTransmissionQ. This policy
         *  is used in conjunction with the selective acknowledgement aspects
         *  of the mechanism and may be useful for multicast and similar
         *  situations where there may be a requirement to delay discarding PDUs
         *  from the retransmission queue
         */
        PolicyConfig recvingacklistpolicy;

        /**
         * Executed by the receiver of the PDU and provides some discretion in
         * the action taken.  The default action is to either Ack immediately
         * or to start the A-Timer and Ack the LeftWindowEdge when it expires.
         */
        PolicyConfig rcvrackpolicy;

        /**
         * This policy allows an alternate action when the A-Timer expires when
         * DTCP is present.
         */
        PolicyConfig sendingackpolicy;

        /** Allows an alternate action when a Control Ack PDU is received. */
        PolicyConfig rcvrcontrolackpolicy;

public:
        DTCPRtxControlConfig();
        int getDatarxmsnmax() const;
        void setDatarxmsnmax(int datarxmsnmax);
        int getInitialATimer() const;
        void setInitialATimer(int initialATimer);
        const PolicyConfig& getRcvrackpolicy() const;
        void setRcvrackpolicy(const PolicyConfig& rcvrackpolicy);
        const PolicyConfig& getRcvrcontrolackpolicy() const;
        void setRcvrcontrolackpolicy(
                        const PolicyConfig& rcvrcontrolackpolicy);
        const PolicyConfig& getRecvingacklistpolicy() const;
        void setRecvingacklistpolicy(
                        const PolicyConfig& recvingacklistpolicy);
        const PolicyConfig& getRttestimatorpolicy() const;
        void setRttestimatorpolicy(const PolicyConfig& rttestimatorpolicy);
        const PolicyConfig& getRtxtimerexpirypolicy() const;
        void setRtxtimerexpirypolicy(
                        const PolicyConfig& rtxtimerexpirypolicy);
        const PolicyConfig& getSenderackpolicy() const;
        void setSenderackpolicy(const PolicyConfig& senderackpolicy);
        const PolicyConfig& getSendingackpolicy() const;
        void setSendingackpolicy(const PolicyConfig& sendingackpolicy);
};

/**
 * Configuration of the DTCP instance, including policies and its parameters
 */
class DTCPConfig {

        /** True if flow control is required */
        bool flowcontrol;

        /** the flow control configuration of a DTCP instance */
        DTCPFlowControlConfig flowcontrolconfig;

        /** True if rtx control is required */
        bool rtxcontrol;

        /** the rtx control configuration of a DTCP instance */
        DTCPRtxControlConfig rtxcontrolconfig;

        /**
         * should be approximately 2Δt. This must be bounded. A DIF
         * specification may want to specify a maximum value.
         */
        int initialsenderinactivitytime;

        /**
         * should be approximately 3Δt. This must be bounded. A DIF
         * specification may want to specify a maximum value.
         */
        int initialrecvrinactivitytime;

        /**
         * used when DTCP is in use. If no PDUs arrive in this time period,
         * the receiver should expect a DRF in the next Transfer PDU. If not,
         * something is very wrong. The timeout value should generally be set
         * to 3(MPL+R+A).
         */
        PolicyConfig rcvrtimerinactivitypolicy;

        /**
         * used when DTCP is in use. This timer is used to detect long periods
         * of no traffic, indicating that a DRF should be sent. If not,
         * something is very wrong. The timeout value should generally be set
         * to 2(MPL+R+A).
         */
        PolicyConfig sendertimerinactiviypolicy;

        /**
         * This policy determines what action to take when the PM detects that
         * a control PDU (Ack or Flow Control) may have been lost.  If this
         * procedure returns True, then the PM will send a Control Ack and an
         * empty Transfer PDU.  If it returns False, then any action is determined
         * by the policy
         */
        PolicyConfig lostcontrolpdupolicy;

public:
        DTCPConfig();
        bool isFlowcontrol() const;
        void setFlowcontrol(bool flowcontrol);
        const DTCPFlowControlConfig& getFlowcontrolconfig() const;
        void setFlowcontrolconfig(
                        const DTCPFlowControlConfig& flowcontrolconfig);
        int getInitialrecvrinactivitytime() const;
        void setInitialrecvrinactivitytime(int initialrecvrinactivitytime);
        int getInitialsenderinactivitytime() const;
        void setInitialsenderinactivitytime(int initialsenderinactivitytime);
        const PolicyConfig& getLostcontrolpdupolicy() const;
        void setLostcontrolpdupolicy(
                        const PolicyConfig& lostcontrolpdupolicy);
        const PolicyConfig& getRcvrtimerinactivitypolicy() const;
        void setRcvrtimerinactivitypolicy(
                        const PolicyConfig& rcvrtimerinactivitypolicy);
        bool isRtxcontrol() const;
        void setRtxcontrol(bool rtxcontrol);
        const DTCPRtxControlConfig& getRtxcontrolconfig() const;
        void setRtxcontrolconfig(const DTCPRtxControlConfig& rtxcontrolconfig);
        const PolicyConfig& getSendertimerinactiviypolicy() const;
        void setSendertimerinactiviypolicy(
                        const PolicyConfig& sendertimerinactiviypolicy);
};

/**
 * This class defines the policies paramenters for an EFCP connection
 */
class ConnectionPolicies {
	/** Indicates if DTCP is required */
	bool DTCPpresent;

	/** The configuration of the DTCP instance */
	DTCPConfig dtcpConfiguration;

	/**
	 * This policy allows some discretion in selecting the initial sequence
	 * number, when DRF is going to be sent.
	 */
	PolicyConfig initialseqnumpolicy;

	/**
	 * When the sequence number is increasing beyond this value, the
	 * sequence number space is close to rolling over, a new connection
	 * should be instantiated and bound to the same port-ids, so that new
	 * PDUs can be sent on the new connection.
	 */
	int seqnumrolloverthreshold;

public:
	ConnectionPolicies();
	const DTCPConfig& getDtcpConfiguration() const;
	void setDtcpConfiguration(const DTCPConfig& dtcpConfiguration);
	bool isDtcPpresent() const;
	void setDtcPpresent(bool dtcPpresent);
	const PolicyConfig& getInitialseqnumpolicy() const;
	void setInitialseqnumpolicy(const PolicyConfig& initialseqnumpolicy);
	int getSeqnumrolloverthreshold() const;
	void setSeqnumrolloverthreshold(int seqnumrolloverthreshold);
};

/**
 * Represents the data to create an EFCP connection
 */
class Connection {
        /** The port-id to which the connection is bound */
        int portId;

        /** The address of the IPC Process at the source of the conection */
        unsigned int sourceAddress;

        /**
         * The address of the IPC Process at the destination of
         * the connection
         */
        unsigned int destAddress;

        /**
         * The id of the QoS cube associated to the connection
         */
        unsigned int qosId;

        /**
         * The source CEP-id
         */
        int sourceCepId;

        /**
         * The destination CEP-id
         */
        int destCepId;

        /**
         * The EFCP connection policies
         */
        ConnectionPolicies connPoliciesParams;

        /**
         * The id of the IPC Process using the flow supported by this
         * connection (0 if it is an application that is not an IPC Process)
         */
        unsigned short flowUserIpcProcessId;

public:
        Connection();
        unsigned int getDestAddress() const;
        void setDestAddress(unsigned int destAddress);
        int getPortId() const;
        void setPortId(int portId);
        unsigned int getQosId() const;
        void setQosId(unsigned int qosId);
        unsigned int getSourceAddress() const;
        void setSourceAddress(unsigned int sourceAddress);
        int getDestCepId() const;
        void setDestCepId(int destCepId);
        unsigned short getFlowUserIpcProcessId() const;
        void setFlowUserIpcProcessId(unsigned short flowUserIpcProcessId);
        int getSourceCepId() const;
        void setSourceCepId(int sourceCepId);
        const ConnectionPolicies& getConnPolicies() const;
        void setConnPolicies(const ConnectionPolicies& connPParams);
        const std::string toString();
};

/**
 * Models an entry in the PDU Forwarding Table
 */
class PDUForwardingTableEntry {
        /** The destination address */
        unsigned int address;

        /** The qos-id */
        unsigned int qosId;

        /** The N-1 portid */
        std::list<unsigned int> portIds;
public:
        PDUForwardingTableEntry();
        bool operator==(const PDUForwardingTableEntry &other) const;
        bool operator!=(const PDUForwardingTableEntry &other) const;
        unsigned int getAddress() const;
        void setAddress(unsigned int address);
        const std::list<unsigned int> getPortIds() const;
        void setPortIds(const std::list<unsigned int>& portIds);
        void addPortId(unsigned int portId);
        unsigned int getQosId() const;
        void setQosId(unsigned int qosId);
        const std::string toString();
};

/**
 * Response of the Kernel IPC Process, reporting on the
 * number of entries in the PDU Forwarding Table for this
 * IPC Process
 */
class DumpFTResponseEvent: public IPCEvent {

        /** The PDU Forwarding Table entries*/
        std::list<PDUForwardingTableEntry> entries;

        /** Result of the operation, 0 success */
        int result;

public:
        DumpFTResponseEvent(const std::list<PDUForwardingTableEntry>& entries,
                        int result, unsigned int sequenceNumber);
        const std::list<PDUForwardingTableEntry>& getEntries() const;
        int getResult() const;
};


/**
 * FIXME: Quick hack to get multiple parameters back
 */
class ReadManagementSDUResult {
        int bytesRead;
        int portId;

public:
        ReadManagementSDUResult();
        int getBytesRead() const;
        void setBytesRead(int bytesRead);
        int getPortId() const;
        void setPortId(int portId);
};

/**
 * Abstraction of the data transfer and data transfer control parts of the
 * IPC Process, which are implemented in the Kernel. This class allows the
 * IPC Process Daemon to communicate with its components in the kernel
 */
class KernelIPCProcess {
        /** The ID of the IPC Process */
        unsigned short ipcProcessId;

public:
        void setIPCProcessId(unsigned short ipcProcessId);
        unsigned short getIPCProcessId() const;

        /**
         * Invoked by the IPC Process Deamon to allow the kernel components
         * to update its internal configuration based on the DIF the IPC
         * Process has been assigned to.
         *
         * @param difInformation The information of the DIF (name, type,
         * configuration)
         * @throws AssignToDIFException if an error happens during the process
         * @returns the handle to the response message
         */
        unsigned int assignToDIF(const DIFInformation& difInformation)
                throw (AssignToDIFException);

        /**
         * Invoked by the IPC Process Daemon to modify the configuration of
         * the kernel components of the IPC Process.
         *
         * @param difConfiguration The configuration of the DIF
         * @throws UpdateDIFConfigurationException if an error happens during
         * the process
         * @returns the handle to the response message
         */
        unsigned int updateDIFConfiguration(
                        const DIFConfiguration& difConfiguration)
        throw (UpdateDIFConfigurationException);

        /**
         * Invoked by the IPC Process Daemon to request the creation of an
         * EFCP connection to the kernel components of the IPC Process
         *
         * @param connection
         * @param connectionPolicies the policies for this EFCP connection
         * @throws CreateConnectionException
         * @return the handle to the response message
         */
        unsigned int createConnection(const Connection& connection,
                        const ConnectionPolicies& connectionPolicies)
        throw (CreateConnectionException);

        /**
         * Invoked by the IPC Process Daemon to request an update of an
         * EFCP connection to the kernel components of the IPC Process
         *
         * @param connection
         * @throws UpdateConnectionException
         * @return the handle to the response message
         */
        unsigned int updateConnection(const Connection& connection)
        throw (UpdateConnectionException);

        /**
         * Invoked by the IPC Process Daemon to request the creation of an
         * EFCP connection to the kernel components of the IPC Process
         * (receiving side of the Flow allocation procedure)
         *
         * @param connection
         * @param connectionPolicies the policies for this EFCP connection
         * @throws CreateConnectionException
         * @return the handle to the response message
         */
        unsigned int createConnectionArrived(const Connection& connection,
                        const ConnectionPolicies& connectionPolicies)
        throw (CreateConnectionException);

        /**
         * Invoked by the IPC Process Daemon to request the destruction of an
         * EFCP connection to the kernel components of the IPC Process
         *
         * @param connection
         * @throws DestroyConnectionException
         * @return the handle to the response message
         */
        unsigned int destroyConnection(const Connection& connection)
        throw (DestroyConnectionException);

        /**
         * Modify the entries of the PDU forwarding table
         * @param entries to be modified
         * @param mode 0 add, 1 remove, 2 flush and add
         */
        void modifyPDUForwardingTableEntries(const std::list<PDUForwardingTableEntry>& entries,
                        int mode) throw (PDUForwardingTableException);

        /**
         * Request the Kernel IPC Process to provide a list of
         * all the entries in the PDU Forwarding table
         * @return a handle to the response event
         * @throws PDUForwardingTabeException if something goes wrong
         */
        unsigned int dumptPDUFT() throw (PDUForwardingTableException);

        /**
         * Requests the kernel to write a management SDU to the
         * portId specified
         *
         * @param sdu A buffer that contains the SDU data
         * @param size The size of the SDU data, in bytes
         * @param portId The N-1 portId where the data has to be written to
         * @throws WriteSDUException
         */
        void writeManagementSDU(void * sdu, int size, int portId)
                throw (WriteSDUException);

        /**
         * Requests the kernel to get a management SDU from a peer
         * IPC Process. This operation will block until there is an SDU available
         *
         * @param sdu A buffer to store the SDU data
         * @param maxBytes The maximum number of bytes to read
         * @return int The number of bytes read and the portId where they have
         * been read from
         * @throws ReadSDUException
         */
        ReadManagementSDUResult readManagementSDU(void * sdu, int maxBytes)
                throw (ReadSDUException);
};

/**
 * Make Kernel IPC Process singleton
 */
extern Singleton<KernelIPCProcess> kernelIPCProcess;

}

#endif

#endif
