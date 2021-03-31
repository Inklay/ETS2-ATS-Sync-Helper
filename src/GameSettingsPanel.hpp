#pragma once

#include "Ets2StaticBox.hpp"
#include "StatusText.hpp"
#include <wx/hyperlink.h>
#include "precomp.hpp"

wxDECLARE_EVENT(EVT_CONFIG_DIR_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_STEAM_DIR_CHANGED, wxCommandEvent);

class GameSettingsPanel : public Ets2StaticBox {
public:
	GameSettingsPanel(wxWindow * parent, wxWindowID id);
	~GameSettingsPanel();

	enum MenuId : int {
		MENU_ID_DIR_OPEN = wxID_HIGHEST + 1,
		MENU_ID_DIR_OPEN_FILE,
		MENU_ID_DIR_CHANGE,
		MENU_ID_DIR_CHECK_AGAIN,
		MENU_ID_DIR_DEFAULT
	};

	enum DirId : int {
		CONFIG,
		STEAM
	};

	void updateFromEts2Info() override;
	wxString getDirectory(DirId dirId = DirId::CONFIG);
	Ets2::Game getGame();
	void setGame(Ets2::Game game, bool sendEvend = true);

private:
	Ets2::Game mGame;
	wxRadioButton * mGameEts2;
	wxRadioButton * mGameAts;
	wxBoxSizer * mConfigDirSizer;
	wxStaticText * mConfigDirText;
	wxStaticText * mConfigDirSeparator;
	wxHyperlinkCtrl * mConfigDirOptionsButton;
	wxMenu * mConfigDirOptionsMenu;
	wxMenuItem * mConfigDirOptionOpen;
	wxMenuItem * mConfigDirOptionOpenFile;
	wxMenuItem * mConfigDirOptionDefault;
	wxBoxSizer * mSteamDirSizer;
	wxStaticText * mSteamDirText;
	wxStaticText * mSteamDirSeparator;
	wxHyperlinkCtrl * mSteamDirOptionsButton;
	wxMenu * mSteamDirOptionsMenu;
	wxMenuItem * mSteamDirOptionOpen;
	wxMenuItem * mSteamDirOptionDefault;
	wxBoxSizer * mSaveFormatSizer;
	wxStaticText * mSaveFormatText;
	wxStaticText * mSaveFormatSeparator;
	wxHyperlinkCtrl * mFixSaveFormatButton;
	StatusText * mStatusText;

	void onDirOptions(DirId dirId);
	wxString onDirOptionsSelected(wxCommandEvent& event, DirId dirId, wxString text, wxString default, wxString label, wxString baseText);
	void onConfigDirOptionSelected(wxCommandEvent& event);
	void onSteamDirOptionSelected(wxCommandEvent& event);
	void setDirText(const wxString& text, DirId dirId);
};