#include "precomp.hpp"
#include "Info.hpp"

#include "Parser/Cfg.hpp"
#include "lib/Utf8ToUtf16.hpp"

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

namespace Ets2 {
	const std::string Info::SAVE_FORMAT_VAR_NAME = "g_save_format";

	std::wstring Info::getDefaultDirectory(Game game, bool isSteam) {
		if (!isSteam)
		{
			wxStandardPaths standardPaths = wxStandardPaths::Get();
			return std::wstring(standardPaths.GetDocumentsDir().Append(game == Game::ATS ? L"\\American Truck Simulator" : L"\\Euro Truck Simulator 2"));
		}
		else
		{
			return L"C:\\Program Files (x86)\\Steam";
		}
	}

	Info::Info(Game game, const std::wstring directory, const std::wstring steamDirectory) :
		mGame(game),
		mDirectory(directory),
		mSteamDirectory(steamDirectory)
	{
		wxStopWatch initTime;
		mProfiles = ProfileList();

		wxFileName configFileName(mDirectory + L"\\config.cfg");
		wxDir profilesDir(mDirectory + L"\\profiles\\");
		wxDir steamProfilesDir(mSteamDirectory + L"\\userdata\\");
		mIsValid = configFileName.Exists() && profilesDir.IsOpened() && steamProfilesDir.IsOpened();
		if (!mIsValid) {
			return;
		}

		mConfigFileName = configFileName.GetFullPath();

		wxStopWatch parseTime;
		processSaveFormat();
		DEBUG_LOG(L"%s: parsed in %lld �s", configFileName.GetFullPath(), parseTime.TimeInMicro());

		// Get the profiles
		wxString profileDir;
		bool fileFound = profilesDir.GetFirst(&profileDir, L"*", wxDIR_DIRS);
		while (fileFound) {
			Profile * profile = new Profile((profilesDir.GetNameWithSep() + profileDir).ToStdWstring());
			if (profile->isValid() && profile->getGame() == mGame) {
				mProfiles.push_back(profile);
			} else {
				delete profile;
			}
			fileFound = profilesDir.GetNext(&profileDir);
		}
#if _DEBUG
		wxString steamPofileDir;
		bool steamProfileFound = steamProfilesDir.GetFirst(&steamPofileDir, L"*", wxDIR_DIRS);
		while (steamProfileFound) {
			if (steamPofileDir != "ac")
			{
				wxString remoteProfileDirPath = steamProfilesDir.GetNameWithSep() + steamPofileDir;
				if (mGame == Ets2::Game::ETS2)
				{
					remoteProfileDirPath += "\\227300";
				}
				else
				{
					remoteProfileDirPath += "\\270880";
				}
				DEBUG_LOG(remoteProfileDirPath);
				wxDir remoteProfileDir(remoteProfileDirPath + "\\remote\\profiles");
				DEBUG_LOG(remoteProfileDirPath);
				fileFound = remoteProfileDir.GetFirst(&profileDir, L"*", wxDIR_DIRS);
				while (fileFound) {
					
					Profile* profile = new Profile((remoteProfileDir.GetNameWithSep() + profileDir).ToStdWstring());
					if (profile->isValid() && profile->getGame() == mGame) {
						mProfiles.push_back(profile);
					}
					else {
						delete profile;
					}
					fileFound = remoteProfileDir.GetNext(&profileDir);
				}
			}
			steamProfileFound = steamProfilesDir.GetNext(&steamPofileDir);
		}
#endif
		mProfiles.sort();
		DEBUG_LOG(L"%s: initialized in %lld �s", mDirectory, initTime.TimeInMicro());
	}

	Info::Info(Info& info) {
		Info::Info(info.getGame(), info.getDirectory(), info.getSteamDirectory());
	}

	bool Info::isValid() {
		return mIsValid;
	}

	std::wstring Info::getDirectory() {
		return mDirectory;
	}

	std::wstring Info::getSteamDirectory() {
		return mSteamDirectory;
	}

	std::wstring Info::getConfigFileName() {
		return mConfigFileName;
	}

	Game Info::getGame() const {
		return mGame;
	}

	Info::SaveFormat Info::getSaveFormat() {
		return mSaveFormat;
	}

	void Info::changeSaveFormat(SaveFormat newFormat) {
		if (mIsValid) {
			processSaveFormat(newFormat);
		}
	}

	std::wstring Info::getRawSaveFormat() {
		return mRawSaveFormat;
	}

	const ProfileList& Info::getProfiles() const {
		return mProfiles;
	}

	void Info::processSaveFormat(SaveFormat newFormat) {
		mRawSaveFormat = std::wstring(L"");
		std::string configFileContents;
		wxStopWatch pt;
		::read_file(mConfigFileName, configFileContents);
		std::string newConfigFile;
		std::string currentLine;
		mSaveFormat = SaveFormat::NOT_FOUND;
		std::string newFormatLine;
		if (newFormat != SaveFormat::INVALID) {
			newFormatLine.append("uset ");
			newFormatLine.append(SAVE_FORMAT_VAR_NAME);
			newFormatLine.append(" \"");
			newFormatLine.append(std::to_string(static_cast<int>(newFormat)));
			newFormatLine.append("\"");
		}
		bool formatLineAppended = false;
		Parser::Cfg::parse(configFileContents, [this, &newFormat, &newConfigFile, &newFormatLine, &formatLineAppended](const std::string& line, const std::string& name, const std::string& value) {
			bool lineChanged = false;
			if (name == SAVE_FORMAT_VAR_NAME) {
				if (newFormat == SaveFormat::INVALID) {
					Utf8ToUtf16(value.data(), value.length(), mRawSaveFormat);
					long saveFormat;
					try {
						saveFormat = std::stol(mRawSaveFormat);
					} catch (std::invalid_argument) {
						saveFormat = -1;
					}
					switch (saveFormat) {
					case 0: case 1:
						mSaveFormat = SaveFormat::BINARY;
						break;
					case 2:
						mSaveFormat = SaveFormat::TEXT;
						break;
					case 3:
						mSaveFormat = SaveFormat::BOTH;
						break;
					default:
						mSaveFormat = SaveFormat::INVALID;
					}
				} else {
					newConfigFile.append(newFormatLine);
					newConfigFile.append("\r\n");
					formatLineAppended = true;
					lineChanged = true;
				}
			}
			if (newFormat != SaveFormat::INVALID && !lineChanged) {
				newConfigFile.append(line);
				newConfigFile.append("\r\n");
			}
		});

		if (newFormat != SaveFormat::INVALID) {
			if (!formatLineAppended) {
				newConfigFile.append(newFormatLine);
				newConfigFile.append("\r\n");
			}
			try {
				write_file(newConfigFile, mConfigFileName);
			} catch(int) {
				// if there was an error, check again to see if the change was saved
				processSaveFormat();
				if (mSaveFormat != newFormat) {
					throw(SaveFormatChangeFailed());
				}
			}
		}
	}
}