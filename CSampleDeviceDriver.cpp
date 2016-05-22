#include "CSampleDeviceDriver.h"
#include "driverlog.h"
#include <openvr_driver.h>
#include <sstream>
using namespace vr;

inline HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z)
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CSampleDeviceDriver::CSampleDeviceDriver(vr::IServerDriverHost *pDriverHost) : m_pServerDriverHost(pDriverHost)
, m_unObjectId(vr::k_unTrackedDeviceIndexInvalid)
{
}

CSampleDeviceDriver::~CSampleDeviceDriver()
{
	m_pServerDriverHost = NULL;
}

EVRInitError CSampleDeviceDriver::Activate(uint32_t unObjectId)
{
	m_unObjectId = unObjectId;
	return VRInitError_None;
}

void CSampleDeviceDriver::Deactivate()
{
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void *CSampleDeviceDriver::GetComponent(const char *pchComponentNameAndVersion)
{
	if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
	{
		return (vr::IVRDisplayComponent*)this;
	}

	return NULL;
}

void CSampleDeviceDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

VRControllerState_t CSampleDeviceDriver::GetControllerState()
{
	return vr::VRControllerState_t();
}

bool CSampleDeviceDriver::TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds)
{
	return false;
}

void CSampleDeviceDriver::PowerOff()
{
}

DriverPose_t CSampleDeviceDriver::GetPose()
{
	DriverLog("<VVR> CSampleDeviceDriver::GetPose");
	DriverPose_t pose = { 0 };
	pose.poseIsValid = true;
	pose.result = TrackingResult_Running_OK;
	pose.deviceIsConnected = true;

	pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
	pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

	pose.poseTimeOffset = 0.016f;

	pose.vecPosition[0] = 0;
	pose.vecPosition[1] = 0;
	pose.vecPosition[2] = 0;

	pose.qRotation = HmdQuaternion_Init(1, 0, 0, 0);

	return pose;
}

// TODO: For some reason this function is called repeatedly over and over again with value 1025
bool CSampleDeviceDriver::GetBoolTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
	*pError = vr::TrackedProp_Success;
	return false;
}

float CSampleDeviceDriver::GetFloatTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
	*pError = vr::TrackedProp_ValueNotProvidedByDevice;
	return 0.0f;
}

int32_t CSampleDeviceDriver::GetInt32TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
	int32_t nRetVal = 0;
	vr::ETrackedPropertyError error = vr::TrackedProp_UnknownProperty;
	switch (prop)
	{
	case vr::Prop_DeviceClass_Int32:
		nRetVal = vr::TrackedDeviceClass_Controller;
		error = vr::TrackedProp_Success;
		break;

	case vr::Prop_Axis0Type_Int32:
		nRetVal = vr::k_eControllerAxis_Joystick;
		error = vr::TrackedProp_Success;
		break;

	case vr::Prop_Axis1Type_Int32:
		nRetVal = vr::k_eControllerAxis_Trigger;
		error = vr::TrackedProp_Success;
		break;

	case vr::Prop_Axis2Type_Int32:
	case vr::Prop_Axis3Type_Int32:
	case vr::Prop_Axis4Type_Int32:
		error = vr::TrackedProp_ValueNotProvidedByDevice;
		break;
	}

	*pError = error;
	return nRetVal;
}

uint64_t CSampleDeviceDriver::GetUint64TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
	uint64_t ulRetVal = 0;
	vr::ETrackedPropertyError error = vr::TrackedProp_ValueNotProvidedByDevice;

	switch (prop)
	{
	case vr::Prop_CurrentUniverseId_Uint64:
	case vr::Prop_PreviousUniverseId_Uint64:
		error = vr::TrackedProp_ValueNotProvidedByDevice;
		break;

	case vr::Prop_SupportedButtons_Uint64:
		ulRetVal =
			vr::ButtonMaskFromId(vr::k_EButton_System) |
			vr::ButtonMaskFromId(vr::k_EButton_Axis0) |
			vr::ButtonMaskFromId(vr::k_EButton_Axis1) |
			vr::ButtonMaskFromId(k_EButton_Button1) |
			vr::ButtonMaskFromId(k_EButton_Button2) |
			vr::ButtonMaskFromId(k_EButton_Button3) |
			vr::ButtonMaskFromId(k_EButton_Button4) |
			vr::ButtonMaskFromId(k_EButton_Bumper);
		error = vr::TrackedProp_Success;
		break;

	case vr::Prop_HardwareRevision_Uint64:
		ulRetVal = 1;
		error = vr::TrackedProp_Success;
		break;

	case vr::Prop_FirmwareVersion_Uint64:
		ulRetVal = 1;
		error = vr::TrackedProp_Success;
		break;

	}

	*pError = error;
	return ulRetVal;
}

vr::HmdMatrix34_t CSampleDeviceDriver::GetMatrix34TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
	return vr::HmdMatrix34_t();
}

uint32_t CSampleDeviceDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, char *pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError *pError)
{
	std::ostringstream ssRetVal;

	switch (prop)
	{
	case vr::Prop_SerialNumber_String:
		ssRetVal << "123456";
		break;

	case vr::Prop_RenderModelName_String:
		ssRetVal << "generic_hmd";
		break;

	case vr::Prop_ManufacturerName_String:
		ssRetVal << "Second Reality";
		break;

	case vr::Prop_ModelNumber_String:
		ssRetVal << "Virtual Controller Device";
		break;

	case vr::Prop_TrackingFirmwareVersion_String:
		ssRetVal << "cd.firmware_revision=" << 1;
		break;

	case vr::Prop_HardwareRevision_String:
		ssRetVal << "cd.hardware_revision=" << 1;
		break;
	}

	std::string sRetVal = ssRetVal.str();
	if (sRetVal.empty())
	{
		*pError = vr::TrackedProp_ValueNotProvidedByDevice;
		return 0;
	}
	else if (sRetVal.size() + 1 > unBufferSize)
	{
		*pError = vr::TrackedProp_BufferTooSmall;
		return sRetVal.size() + 1;  // caller needs to know how to size buffer
	}
	else
	{
		_snprintf(pchValue, unBufferSize, sRetVal.c_str());
		*pError = vr::TrackedProp_Success;
		return sRetVal.size() + 1;
	}
}

void CSampleDeviceDriver::RunFrame()
{
	// In a real driver, this should happen from some pose tracking thread.
	// The RunFrame interval is unspecified and can be very irregular if some other
	// driver blocks it for some periodic task.
	if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
	{
		m_pServerDriverHost->TrackedDevicePoseUpdated(m_unObjectId, GetPose());
	}
}

