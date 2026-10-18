#include "SettingsFile.h"

#include <nap/core.h>
#include <rtti/jsonreader.h>
#include <rtti/jsonwriter.h>
#include <nap/logger.h>
#include <utility/fileutils.h>

RTTI_BEGIN_CLASS(nap::SettingsFile)
	RTTI_PROPERTY("FileName", &nap::SettingsFile::mFileName, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("Directory", &nap::SettingsFile::mDirectory, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

namespace nap
{

	std::string SettingsFile::getPath() const
	{
		if (mDirectory != nullptr)
			return utility::joinPath({ mDirectory->get(), mFileName });

		return mFileName;
	}


	bool SettingsFile::init(utility::ErrorState& errorState)
	{
		std::string path = mFileName;
		if (mDirectory != nullptr)
		{
			auto dir = mDirectory->get();
			path = utility::joinPath({ dir, mFileName });
		}

		// No settings file present yet, ignore
		if (!utility::fileExists(path))
		{
			path = mFileName;
			if (!utility::fileExists(path))
				return true;
		}

		// Read file to string
		std::string json;
		if (!utility::readFileToString(path, json, errorState))
		{
			errorState.fail("Failed to read settings file %s", path.c_str());
			return false;
		}

		// Parse json
		rapidjson::Document jsonDocument;
		jsonDocument.SetObject();
		jsonDocument.Parse(json.c_str());
		if (!errorState.check(!jsonDocument.HasParseError(), "Failed to parse settings file %s", path.c_str()))
			return false;

		// Create settings tree
		mRoot.readFromJson(jsonDocument);

		return true;
	}


	void SettingsFile::onDestroy()
	{
		save();
	}


	void SettingsFile::save()
	{
		// Write settings tree to json document
		rapidjson::Document jsonDocument;
		jsonDocument.SetObject();
		mRoot.writeToJson(jsonDocument, jsonDocument.GetAllocator());

		// Write json document to string
		rapidjson::StringBuffer stringBuffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);
		jsonDocument.Accept(writer);

		// Write the json string to a file
		utility::writeStringToFile(getPath(), stringBuffer.GetString());
	}

}

