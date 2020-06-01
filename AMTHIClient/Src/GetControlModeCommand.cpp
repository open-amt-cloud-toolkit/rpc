/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2010-2019 Intel Corporation
 */
/*++

@file: GetControlModeCommand.cpp

--*/

#include "GetControlModeCommand.h"
#include "StatusCodeDefinitions.h"
#include <string.h>

using namespace std;

using namespace Intel::MEI_Client::AMTHI_Client;

GetControlModeCommand::GetControlModeCommand()
{
	shared_ptr<MEICommandRequest> tmp(new GetControlModeRequest());
	m_request = tmp;
	Transact();
}

void GetControlModeCommand::reTransact()
{
	shared_ptr<MEICommandRequest> tmp(new GetControlModeRequest());
	m_request = tmp;
	Transact();
}

GET_CONTROL_MODE_RESPONSE GetControlModeCommand::getResponse()
{
	return m_response->getResponse();
}

void
GetControlModeCommand::parseResponse(const vector<uint8_t>& buffer)
{
	shared_ptr<AMTHICommandResponse<GET_CONTROL_MODE_RESPONSE>> tmp(
		new AMTHICommandResponse<GET_CONTROL_MODE_RESPONSE>(buffer, RESPONSE_COMMAND_NUMBER));
	m_response = tmp;
}

std::vector<uint8_t> 
GetControlModeRequest::SerializeData()
{
	vector<uint8_t> output;
	return output;
}
