#pragma once

#include <Settings.h>
#include <PlatformSpecificDir.h>
#include <nap/resourceptr.h>

namespace nap
{

	// Forward declarations
	class Core;

	/**
	 * Resource for a json file containing settings of primitive data types.
	 * Supported types are int, float, bool, string and groups.
	 * The settings are read from a json file on init and saved on destruction.
	 */
	class NAPAPI SettingsFile : public Resource
	{
		RTTI_ENABLE(Resource)

	public:
		SettingsFile() = default;

		// Inherited from Resource
		bool init(utility::ErrorState& error) override;
		void onDestroy() override;

		/**
		 * @return The full path of the settings file, computed from mFileName and mPath.
		 */
		std::string getPath() const;

		/**
		 * Save the settings to a json file at the path returned by getPath().
		 */
		void save();

		/**
		 * @return The root group containing all settings.
		 */
		SettingGroup& getRoot() { return mRoot; }

		std::string mFileName; ///< Property: 'FileName' Filename of the settings file
		ResourcePtr<PlatformSpecificDir> mDirectory = nullptr; ///< Property: 'Path' Link to a PlatformSpecificDir resource that determines where the settings file is saved and reloaded

	private:
		SettingGroup mRoot;
	};


}

