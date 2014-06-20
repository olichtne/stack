//
// Test-CDAP
//
//	  Bernat Gaston			<bernat.gaston@i2cat.net>
//    Eduard Grasa          <eduard.grasa@i2cat.net>
//    Francesco Salvestrini <f.salvestrini@nextworks.it>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301  USA
//
#include <iostream>
#include "librina/cdap.h"

using namespace rina;

bool test1(CDAPSessionManagerInterface *session_manager,
		CDAPSessionInterface *session) {
	AuthValue auth_value;
	const CDAPMessage *sent_message =
			session_manager->getOpenConnectionRequestMessage(1,
					CDAPMessage::AUTH_NONE, auth_value, "1", "dest instance",
					"1", "dest", "1", "src instance", "1", "src");
	const char *serialized_message = session->encodeNextMessageToBeSent(
			*sent_message);
	session->messageSent(*sent_message);



	const CDAPMessage *recevied_message = session->messageReceived(
			serialized_message);

	if (sent_message->to_string() == recevied_message->to_string())	{
		delete sent_message;
		delete recevied_message;
		return true;
	}
	else {
		delete sent_message;
		delete recevied_message;
		return false;
	}
}

int main() {
	WireMessageProviderFactory wire_factory;
	CDAPSessionManagerFactory cdap_manager_factory;
	long timeout = 2000;
	CDAPSessionManagerInterface *session_manager =
			cdap_manager_factory.createCDAPSessionManager(&wire_factory,
					timeout);
	CDAPSessionInterface *session = session_manager->createCDAPSession(1);

	if (!test1(session_manager, session))
		return -1;

	delete session;
	delete session_manager;
	return 0;
}
