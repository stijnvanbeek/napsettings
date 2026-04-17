#pragma once

#include <utility/dllexport.h>
#include <rtti/object.h>
#include <nap/resource.h>
#include <rapidjson/document.h>

namespace nap
{

	/**
	 * Base class for all Setting objects
	 * The inherited mID property indicates the name of the setting in the json settings file.
	 */
	class SettingBase : public rtti::Object
	{
		RTTI_ENABLE(rtti::Object)

	public:
		SettingBase() = default;
	};


	/**
	 * Typed Setting object, for primitive setting types
	 * @tparam T Primitive data type of the setting
	 */
	template <typename T>
	class NAPAPI Setting : public SettingBase
	{
		RTTI_ENABLE(SettingBase)

	public:
		Setting() = default;

		T mValue; ///< Property: 'Value' Value of the setting
	};


	/**
	 * Container setting object holding any number of other settings of any type
	 */
	class NAPAPI SettingGroup : public SettingBase
	{
		RTTI_ENABLE(SettingBase)

	public:
		/**
		 * Find a datatype setting within the container
		 * @tparam T The datatype of the setting to find
		 * @param name The name of the setting to find
		 * @return Pointer to the found setting, nullptr if no setting with this name and type was found.
		 */
		template <typename T>
		Setting<T>* find(const std::string& name);

		/**
		 * Sets the value of a specified setting. If the setting does not exist yet, it will be created.
		 * @tparam T Primitive datatype of the setting
		 * @param name Name of the setting
		 * @param value New value for the setting
		 */
		template <typename T>
		void set(const std::string& name, const T& value);

        /**
         * Tries to find a setting. If it doesn't exist it will be created and given a default value.
         * @tparam T Primitive datatype of the setting
         * @param name Name of the setting
         * @defaultValue Default value of the new setting when it is created.
         * @return
         */
        template <typename T>
        Setting<T>* getOrCreate(const std::string& name, const T& defaultValue);

		/**
		 * Tries to find an existing SettingGroup within this group.
		 * If the group does not exist it will be created.
		 * @param name Name of the group
		 * @return Pointer to the requested group
		 */
		SettingGroup* getOrCreateGroup(const std::string& name);

		/**
		 * @return Number of settings in the group
		 */
		int getSize() const { return mSettings.size(); }

		/**
		 * Tries to retrieve setting from the group
		 * @tparam T Datatype of the setting
		 * @param name Name of the setting
		 * @return Pointer to the setting if found with specified type, otherwise nullptr.
		 */
		template <typename T>
		Setting<T>* get(const std::string& name)
		{
			auto it = std::find_if(mSettings.begin(), mSettings.end(), [&](auto& e){ return e->mID == name; });
			if (it == mSettings.end())
				return nullptr;
			return rtti_cast<Setting<T>>(it->get());
		}

		/**
		 * Read the group from a rapidjson object tree
		 * @param object Rapidjson object tree. Can be the document root.
		 */
		void readFromJson(rapidjson::Value& object);

		/**
		 * Write the group to a rapidjson object tree
		 * @param object Empty rapidjson object that will contain the settings within this group. Can also be the rapidjson document root.
		 * @param allocator
		 */
		void writeToJson(rapidjson::Value& object, rapidjson::Document::AllocatorType& allocator);

	private:
		std::vector<std::unique_ptr<SettingBase>> mSettings;
	};


	// Setting type definitions
	using SettingFloat = Setting<float>;
	using SettingInt = Setting<int>;
	using SettingBool = Setting<bool>;
	using SettingString = Setting<std::string>;

	// Use this macro to rtti define Setting types
	#define RTTI_DEFINE_SETTING(Type) \
		RTTI_BEGIN_CLASS(Type) \
    		RTTI_PROPERTY("Value", &Type::mValue, nap::rtti::EPropertyMetaData::Required) \
		RTTI_END_CLASS


	template <typename T>
	Setting<T>* SettingGroup::find(const std::string& name)
	{
		auto it = std::find_if(mSettings.begin(), mSettings.end(), [&](auto& setting){ return setting->mID == name;  });
		if (it != mSettings.end())
			return rtti_cast<Setting<T>>(it->get());
		else
			return nullptr;
	}


    template <typename T>
    Setting<T>* SettingGroup::getOrCreate(const std::string& name, const T& defaultValue)
    {
        auto setting = find<T>(name);
        if (setting == nullptr)
        {
            std::unique_ptr<Setting<T>> newSetting = std::make_unique<Setting<T>>();
            newSetting->mID = name;
            newSetting->mValue = defaultValue;
            auto result = newSetting.get();
            mSettings.emplace_back(std::move(newSetting));
            return result;
        }
        return setting;
    }


    template <typename T>
	void SettingGroup::set(const std::string& name, const T& value)
	{
		auto setting = find<T>(name);
		if (setting == nullptr)
		{
			std::unique_ptr<Setting<T>> newSetting = std::make_unique<Setting<T>>();
			newSetting->mID = name;
			newSetting->mValue = value;
			mSettings.emplace_back(std::move(newSetting));
		}
		else
			setting->mValue = value;
	}


}
