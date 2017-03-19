////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2017, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InterceptorHelper.h"
#include "GameConstants.h"
#include "GameImageHooker.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraAddressInterceptor();
	void cameraWriteInterceptor1();		// create as much interceptors for write interception as needed. In the example game, there are 4.
	void cameraWriteInterceptor2();
	void cameraWriteInterceptor3();
	void cameraReadInterceptor1();
	void gamespeedAddressInterceptor();
}

// external addresses used in asm.
extern "C" {
	// The continue address for continuing execution after camera values address interception. 
	LPBYTE _cameraStructInterceptionContinue = 0;
	// the continue address for continuing execution after interception of the first block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue1 = 0;
	// the continue address for continuing execution after interception of the second block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue2 = 0;
	// the continue address for continuing execution after interception of the third block of code which writes to the camera values. 
	LPBYTE _cameraWriteInterceptionContinue3 = 0;
	// the continue address for the continuing execution after interception of the gamespeed block of code. 
	LPBYTE _gamespeedInterceptionContinue = 0;
	// the continue address for the continuing exeuction after interception of the camera quaternion read code.
	LPBYTE _cameraReadInterceptionContinue1 = 0;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void setCameraStructInterceptorHook(LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(hostImageAddress, CAMERA_ADDRESS_INTERCEPT_START_OFFSET, CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	}


	void setCameraWriteInterceptorHooks(LPBYTE hostImageAddress)
	{
		// for each block of code that writes to the camera values we're manipulating we need an interception to block it. For the example game there are 3. 
		GameImageHooker::setHook(hostImageAddress, CAMERA_WRITE_INTERCEPT1_START_OFFSET, CAMERA_WRITE_INTERCEPT1_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue1, &cameraWriteInterceptor1);
		GameImageHooker::setHook(hostImageAddress, CAMERA_WRITE_INTERCEPT2_START_OFFSET, CAMERA_WRITE_INTERCEPT2_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue2, &cameraWriteInterceptor2);
		GameImageHooker::setHook(hostImageAddress, CAMERA_WRITE_INTERCEPT3_START_OFFSET, CAMERA_WRITE_INTERCEPT3_CONTINUE_OFFSET, &_cameraWriteInterceptionContinue3, &cameraWriteInterceptor3);
		GameImageHooker::setHook(hostImageAddress, CAMERA_READ_INTERCEPT1_START_OFFSET, CAMERA_READ_INTERCEPT1_CONTINUE_OFFSET, &_cameraReadInterceptionContinue1, &cameraReadInterceptor1);
	}


	void setTimestopInterceptorHook(LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(hostImageAddress, GAMESPEED_ADDRESS_INTERCEPT_START_OFFSET, GAMESPEED_ADDRESS_INTERCEPT_CONTINUE_OFFSET, &_gamespeedInterceptionContinue, &gamespeedAddressInterceptor);
	}


	// The FoV write is disabled with NOPs, as the code block contains jumps out of the block so it's not easy to intercept with a silent method. It's OK though
	// as the FoV changes simply change a value, so instead of the game keeping it at a value we overwrite it. We reset it to a default value when the FoV is disabled by the user 
	void disableFoVWrite(LPBYTE hostImageAddress)
	{
		GameImageHooker::nopRange(hostImageAddress + FOV_WRITE_INTERCEPT1_START_OFFSET, 2);
		GameImageHooker::nopRange(hostImageAddress + FOV_WRITE_INTERCEPT2_START_OFFSET, 8);
	}
}