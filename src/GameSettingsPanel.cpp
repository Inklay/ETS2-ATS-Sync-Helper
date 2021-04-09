#include "precomp.hpp"

#include "GameSettingsPanel.hpp"
#include "ShowLastWindowsError.hpp"

#include <wx/filename.h>

wxDEFINE_EVENT(EVT_CONFIG_DIR_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_STEAM_DIR_CHANGED, wxCommandEvent);

GameSettingsPanel::GameSettingsPanel(wxWindow * parent, wxWindowID id)
	: Ets2StaticBox(parent, id, L"Game Settings")
{
	mConfigDirOptionsMenu = new wxMenu();
	mConfigDirOptionOpen = new wxMenuItem(mConfigDirOptionsMenu, MENU_ID_DIR_OPEN, L"View folder");
	mConfigDirOptionsMenu->Append(mConfigDirOptionOpen);
	mConfigDirOptionOpenFile = new wxMenuItem(mConfigDirOptionsMenu, MENU_ID_DIR_OPEN_FILE, L"Open game config file");
	mConfigDirOptionsMenu->Append(mConfigDirOptionOpenFile);
	mConfigDirOptionsMenu->AppendSeparator();
	mConfigDirOptionsMenu->Append(MENU_ID_DIR_CHANGE, L"Change…");
	mConfigDirOptionsMenu->Append(MENU_ID_DIR_CHECK_AGAIN, L"Check again");
	mConfigDirOptionDefault = new wxMenuItem(mConfigDirOptionsMenu, MENU_ID_DIR_DEFAULT, L"Set to default");
	mConfigDirOptionsMenu->Append(mConfigDirOptionDefault);
	mConfigDirOptionsMenu->Bind(wxEVT_MENU, &GameSettingsPanel::onConfigDirOptionSelected, this);

	mSteamDirOptionsMenu = new wxMenu();
	mSteamDirOptionOpen = new wxMenuItem(mSteamDirOptionsMenu, MENU_ID_DIR_OPEN, L"View folder");
	mSteamDirOptionsMenu->Append(mSteamDirOptionOpen);
	mSteamDirOptionsMenu->AppendSeparator();
	mSteamDirOptionsMenu->Append(MENU_ID_DIR_CHANGE, L"Change…");
	mSteamDirOptionsMenu->Append(MENU_ID_DIR_CHECK_AGAIN, L"Check again");
	mSteamDirOptionDefault = new wxMenuItem(mSteamDirOptionsMenu, MENU_ID_DIR_DEFAULT, L"Set to default");
	mSteamDirOptionsMenu->Append(mSteamDirOptionDefault);
	mSteamDirOptionsMenu->Bind(wxEVT_MENU, &GameSettingsPanel::onSteamDirOptionSelected, this);

	wxPoint spacer(wxDLG_UNIT(this, wxPoint(4, 4)));

	wxBoxSizer * contentSizer = getContentSizer();

	wxFlexGridSizer * mainSizer = new wxFlexGridSizer(2, spacer.y, spacer.x);
	mainSizer->AddGrowableCol(0, 0);
	mainSizer->AddGrowableCol(1, 1);
	contentSizer->Add(mainSizer, wxSizerFlags().Expand());

	// Game
	mainSizer->Add(new wxStaticText(this, wxID_ANY, L"Game:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), wxSizerFlags().CenterVertical().Expand());
	wxSizer * gameSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(gameSizer);
	mGameEts2 = new wxRadioButton(this, wxID_ANY, L"ETS2");
	mGameAts = new wxRadioButton(this, wxID_ANY, L"ATS");
	gameSizer->Add(mGameEts2);
	gameSizer->Add(mGameAts);

	mGameEts2->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { setGame(Ets2::Game::ETS2); });
	mGameAts->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { setGame(Ets2::Game::ATS); });

	// Settings Folder
	mainSizer->Add(new wxStaticText(this, wxID_ANY, L"Settings Folder:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), wxSizerFlags().CenterVertical().Expand());

	mConfigDirSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(mConfigDirSizer, wxSizerFlags().Expand());

	mConfigDirText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_MIDDLE);
	mConfigDirSeparator = new wxStaticText(this, wxID_ANY, L" - ");
	mConfigDirOptionsButton = new wxHyperlinkCtrl(this, wxID_ANY, L"Options", wxEmptyString);
	mConfigDirOptionsButton->SetMinSize(mConfigDirOptionsButton->GetClientSize());
	mConfigDirOptionsButton->Bind(wxEVT_HYPERLINK, [this](wxCommandEvent&) { onDirOptions(DirId::CONFIG); });

	mConfigDirSizer->Add(mConfigDirText, wxSizerFlags().CenterVertical());
	mConfigDirSizer->Add(mConfigDirSeparator, wxSizerFlags().CenterVertical());
	mConfigDirSizer->Add(mConfigDirOptionsButton, wxSizerFlags().CenterVertical());

	// Steam Folder
	mSteamDirSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, L"Steam Folder:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), wxSizerFlags().CenterVertical().Expand());
	mainSizer->Add(mSteamDirSizer, wxSizerFlags().Expand());

	mSteamDirText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_MIDDLE);
	mSteamDirSeparator = new wxStaticText(this, wxID_ANY, L" - ");
	mSteamDirOptionsButton = new wxHyperlinkCtrl(this, wxID_ANY, L"Options", wxEmptyString);
	mSteamDirOptionsButton->SetMinSize(mSteamDirOptionsButton->GetClientSize());
	mSteamDirOptionsButton->Bind(wxEVT_HYPERLINK, [this](wxCommandEvent&) { onDirOptions(DirId::STEAM); });

	mSteamDirSizer->Add(mSteamDirText, wxSizerFlags().CenterVertical());
	mSteamDirSizer->Add(mSteamDirSeparator, wxSizerFlags().CenterVertical());
	mSteamDirSizer->Add(mSteamDirOptionsButton, wxSizerFlags().CenterVertical());

	// Save Format
	mainSizer->Add(new wxStaticText(this, wxID_ANY, L"Save Format:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT), wxSizerFlags().CenterVertical().Expand());

	mSaveFormatSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(mSaveFormatSizer, wxSizerFlags().CenterVertical().Expand());

	mSaveFormatText = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_MIDDLE);
	mSaveFormatSeparator = new wxStaticText(this, wxID_ANY, L" - ");
	//mFixSaveFormatButton = new wxHyperlinkCtrl(this, wxID_ANY, L"Set to “Binary”", wxEmptyString);
	//mFixSaveFormatButton->Bind(wxEVT_HYPERLINK, [this](wxCommandEvent&) { getEts2Info()->changeSaveFormat(Ets2::Info::SaveFormat::BINARY); });
	mFixSaveFormatButton = new wxHyperlinkCtrl(this, wxID_ANY, L"Set to “Text”", wxEmptyString);
	mFixSaveFormatButton->Bind(wxEVT_HYPERLINK, [this](wxCommandEvent&) { getEts2Info()->changeSaveFormat(Ets2::Info::SaveFormat::TEXT); });

	mSaveFormatSizer->Add(mSaveFormatText, wxSizerFlags().CenterVertical());
	mSaveFormatSizer->Add(mSaveFormatSeparator, wxSizerFlags().CenterVertical());
	mSaveFormatSizer->Add(mFixSaveFormatButton, wxSizerFlags().CenterVertical());

	contentSizer->AddSpacer(spacer.y);

	mStatusText = new StatusText(this, wxID_ANY);
	contentSizer->Add(mStatusText);

	setGame(Ets2::Game::ETS2, false);
}

GameSettingsPanel::~GameSettingsPanel() {
	wxDELETE(mConfigDirOptionsMenu);
}

Ets2::Game GameSettingsPanel::getGame() {
	return mGame;
}

void GameSettingsPanel::setGame(Ets2::Game game, bool sendEvent) {
	mGame = game;
	if (game == Ets2::Game::ATS) {
		mGameAts->SetValue(true);
		mGameEts2->SetValue(false);
	} else {
		mGameAts->SetValue(false);
		mGameEts2->SetValue(true);
	}
	if (sendEvent) {
		setDirText(Ets2::Info::getDefaultDirectory(game), DirId::CONFIG);
		QueueEvent(new wxCommandEvent(EVT_CONFIG_DIR_CHANGED, GetId()));
	}
}

void GameSettingsPanel::onDirOptions(DirId dirId) {
	if (dirId == DirId::CONFIG)
	{
		wxPoint position = mConfigDirOptionsButton->GetPosition();
		PopupMenu(mConfigDirOptionsMenu, position.x, position.y + mConfigDirOptionsButton->GetSize().y);
	}
	else if (dirId == DirId::STEAM)
	{
		wxPoint position = mSteamDirOptionsButton->GetPosition();
		PopupMenu(mSteamDirOptionsMenu, position.x, position.y + mSteamDirOptionsButton->GetSize().y);
	}
}

wxString GameSettingsPanel::onDirOptionsSelected(wxCommandEvent& event, DirId dirId, wxString text, wxString default, wxString label, wxString baseText)
{
	wxString dir = baseText;
	switch (event.GetId()) {
		case MENU_ID_DIR_OPEN:
			wxLaunchDefaultApplication(getDirectory(dirId));
			break;
		case MENU_ID_DIR_OPEN_FILE:
			wxLaunchDefaultApplication(getEts2Info()->getConfigFileName());
			break;
		case MENU_ID_DIR_DEFAULT:
			dir = default;
			break;
		case MENU_ID_DIR_CHECK_AGAIN:
			dir = label;
			break;
		case MENU_ID_DIR_CHANGE:
			wxDirDialog dlg(this, text, getDirectory(dirId), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
			if (dlg.ShowModal() == wxID_OK) {
				dir = dlg.GetPath();
			}
			else
			{
				return L"";
			}
			break;
	}
	return dir;
}

void GameSettingsPanel::onConfigDirOptionSelected(wxCommandEvent& event)
{
	wxString dir = onDirOptionsSelected(event, DirId::CONFIG, L"Select the Game Settings folder", Ets2::Info::getDefaultDirectory(getGame()), mConfigDirText->GetLabel(), getDirectory(DirId::CONFIG));
	setDirText(dir, DirId::CONFIG);
	QueueEvent(new wxCommandEvent(EVT_CONFIG_DIR_CHANGED, GetId()));
}

void GameSettingsPanel::onSteamDirOptionSelected(wxCommandEvent& event)
{
	wxString dir = onDirOptionsSelected(event, DirId::STEAM, L"Select your Steam installation folder", Ets2::Info::getDefaultDirectory(getGame(), true), mSteamDirText->GetLabel(), getDirectory(DirId::STEAM));
	setDirText(dir, DirId::STEAM);
	QueueEvent(new wxCommandEvent(EVT_STEAM_DIR_CHANGED, GetId()));
}

void GameSettingsPanel::updateFromEts2Info() {
	Ets2::Info * ets2Info = getEts2Info();

	wxFileName dir = wxFileName::DirName(ets2Info->getDirectory());
	bool isDefaultDir = dir.SameAs(wxFileName::DirName(Ets2::Info::getDefaultDirectory(getGame())));
	mConfigDirOptionOpen->Enable(dir.Exists());
	mConfigDirOptionOpenFile->Enable(ets2Info->isValid());
	mConfigDirOptionDefault->Enable(!isDefaultDir);
	MenuId id = MENU_ID_DIR_OPEN;
	if (!ets2Info->isValid()) {
		id = isDefaultDir ? MENU_ID_DIR_CHANGE : MENU_ID_DIR_DEFAULT;
	}
	SetMenuDefaultItem((HMENU)mConfigDirOptionsMenu->GetHMenu(), id, FALSE);
	if (mConfigDirText->GetLabel() != ets2Info->getDirectory()) {
		setDirText(ets2Info->getDirectory(), DirId::CONFIG);
	}

	dir = wxFileName::DirName(ets2Info->getSteamDirectory());
	isDefaultDir = dir.SameAs(wxFileName::DirName(Ets2::Info::getDefaultDirectory(getGame(), true)));
	mSteamDirOptionOpen->Enable(dir.Exists());
	mSteamDirOptionDefault->Enable(!isDefaultDir);
	id = MENU_ID_DIR_OPEN;
	if (!ets2Info->isValid()) {
		id = isDefaultDir ? MENU_ID_DIR_CHANGE : MENU_ID_DIR_DEFAULT;
	}
	SetMenuDefaultItem((HMENU)mSteamDirOptionsMenu->GetHMenu(), id, FALSE);
	if (mSteamDirText->GetLabel() != ets2Info->getDirectory()) {
		setDirText(ets2Info->getSteamDirectory(), DirId::STEAM);
	}

	if (ets2Info->isValid()) {
		//if (ets2Info->getSaveFormat() == Ets2::Info::SaveFormat::BINARY) {
		if (ets2Info->getSaveFormat() == Ets2::Info::SaveFormat::TEXT) {
			mSaveFormatSeparator->Hide();
			mFixSaveFormatButton->Hide();
		} else {
			mSaveFormatSeparator->Show();
			mFixSaveFormatButton->Show();
		}
		mSaveFormatText->SetToolTip(L"");
		switch (ets2Info->getSaveFormat()) {
		case Ets2::Info::SaveFormat::BINARY:
			mSaveFormatText->SetLabel(L"Binary");
			//mStatusText->SetLabel(L"Settings OK", StatusText::Type::SUCCESS);
			mStatusText->SetLabel("The recommended save format is “Text”.", StatusText::Type::WARNING);
			break;
		case Ets2::Info::SaveFormat::TEXT:
			mSaveFormatText->SetLabel(L"Text");
			//mStatusText->SetLabel("The recommended save format is “Binary”.", StatusText::Type::WARNING);
			mStatusText->SetLabel(L"Settings OK", StatusText::Type::SUCCESS);
			break;
		case Ets2::Info::SaveFormat::BOTH:
			mSaveFormatText->SetLabel(L"Hybrid");
			//mStatusText->SetLabel("The recommended save format is “Binary”.", StatusText::Type::WARNING);
			mStatusText->SetLabel("The recommended save format is “Text”.", StatusText::Type::WARNING);
			break;
		case Ets2::Info::SaveFormat::NOT_FOUND:
			mSaveFormatText->SetLabel(L"Not found");
			mStatusText->SetLabel("The save format was not found in the settings file.", StatusText::Type::FAILURE);
			break;
		default:
			mStatusText->SetLabel("Unknown save format.", StatusText::Type::FAILURE);
			mSaveFormatText->SetLabel(wxString::Format(L"Unknown (%s)", ets2Info->getRawSaveFormat()));
			mSaveFormatText->SetToolTip(ets2Info->getRawSaveFormat());
		}
		mSaveFormatText->SetMaxSize(wxSize(mSaveFormatSizer->GetSize().x - mFixSaveFormatButton->GetSize().x - mSaveFormatSeparator->GetSize().x, -1));
		Layout();
	} else {
		mSaveFormatText->SetLabel(L"Not found");
		mSaveFormatSeparator->Hide();
		mFixSaveFormatButton->Hide();
		mStatusText->SetLabel(L"Game settings were not found in the directory above.", StatusText::Type::FAILURE);
		Layout();
	}
}

wxString GameSettingsPanel::getDirectory(DirId dirId)
{
	switch (dirId)
	{
		case DirId::STEAM:
			return mSteamDirText->GetLabel();
		default:
		case DirId::CONFIG:
			return mConfigDirText->GetLabel();
	}
}

void GameSettingsPanel::setDirText(const wxString& text, DirId dirId) {
	wxStaticText* dirText;
	wxBoxSizer* dirSizer;
	wxHyperlinkCtrl* dirOptionsButton;
	wxStaticText* dirSeparator;

	if (dirId == DirId::CONFIG)
	{
		dirText = mConfigDirText;
		dirSizer = mConfigDirSizer;
		dirOptionsButton = mConfigDirOptionsButton;
		dirSeparator = mConfigDirSeparator;
	}
	else
	{
		dirText = mSteamDirText;
		dirSizer = mSteamDirSizer;
		dirOptionsButton = mSteamDirOptionsButton;
		dirSeparator = mSteamDirSeparator;
	}
	dirText->SetLabel(text);
	int maxWidth = dirSizer->GetSize().x - dirOptionsButton->GetSize().x - dirSeparator->GetSize().x;
	if (dirText->GetSize().x > maxWidth) {
		dirText->SetToolTip(text);
	} else {
		dirText->SetToolTip(L"");
	}
	dirText->SetMaxSize(wxSize(maxWidth, -1));
	Layout();
}
