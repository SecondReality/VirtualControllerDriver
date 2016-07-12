#include "Communication.h"
#include <openvr_driver.h>
#include "driverlog.h"
#include "CSampleDeviceDriver.h"

#include <windows.h>

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
		: m_bEnableNullDriver(true)
		, m_bInit(false)
	{
	}

	virtual EVRInitError Init(vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir);
	virtual void Cleanup();
	virtual bool BIsHmdPresent(const char *pchUserDriverConfigDir);
	virtual EVRInitError SetDisplayId(const char *pchDisplayId) { return VRInitError_None; } // Null doesn't care
	virtual HiddenAreaMesh_t GetHiddenAreaMesh(EVREye eEye);
	virtual uint32_t GetMCImage(uint32_t *pImgWidth, uint32_t *pImgHeight, uint32_t *pChannels, void *pDataBuffer, uint32_t unBufferLen) { return uint32_t(); }

private:
	vr::IClientDriverHost *m_pClientDriverHost;

	bool m_bEnableNullDriver;
	bool m_bInit;
};

CClientDriver_Sample g_clientDriverNull;

EVRInitError CClientDriver_Sample::Init(vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir)
{
	m_pClientDriverHost = pDriverHost;
	InitDriverLog(pDriverLog);

	if (!m_bInit)
	{
		if (m_pClientDriverHost)
		{
			IVRSettings *pSettings = m_pClientDriverHost->GetSettings(vr::IVRSettings_Version);

			if (!m_bEnableNullDriver && pSettings)
			{
				m_bEnableNullDriver = pSettings->GetBool(k_pch_Sample_Section, k_pch_Sample_EnableSampleDriver_Bool, false);
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

bool CClientDriver_Sample::BIsHmdPresent(const char *pchUserDriverConfigDir)
{
	return false;
}

HiddenAreaMesh_t CClientDriver_Sample::GetHiddenAreaMesh(EVREye eEye)
{
	return vr::HiddenAreaMesh_t();
}

//-----------------------------------------------------------------------------

class CServerDriver_Sample : public IServerTrackedDeviceProvider
{
public:
	CServerDriver_Sample()
		: cSampleDeviceDriver(NULL)
		, m_bEnableNullDriver(false),
		myHost(NULL)
	{
	}

	virtual EVRInitError Init(IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir);
	virtual void Cleanup();
	virtual uint32_t GetTrackedDeviceCount();
	virtual ITrackedDeviceServerDriver *GetTrackedDeviceDriver(uint32_t unWhich) override;
	virtual ITrackedDeviceServerDriver* FindTrackedDeviceDriver(const char *pchId) override;
	virtual void RunFrame();

	virtual bool ShouldBlockStandbyMode() { return false; }
	virtual void EnterStandby() {}
	virtual void LeaveStandby() {}

private:
	CSampleDeviceDriver *cSampleDeviceDriver;

	bool m_bEnableNullDriver;

	vr::IServerDriverHost * myHost;

	bool createdController = false;
	int runFrameCounter = 0;

	virtual const char * const * GetInterfaceVersions() override
	{
		return vr::k_InterfaceVersions;
	}
};

CServerDriver_Sample g_serverDriverNull;

EVRInitError CServerDriver_Sample::Init(IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir)
{
	DriverLog("Init called\n");
	myHost = pDriverHost;
	InitDriverLog(pDriverLog);

	IVRSettings *pSettings = pDriverHost ? pDriverHost->GetSettings(vr::IVRSettings_Version) : NULL;

	m_bEnableNullDriver = true;

	return VRInitError_None;
}

void CServerDriver_Sample::Cleanup()
{
	CleanupDriverLog();
}

uint32_t CServerDriver_Sample::GetTrackedDeviceCount()
{
	DriverLog("GetTrackedDeviceCount\n");

	if (cSampleDeviceDriver)
	{
		DriverLog("GetTrackedDeviceCount returned 1\n");
		return 1;
	}

	return 0;
}

ITrackedDeviceServerDriver *CServerDriver_Sample::GetTrackedDeviceDriver(uint32_t unWhich)
{
	DriverLog("GetTrackedDeviceDriver\n");
	return cSampleDeviceDriver;
}

ITrackedDeviceServerDriver* CServerDriver_Sample::FindTrackedDeviceDriver(const char *pchId)
{
	DriverLog("FindTrackedDeviceDriver\n");
	return cSampleDeviceDriver;
}

void CServerDriver_Sample::RunFrame()
{
	runFrameCounter++;

	// We wait a number of frames, so that physical devices can be turned on,
	// before the virtual controller is created.
	if (runFrameCounter > 2000 && !createdController)
	{
		createdController = true;

		DriverLog("Adding virtual device\n");

		if (!cSampleDeviceDriver)
		{
			cSampleDeviceDriver = new CSampleDeviceDriver(myHost);
			myHost->TrackedDeviceAdded("Virtual Controller");
		}
	}

	if (cSampleDeviceDriver)
	{
		cSampleDeviceDriver->RunFrame();
	}
}

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
	if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
	{
		return &g_serverDriverNull;
	}

	if (0 == strcmp(IClientTrackedDeviceProvider_Version, pInterfaceName))
	{
		return &g_clientDriverNull;
	}

	if (pReturnCode)
	{
		*pReturnCode = VRInitError_Init_InterfaceNotFound;
	}

	return NULL;
}

//-----------------------------------------------------------------------------

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD     fdwReason, _In_ LPVOID    lpvReserved)
{
	DriverLog("Main Called\n");

	return TRUE;
}