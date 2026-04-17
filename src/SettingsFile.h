#pragma once

#include <Settings.h>

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
		 * Save the settings to a json file at mPath
		 */
		void save();

		/**
		 * @return The root group containing all settings.
		 */
		SettingGroup& getRoot() { return mRoot; }

		std::string mPath; ///< Property: 'Path' Path to save the settings file to

	private:
		SettingGroup mRoot;
	};


}

