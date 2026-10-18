#include "PlatformSpecificDir.h"

#include <utility/fileutils.h>

RTTI_BEGIN_CLASS(nap::PlatformSpecificDir)
	RTTI_PROPERTY("MacOSDir", &nap::PlatformSpecificDir::mMacOSDir, nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("WindowsDir", &nap::PlatformSpecificDir::mWindowsDir, nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("LinuxDir", &nap::PlatformSpecificDir::mLinuxDir, nap::rtti::EPropertyMetaData::Default)
	RTTI_PROPERTY("SubDir", &nap::PlatformSpecificDir::mSubDir, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

namespace nap
{

	bool PlatformSpecificDir::init(utility::ErrorState& error)
	{
		const char* baseDir = nullptr;

#if defined(__APPLE__)
		baseDir = mMacOSDir.c_str();
#elif defined(_WIN32)
		baseDir = mWindowsDir.c_str();
#else
		baseDir = mLinuxDir.c_str();
#endif

		std::string path(baseDir);

		// Expand leading ~ to the home directory
		if (!path.empty() && path[0] == '~')
		{
			if (const char* home = getenv("HOME"))
				path.replace(0, 1, home);
		}
		else if (baseDir == mWindowsDir.c_str())
		{
			// Replace <username> placeholder with the current username
			if (const char* username = getenv("USERNAME"))
				path.replace(path.find("<username>"), strlen("<username>"), username);
		}

		if (!utility::dirExists(path))
		{
			error.fail("Platform path does not exist: %s", path.c_str());
			return false;
		}

		if (mSubDir.empty())
			mDir = path;
		else
		{
			mDir =  path + "/" + mSubDir;
			if (!utility::dirExists(mDir))
				if (!utility::makeDirs(mDir))
				{
					error.fail("Failed to create platform specific path %s", mDir.c_str());
					return false;
				}
		}

		return true;
	}


	std::string PlatformSpecificDir::get() const
	{
		return mDir;
	}


}
