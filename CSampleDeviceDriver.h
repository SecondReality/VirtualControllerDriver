#pragma once

#include <openvr_driver.h>
#include <string>


class CSampleDeviceDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRControllerComponent
{
public:

	static const vr::EVRButtonId k_EButton_Button1 = (vr::EVRButtonId) 7;
	static const vr::EVRButtonId k_EButton_Button2 = (vr::EVRButtonId) 8;
	static const vr::EVRButtonId k_EButton_Button3 = (vr::EVRButtonId) 9;
	static const vr::EVRButtonId k_EButton_Button4 = vr::k_EButton_ApplicationMenu;
	static const vr::EVRButtonId k_EButton_Bumper = vr::k_EButton_Grip; // Just for demo compatibility

	CSampleDeviceDriver(vr::IServerDriverHost *pDriverHost);

	virtual ~CSampleDeviceDriver();

	virtual vr::EVRInitError Activate(uint32_t unObjectId);

	virtual void Deactivate();

	void *GetComponent(const char *pchComponentNameAndVersion);

	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);

	virtual vr::VRControllerState_t GetControllerState();

	virtual bool TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds);

	virtual void PowerOff();

	virtual vr::DriverPose_t GetPose();

	// TODO: For some reason this function is called repeatedly over and over again with value 1025
	virtual bool GetBoolTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError);

	virtual float GetFloatTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError);

	int32_t GetInt32TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError);

	virtual uint64_t GetUint64TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError);

	vr::HmdMatrix34_t GetMatrix34TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError);

	virtual uint32_t GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, char *pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError *pError);

	void RunFrame();

private:
	vr::IServerDriverHost *m_pServerDriverHost;
	uint32_t m_unObjectId;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	int32_t m_nWindowX;
	int32_t m_nWindowY;
	int32_t m_nWindowWidth;
	int32_t m_nWindowHeight;
	int32_t m_nRenderWidth;
	int32_t m_nRenderHeight;
	float m_flSecondsFromVsyncToPhotons;
	float m_flDisplayFrequency;
	float m_flIPD;
};
