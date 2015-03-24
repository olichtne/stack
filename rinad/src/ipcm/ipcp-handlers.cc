/*
 * IPC Manager - IPCP related routine handlers
 *
 *    Vincenzo Maffione     <v.maffione@nextworks.it>
 *
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

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <utility>

#include <librina/common.h>
#include <librina/ipc-manager.h>

#define RINA_PREFIX "ipcm.ipcp"
#include <librina/logs.h>

#include "rina-configuration.h"
#include "ipcp-handlers.h"

using namespace std;


namespace rinad {

#define IPCP_DAEMON_INIT_RETRIES 5

void IPCManager_::ipc_process_daemon_initialized_event_handler(
				rina::IPCProcessDaemonInitializedEvent * e)
{
	ostringstream ss;
	IPCMIPCProcess* ipcp;
	int i;
	SyscallTransState* trans = NULL;

	//There can be race condition between the caller and us (notification)
	for(i=0; i<IPCP_DAEMON_INIT_RETRIES; ++i){
		trans = get_syscall_transaction_state(e->ipcProcessId);
		if(trans)
			break;
	}

	if(!trans){
		ss << ": Warning: IPCP daemon '"<<e->ipcProcessId<<"'initialized, but no pending normal IPC process initialization. Corrupted state?"<< endl;
		assert(0);
		return;
	}

	//Recover IPCP process with writelock
	ipcp = lookup_ipcp_by_id(e->ipcProcessId, true);

	//If the ipcp is not there, there is some corruption
	if(!ipcp){
		ss << ": Warning: IPCP daemon '"<<e->ipcProcessId<<"'initialized, but no pending normal IPC process initialization. Corrupted state?"<< endl;
		FLUSH_LOG(WARN, ss);
		assert(0);

		//Set promise return
		trans->completed(IPCM_FAILURE);

		//Remove syscall transaction and return
		remove_syscall_transaction_state(e->ipcProcessId);
		return;
	}

	//Auto release the read lock
	rina::WriteScopedLock writelock(ipcp->rwlock, false);

	assert(ipcp->get_type() == rina::NORMAL_IPC_PROCESS);

	//Initialize
	ipcp->setInitialized();

	ss << "IPC process daemon initialized [id = " <<
		e->ipcProcessId<< "]" << endl;
	FLUSH_LOG(INFO, ss);

	//Set return value, mark as completed and signal
	trans->completed(IPCM_SUCCESS);
	remove_transaction_state(trans->tid);

	return;
}

int IPCManager_::ipcm_register_response_ipcp(
	rina::IpcmRegisterApplicationResponseEvent *e)
{
	ostringstream ss;
	ipcm_res_t ret = IPCM_FAILURE;

	IPCPregTransState* trans = get_transaction_state<IPCPregTransState>(e->sequenceNumber);

	if(!trans){
		assert(0);
		return -1;
	}

	rinad::IPCMIPCProcess *ipcp = lookup_ipcp_by_id(trans->ipcp_id);
	if(!ipcp)
		return -1;
	//Auto release the read lock
	rina::ReadScopedLock readlock(ipcp->rwlock, false);

	rinad::IPCMIPCProcess *slave_ipcp = lookup_ipcp_by_id(trans->slave_ipcp_id);
	if(!slave_ipcp)
		return -1;
	//Auto release the read lock
	rina::ReadScopedLock sreadlock(slave_ipcp->rwlock, false);

	const rina::ApplicationProcessNamingInformation& slave_dif_name =
						slave_ipcp->dif_name_;

	// Notify the registered IPC process.
	if( ipcm_register_response_common(e, ipcp->get_name(), slave_ipcp,
				slave_dif_name) ){
		try {
			ipcp->notifyRegistrationToSupportingDIF(
					slave_ipcp->get_name(),
					slave_dif_name
					);
			ss << "IPC process " << ipcp->get_name().toString() <<
				" informed about its registration "
				"to N-1 DIF " <<
				slave_dif_name.toString() << endl;
			FLUSH_LOG(INFO, ss);

			ret = IPCM_SUCCESS;

		} catch (rina::NotifyRegistrationToDIFException) {
			ss  << ": Error while notifying "
				"IPC process " <<
				ipcp->get_name().toString() <<
				" about registration to N-1 DIF"
				<< slave_dif_name.toString() << endl;
			FLUSH_LOG(ERR, ss);
		}
	} else {
		ss  << "Cannot register IPC process "
			<< ipcp->get_name().toString() <<
			" to DIF " << slave_dif_name.toString() << endl;
		FLUSH_LOG(ERR, ss);
	}

	//Set return value, mark as completed and signal
	trans->completed(ret);
	remove_transaction_state(trans->tid);

	return -(ret == IPCM_SUCCESS);
}

int IPCManager_::ipcm_unregister_response_ipcp(
			rina::IpcmUnregisterApplicationResponseEvent *e)
{
	ostringstream ss;
	bool success;
	ipcm_res_t ret = IPCM_FAILURE;

	IPCPregTransState* trans = get_transaction_state<IPCPregTransState>(e->sequenceNumber);

	if(!trans){
		assert(0);
		return -1;
	}

	rinad::IPCMIPCProcess *ipcp = lookup_ipcp_by_id(trans->ipcp_id);
	if(!ipcp)
		return -1;

	//Auto release the read lock
	rina::ReadScopedLock readlock(ipcp->rwlock, false);

	rinad::IPCMIPCProcess *slave_ipcp = lookup_ipcp_by_id(trans->slave_ipcp_id);
	if(!slave_ipcp)
		return -1;

	//Auto release the read lock
	rina::ReadScopedLock sreadlock(slave_ipcp->rwlock, false);

	const rina::ApplicationProcessNamingInformation& slave_dif_name =
							slave_ipcp->dif_name_;

	// Inform the supporting IPC process
	if(ipcm_unregister_response_common(e, slave_ipcp,
						  ipcp->get_name())){
		try {
			// Notify the IPCP process that it has been unregistered
			// from the DIF
			ipcp->notifyUnregistrationFromSupportingDIF(
							slave_ipcp->get_name(),
							slave_dif_name);
			ss << "IPC process " << ipcp->get_name().toString() <<
				" informed about its unregistration from DIF "
				<< slave_dif_name.toString() << endl;
			FLUSH_LOG(INFO, ss);

			ret = IPCM_SUCCESS;
		} catch (rina::NotifyRegistrationToDIFException) {
			ss  << ": Error while reporing "
				"unregistration result for IPC process "
				<< ipcp->get_name().toString() << endl;
			FLUSH_LOG(ERR, ss);
		}
	} else {
		ss  << ": Cannot unregister IPC Process "
			<< ipcp->get_name().toString() << " from DIF " <<
			slave_dif_name.toString() << endl;
		FLUSH_LOG(ERR, ss);
	}

	//Set return value, mark as completed and signal
	trans->completed(ret);
	remove_transaction_state(trans->tid);

	return -(ret == IPCM_SUCCESS);
}

void
IPCManager_::application_unregistered_event_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

void
IPCManager_::assign_to_dif_request_event_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

void
IPCManager_::assign_to_dif_response_event_handler(rina::AssignToDIFResponseEvent* e)
{
	ostringstream ss;
	bool success = (e->result == 0);
	IPCMIPCProcess* ipcp;
	ipcm_res_t ret;

	IPCPTransState* trans = get_transaction_state<IPCPTransState>(e->sequenceNumber);

	if(!trans){
		ss << ": Warning: unknown assign to DIF response received: "<<e->sequenceNumber<<endl;
		FLUSH_LOG(WARN, ss);
		return;
	}

	// Inform the IPC process about the result of the
	// DIF assignment operation
	try {
		ipcp = lookup_ipcp_by_id(trans->ipcp_id);
		if(!ipcp){
			ss << ": Warning: Could not complete assign to dif action: "<<e->sequenceNumber<<
			"IPCP with id: "<<trans->ipcp_id<<"does not exist! Perhaps deleted?" << endl;
			FLUSH_LOG(WARN, ss);
			throw rina::AssignToDIFException();
		}
		//Auto release the read lock
		rina::ReadScopedLock readlock(ipcp->rwlock, false);

		ipcp->assignToDIFResult(success);

		ss << "DIF assignment operation completed for IPC "
			<< "process " << ipcp->get_name().toString() <<
			" [success=" << success << "]" << endl;
		FLUSH_LOG(INFO, ss);
		ret = (success)? IPCM_SUCCESS : IPCM_FAILURE;
	} catch (rina::AssignToDIFException) {
		ss << ": Error while reporting DIF "
			"assignment result for IPC process "
			<< ipcp->get_name().toString() << endl;
		FLUSH_LOG(ERR, ss);
		ret = IPCM_FAILURE;
	}
	//Mark as completed
	trans->completed(ret);
	remove_transaction_state(trans->tid);
}

void
IPCManager_::update_dif_config_request_event_handler(rina::IPCEvent *event)
{
	(void)event;
}

void
IPCManager_::update_dif_config_response_event_handler(rina::UpdateDIFConfigurationResponseEvent* e)
{
	ostringstream ss;
	bool success = (e->result == 0);
	IPCMIPCProcess* ipcp;
	ipcm_res_t ret;

	IPCPTransState* trans = get_transaction_state<IPCPTransState>(e->sequenceNumber);

	if(!trans){
		ss << ": Warning: unknown DIF config response received: "<<e->sequenceNumber<<endl;
		FLUSH_LOG(WARN, ss);
		return;
	}

	try {
		ipcp = lookup_ipcp_by_id(trans->ipcp_id);
		if(!ipcp){
			ss << ": Warning: Could not complete config to dif action: "<<e->sequenceNumber<<
			"IPCP with id: "<<trans->ipcp_id<<"does not exist! Perhaps deleted?" << endl;
			FLUSH_LOG(WARN, ss);
			throw rina::UpdateDIFConfigurationException();
		}
		//Auto release the read lock
		rina::ReadScopedLock readlock(ipcp->rwlock, false);


		// Inform the requesting IPC process about the result of
		// the configuration update operation
		ss << "Configuration update operation completed for IPC "
			<< "process " << ipcp->get_name().toString() <<
			" [success=" << success << "]" << endl;
		FLUSH_LOG(INFO, ss);
		ret = IPCM_SUCCESS;
	} catch (rina::UpdateDIFConfigurationException) {
		ss  << ": Error while reporting DIF "
			"configuration update for process " <<
			ipcp->get_name().toString() << endl;
		FLUSH_LOG(ERR, ss);
		ret = IPCM_FAILURE;
	}
	//Mark as completed
	trans->completed(ret);
	remove_transaction_state(trans->tid);
}

void
IPCManager_::enroll_to_dif_request_event_handler(rina::IPCEvent *event)
{
	(void) event; // Stop compiler barfs
}

void
IPCManager_::enroll_to_dif_response_event_handler(rina::EnrollToDIFResponseEvent *event)
{
	ostringstream ss;
	IPCMIPCProcess *ipcp;
	bool success = (event->result == 0);
	ipcm_res_t ret = IPCM_FAILURE;

	IPCPTransState* trans = get_transaction_state<IPCPTransState>(event->sequenceNumber);

	if(!trans){
		ss << ": Warning: unknown enrollment to DIF response received: "<<event->sequenceNumber<<endl;
		FLUSH_LOG(WARN, ss);
		return;
	}

	ipcp = lookup_ipcp_by_id(trans->ipcp_id);
	if(!ipcp){
		ss << ": Warning: Could not complete enroll to dif action: "<<event->sequenceNumber<<
		"IPCP with id: "<<trans->ipcp_id<<" does not exist! Perhaps deleted?" << endl;
		FLUSH_LOG(WARN, ss);
	}else{
		//Auto release the read lock
		rina::ReadScopedLock readlock(ipcp->rwlock, false);


		if (success) {
			ss << "Enrollment operation completed for IPC "
				<< "process " << ipcp->get_name().toString() << endl;
			FLUSH_LOG(INFO, ss);

			ret = IPCM_SUCCESS;
		} else {
			ss  << ": Error: Enrollment operation of "
				"process " << ipcp->get_name().toString() << " failed"
				<< endl;
			FLUSH_LOG(ERR, ss);
		}
	}

	//Mark as completed
	trans->completed(ret);
	remove_transaction_state(trans->tid);
}

void IPCManager_::neighbors_modified_notification_event_handler(rina::NeighborsModifiedNotificationEvent* event)
{
	ostringstream ss;

	if (!event->neighbors.size()) {
		ss  << ": Warning: Empty neighbors-modified "
			"notification received" << endl;
		FLUSH_LOG(WARN, ss);
		return;
	}

	IPCPTransState* trans = get_transaction_state<IPCPTransState>(event->sequenceNumber);

	if(!trans){
		ss << ": Warning: unknown enrollment to DIF response received: "<<event->sequenceNumber<<endl;
		FLUSH_LOG(WARN, ss);
		return;
	}

	IPCMIPCProcess* ipcp = lookup_ipcp_by_id(trans->ipcp_id);
	if(!ipcp){
		ss  << ": Error: IPC process unexpectedly "
			"went away" << endl;
		FLUSH_LOG(ERR, ss);
		return;
	}

	//Auto release the read lock
	rina::ReadScopedLock readlock(ipcp->rwlock, false);

	ss << "Neighbors update [" << (event->added ? "+" : "-") <<
		"#" << event->neighbors.size() << "]for IPC process " <<
		ipcp->get_name().toString() <<  endl;
	FLUSH_LOG(INFO, ss);

}

void IPCManager_::ipc_process_dif_registration_notification_handler(rina::IPCEvent *event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::ipc_process_query_rib_handler(rina::IPCEvent *event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::get_dif_properties_handler(rina::IPCEvent *event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::get_dif_properties_response_event_handler(rina::IPCEvent *event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::ipc_process_create_connection_response_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::ipc_process_update_connection_response_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::ipc_process_create_connection_result_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::ipc_process_destroy_connection_result_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

void IPCManager_::ipc_process_dump_ft_response_handler(rina::IPCEvent * event)
{
	(void) event;  // Stop compiler barfs
}

} //namespace rinad
