#include "Settings.h"

#include <utility/fileutils.h>

RTTI_BEGIN_CLASS(nap::SettingBase)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::SettingGroup)
RTTI_END_CLASS

RTTI_DEFINE_SETTING(nap::SettingFloat)
RTTI_DEFINE_SETTING(nap::SettingInt)
RTTI_DEFINE_SETTING(nap::SettingBool)
RTTI_DEFINE_SETTING(nap::SettingString)

namespace nap
{

	SettingGroup* SettingGroup::getOrCreateGroup(const std::string& name)
	{
		auto it = mSettings.find(name);
		SettingGroup* result = nullptr;
		if (it != mSettings.end())
			result = rtti_cast<SettingGroup>(it->second.get());

		if (result == nullptr)
		{
			auto newGroup = std::make_unique<SettingGroup>();
			result = newGroup.get();
			mSettings[name] = std::move(newGroup);
		}

		return result;
	}


	void SettingGroup::remove(const std::string &name)
	{
		mSettings.erase(name);
	}


	void SettingGroup::readFromJson(rapidjson::Value& object)
	{
		for (auto it = object.MemberBegin(); it != object.MemberEnd(); ++it)
		{
			auto& value = it->value;
			if (value.IsObject())
			{
				auto newSetting = std::make_unique<SettingGroup>();
				newSetting->readFromJson(value);
				mSettings[it->name.GetString()] = std::move(newSetting);
			}
			else if (value.IsFloat())
			{
				auto newSetting = std::make_unique<SettingFloat>();
				newSetting->mValue = value.GetFloat();
				mSettings[it->name.GetString()] = std::move(newSetting);
			}
			else if (value.IsInt())
			{
				auto newSetting = std::make_unique<SettingInt>();
				newSetting->mValue = value.GetInt();
				mSettings[it->name.GetString()] = std::move(newSetting);
			}
			else if (value.IsBool())
			{
				auto newSetting = std::make_unique<SettingBool>();
				newSetting->mValue = value.GetBool();
				mSettings[it->name.GetString()] = std::move(newSetting);
			}
			else if (value.IsString())
			{
				auto newSetting = std::make_unique<SettingString>();
				newSetting->mValue = value.GetString();
				mSettings[it->name.GetString()] = std::move(newSetting);
			}
		}
	}


	void SettingGroup::writeToJson(rapidjson::Value& object, rapidjson::Document::AllocatorType& allocator)
	{
		for (auto& [name, setting] : mSettings)
		{
			if (setting->get_type().is_derived_from<SettingGroup>())
			{
				rapidjson::Value child(rapidjson::kObjectType);
				rtti_cast<SettingGroup>(setting.get())->writeToJson(child, allocator);
				object.AddMember(rapidjson::StringRef(name.c_str()), child, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingFloat>())
			{
				rapidjson::Value value(rtti_cast<SettingFloat>(setting.get())->mValue);
				object.AddMember(rapidjson::StringRef(name.c_str()), value, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingInt>())
			{
				rapidjson::Value value(rtti_cast<SettingInt>(setting.get())->mValue);
				object.AddMember(rapidjson::StringRef(name.c_str()), value, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingBool>())
			{
				rapidjson::Value value(rtti_cast<SettingBool>(setting.get())->mValue);
				object.AddMember(rapidjson::StringRef(name.c_str()), value, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingString>())
			{
				rapidjson::Value value;
				auto& str = rtti_cast<SettingString>(setting.get())->mValue;
				value.SetString(str.c_str(), str.size(), allocator);
				object.AddMember(rapidjson::StringRef(name.c_str()), value, allocator);
			}
		}
	}

}