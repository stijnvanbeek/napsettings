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
		auto it = std::find_if(mSettings.begin(), mSettings.end(), [&, name](auto& setting){
		  return setting->mID == name;
		});
		SettingGroup* result = nullptr;
		if (it != mSettings.end())
			result = rtti_cast<SettingGroup>(it->get());

		if (result == nullptr)
		{
			auto newGroup = std::make_unique<SettingGroup>();
			newGroup->mID = name;
			result = newGroup.get();
			mSettings.emplace_back(std::move(newGroup));
		}

		return result;
	}


	void SettingGroup::readFromJson(rapidjson::Value& object)
	{
		for (auto it = object.MemberBegin(); it != object.MemberEnd(); ++it)
		{
			auto& value = it->value;
			if (value.IsObject())
			{
				auto newSetting = std::make_unique<SettingGroup>();
				newSetting->mID = it->name.GetString();
				newSetting->readFromJson(value);
				mSettings.emplace_back(std::move(newSetting));
			}
			else if (value.IsFloat())
			{
				auto newSetting = std::make_unique<SettingFloat>();
				newSetting->mID = it->name.GetString();
				newSetting->mValue = value.GetFloat();
				mSettings.emplace_back(std::move(newSetting));
			}
			else if (value.IsInt())
			{
				auto newSetting = std::make_unique<SettingInt>();
				newSetting->mID = it->name.GetString();
				newSetting->mValue = value.GetInt();
				mSettings.emplace_back(std::move(newSetting));
			}
			else if (value.IsBool())
			{
				auto newSetting = std::make_unique<SettingBool>();
				newSetting->mID = it->name.GetString();
				newSetting->mValue = value.GetBool();
				mSettings.emplace_back(std::move(newSetting));
			}
			else if (value.IsString())
			{
				auto newSetting = std::make_unique<SettingString>();
				newSetting->mID = it->name.GetString();
				newSetting->mValue = value.GetString();
				mSettings.emplace_back(std::move(newSetting));
			}
		}
	}


	void SettingGroup::writeToJson(rapidjson::Value& object, rapidjson::Document::AllocatorType& allocator)
	{
		for (auto& setting : mSettings)
		{
			if (setting->get_type().is_derived_from<SettingGroup>())
			{
				rapidjson::Value child(rapidjson::kObjectType);
				rtti_cast<SettingGroup>(setting.get())->writeToJson(child, allocator);
				object.AddMember(rapidjson::StringRef(setting->mID.c_str()), child, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingFloat>())
			{
				rapidjson::Value value(rtti_cast<SettingFloat>(setting.get())->mValue);
				object.AddMember(rapidjson::StringRef(setting->mID.c_str()), value, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingInt>())
			{
				rapidjson::Value value(rtti_cast<SettingInt>(setting.get())->mValue);
				object.AddMember(rapidjson::StringRef(setting->mID.c_str()), value, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingBool>())
			{
				rapidjson::Value value(rtti_cast<SettingBool>(setting.get())->mValue);
				object.AddMember(rapidjson::StringRef(setting->mID.c_str()), value, allocator);
			}
			else if (setting->get_type().is_derived_from<SettingString>())
			{
				rapidjson::Value value;
				auto& str = rtti_cast<SettingString>(setting.get())->mValue;
				value.SetString(str.c_str(), str.size(), allocator);
				object.AddMember(rapidjson::StringRef(setting->mID.c_str()), value, allocator);
			}
		}
	}

}