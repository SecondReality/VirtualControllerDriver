#include <openvr_driver.h>
#include "driverlog.h"

#include <vector>
#include <sstream>

using namespace vr;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(GNUC) || defined(COMPILER_GCC)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

// keys for use with the settings API
static const char * const k_pch_Sample_Section = "driver_virtualcontroller";
static const char * const k_pch_Sample_EnableSampleDriver_Bool = "enable";
static const char * const k_pch_Sample_SerialNumber_String = "serialNumber";
static const char * const k_pch_Sample_ModelNumber_String = "modelNumber";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CClientDriver_Sample : public IClientTrackedDeviceProvider
{
public:
	CClientDriver_Sample()
		: m_bEnableNullDriver( true )
		, m_bInit( false )
	{
	}

	virtual EVRInitError Init( vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir ) ;
	virtual void Cleanup() ;
	virtual bool BIsHmdPresent( const char *pchUserDriverConfigDir ) ;
	virtual EVRInitError SetDisplayId( const char *pchDisplayId )  { return VRInitError_None; } // Null doesn't care
	virtual HiddenAreaMesh_t GetHiddenAreaMesh( EVREye eEye ) ;
	virtual uint32_t GetMCImage( uint32_t *pImgWidth, uint32_t *pImgHeight, uint32_t *pChannels, void *pDataBuffer, uint32_t unBufferLen )  { return uint32_t(); }

private:
	vr::IClientDriverHost *m_pClientDriverHost;

	bool m_bEnableNullDriver;
	bool m_bInit;
};

CClientDriver_Sample g_clientDriverNull;


EVRInitError CClientDriver_Sample::Init( vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir ) 
{
	m_pClientDriverHost = pDriverHost;
	InitDriverLog( pDriverLog );

	if ( !m_bInit )
	{

		if ( m_pClientDriverHost )
		{
			IVRSettings *pSettings = m_pClientDriverHost->GetSettings( vr::IVRSettings_Version );

			if ( !m_bEnableNullDriver && pSettings )
			{
				m_bEnableNullDriver = pSettings->GetBool( k_pch_Sample_Section, k_pch_Sample_EnableSampleDriver_Bool, false );
			}
		}
		m_bInit = true;
	}

	return VRInitError_None;
}


void CClientDriver_Sample::Cleanup() 
{
	CleanupDriverLog();
}


bool CClientDriver_Sample::BIsHmdPresent( const char *pchUserDriverConfigDir ) 
{
	return false;
}

HiddenAreaMesh_t CClientDriver_Sample::GetHiddenAreaMesh( EVREye eEye )
{
	return vr::HiddenAreaMesh_t();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CSampleDeviceDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRControllerComponent
{
public:

	static const vr::EVRButtonId k_EButton_Button1 = (vr::EVRButtonId) 7;
	static const vr::EVRButtonId k_EButton_Button2 = (vr::EVRButtonId) 8;
	static const vr::EVRButtonId k_EButton_Button3 = (vr::EVRButtonId) 9;
	static const vr::EVRButtonId k_EButton_Button4 = vr::k_EButton_ApplicationMenu;
	static const vr::EVRButtonId k_EButton_Bumper = vr::k_EButton_Grip; // Just for demo compatibility

	CSampleDeviceDriver( vr::IServerDriverHost *pDriverHost )
		: m_pServerDriverHost( pDriverHost )
		, m_unObjectId( vr::k_unTrackedDeviceIndexInvalid )
	{
	}

	virtual ~CSampleDeviceDriver()
	{
		m_pServerDriverHost = NULL;
	}
	
	virtual EVRInitError Activate( uint32_t unObjectId ) 
	{
		m_unObjectId = unObjectId;
		return VRInitError_None;
	}

	virtual void Deactivate() 
	{
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	}

	void *GetComponent( const char *pchComponentNameAndVersion )
	{
		if ( !_stricmp( pchComponentNameAndVersion, vr::IVRDisplayComponent_Version ) )
		{
			return (vr::IVRDisplayComponent*)this;
		}

		return NULL;
	}

	virtual void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) 
	{
		if( unResponseBufferSize >= 1 )
			pchResponseBuffer[0] = 0;
	}

	virtual VRControllerState_t GetControllerState()
	{
		return vr::VRControllerState_t();
	}

	virtual bool TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds)
	{
		return false;
	}

	virtual void PowerOff()
	{
	}

	virtual DriverPose_t GetPose() 
	{
		DriverPose_t pose = { 0 };
		pose.poseIsValid = true;
		pose.result = TrackingResult_Running_OK;
		pose.deviceIsConnected = true;

		pose.qWorldFromDriverRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
		pose.qDriverFromHeadRotation = HmdQuaternion_Init( 1, 0, 0, 0 );

		pose.poseTimeOffset = 0.016f;

		pose.vecPosition[0] = 0;
		pose.vecPosition[1] = 0;
		pose.vecPosition[2] = 0;
		
		pose.qRotation = HmdQuaternion_Init(1, 0, 0, 0);
		
		return pose;
	}
	
	// TODO: For some reason this function is called repeatedly over and over again with value 1025
	virtual bool GetBoolTrackedDeviceProperty( vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError ) 
	{
		*pError = vr::TrackedProp_Success;
		return false;
	}

	virtual float GetFloatTrackedDeviceProperty( vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError ) 
	{
		*pError = vr::TrackedProp_ValueNotProvidedByDevice;
		return 0.0f;
	}

	int32_t GetInt32TrackedDeviceProperty( vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError )
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

	virtual uint64_t GetUint64TrackedDeviceProperty( vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError ) 
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

	vr::HmdMatrix34_t GetMatrix34TrackedDeviceProperty( vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError ) 
	{
		return vr::HmdMatrix34_t();
	}

	virtual uint32_t GetStringTrackedDeviceProperty( vr::ETrackedDeviceProperty prop, char *pchValue, uint32_t unBufferSize, vr::ETrackedPropertyError *pError ) 
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

	void RunFrame()
	{
		// In a real driver, this should happen from some pose tracking thread.
		// The RunFrame interval is unspecified and can be very irregular if some other
		// driver blocks it for some periodic task.
		if ( m_unObjectId != vr::k_unTrackedDeviceIndexInvalid )
		{
			m_pServerDriverHost->TrackedDevicePoseUpdated( m_unObjectId, GetPose() );
		}
	}

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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CServerDriver_Sample: public IServerTrackedDeviceProvider
{
public:
	CServerDriver_Sample()
		: m_pNullHmdLatest( NULL )
		, m_bEnableNullDriver( false ),
		myHost ( NULL)
	{
	}

	virtual EVRInitError Init( IDriverLog *pDriverLog, 	vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir ) ;
	virtual void Cleanup() ;
	virtual uint32_t GetTrackedDeviceCount() ;
	virtual ITrackedDeviceServerDriver *GetTrackedDeviceDriver( uint32_t unWhich, const char *pchInterfaceVersion ) ;
	virtual ITrackedDeviceServerDriver* FindTrackedDeviceDriver( const char *pchId, const char *pchInterfaceVersion ) ;
	virtual void RunFrame() ;
	virtual bool ShouldBlockStandbyMode()  { return false; }
	virtual void EnterStandby()  {}
	virtual void LeaveStandby()  {}

private:
	CSampleDeviceDriver *m_pNullHmdLatest;
	
	bool m_bEnableNullDriver;

	vr::IServerDriverHost * myHost;

	bool createdController;
	int frameCount;
};

CServerDriver_Sample g_serverDriverNull;

EVRInitError CServerDriver_Sample::Init( IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir ) 
{
	myHost = pDriverHost;
	InitDriverLog( pDriverLog );

	IVRSettings *pSettings = pDriverHost ? pDriverHost->GetSettings( vr::IVRSettings_Version ) : NULL;

	m_bEnableNullDriver = true;

	m_pNullHmdLatest = new CSampleDeviceDriver( pDriverHost);
	
	return VRInitError_None;
}

void CServerDriver_Sample::Cleanup() 
{
	CleanupDriverLog();
}

uint32_t CServerDriver_Sample::GetTrackedDeviceCount()
{
	// TODO: Make this return the correct number (however doesn't seem to cause issues currently)
	return 0;
}

ITrackedDeviceServerDriver *CServerDriver_Sample::GetTrackedDeviceDriver( uint32_t unWhich, const char *pchInterfaceVersion )
{
	// don't return anything if that's not the interface version we have
	if (0 != _stricmp(pchInterfaceVersion, ITrackedDeviceServerDriver_Version))
	{
		return NULL;
	}
		
	return m_pNullHmdLatest;
}

ITrackedDeviceServerDriver* CServerDriver_Sample::FindTrackedDeviceDriver( const char *pchId, const char *pchInterfaceVersion )
{
	// don't return anything if that's not the interface version we have
	if (0 != _stricmp(pchInterfaceVersion, ITrackedDeviceServerDriver_Version))
	{
		return NULL;
	}

	return m_pNullHmdLatest;
}

void CServerDriver_Sample::RunFrame()
{
	// Used to be in init:
	frameCount++;

	if (frameCount > 1000 && !createdController)
	{
		createdController = true;
		myHost->TrackedDeviceAdded("Virtual Controller");
	}

	if (m_pNullHmdLatest)
	{
		m_pNullHmdLatest->RunFrame();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HMD_DLL_EXPORT void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return &g_serverDriverNull;
	}
	
	
	if( 0 == strcmp( IClientTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return &g_clientDriverNull;
	}
	
	if(pReturnCode)
	{
		*pReturnCode = VRInitError_Init_InterfaceNotFound;
	}

	return NULL;
}