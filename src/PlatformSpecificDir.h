#pragma once

#include <nap/resource.h>

namespace nap
{

	/**
	 * Resource representing a platform-specific directory.
	 */
	class NAPAPI PlatformSpecificDir : public Resource
	{
		RTTI_ENABLE(Resource)

	public:
		// Inherited from Resource
		bool init(utility::ErrorState& error) override;

		/**
		 * @return The path for the current platform, with mSubDir appended.
		 */
		std::string get() const;

		std::string mMacOSDir = "~/Library/Application Support/"; ///< Property: 'MacOSDir' The application support folder on MacOS
		std::string mWindowsDir = "C:/Users/<username>/AppData/"; ///< Property: 'WindowsDir' The application data folder on Windows
		std::string mLinuxDir = "~/.local/share/"; ///< Property: 'LinuxDir' The application data folder on Linux
		std::string mSubDir; ///< Property: 'SubDir' Subdirectory appended to the platform-specific directory

	private:
		std::string mDir;
	};


}
