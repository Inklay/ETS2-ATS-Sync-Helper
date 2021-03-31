#pragma once

#include "Profile.hpp"
#include "lib/file.hpp"
#include <string>
#include <regex>

namespace Ets2 {
	enum class Game : unsigned int {
		ETS2,
		ATS
	};

	// Information from a ETS2 Settings directory
	class Info {
	public:
		class SaveFormatChangeFailed : public std::exception {
			const char * what() const throw() { return "Save Format change failed."; }
		};

		// Returns the default ETS2 Settings directory for the current user
		// i.e. the current user's Documents\\Euro Truck Simulator 2
		enum class SaveFormat : int {
			BINARY = 0,
			TEXT = 2,
			BOTH = 3,
			INVALID = -1,
			NOT_FOUND = -2
		};

		static std::wstring getDefaultDirectory(Game game, bool isSteam = false);

		Info(Game game, const std::wstring directory, const std::wstring steamDirectory);
		Info(Info& info);

		bool isValid();
		std::wstring getDirectory();
		std::wstring getSteamDirectory();
		std::wstring getConfigFileName();
		Game getGame() const;
		SaveFormat getSaveFormat();
		void changeSaveFormat(SaveFormat newFormat);
		std::wstring getRawSaveFormat();
		const ProfileList& getProfiles() const;

	private:
		static const std::string SAVE_FORMAT_VAR_NAME;

		bool mIsValid;
		std::wstring mDirectory;
		std::wstring mSteamDirectory;
		std::wstring mConfigFileName;
		Game mGame;
		ProfileList mProfiles;
		std::wstring mRawSaveFormat;
		SaveFormat mSaveFormat;

		// If called with SAVE_FORMAT_INVALID, reads the save format and
		// sets mSaveFormat and mSaveFormatLine, otherwise changes the
		// save format in the file.
		void processSaveFormat(SaveFormat newFormat = SaveFormat::INVALID);
	};
}