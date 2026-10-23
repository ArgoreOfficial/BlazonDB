#pragma once

#include <wxinclude.h>
#include <Blazon.h>

#include <filesystem>

namespace blazon {

class SearchWindow : public wxFrame
{
public:
	SearchWindow();

private:
	void OnAbout( wxCommandEvent& event ) {
		std::string version = "This is some about text";
		wxMessageBox( version, "About", wxOK | wxICON_INFORMATION );
	}

	void OnExit( wxCommandEvent& event ) {
		Close( true );
	}

	void OnSearch( wxCommandEvent& event );

	void OnCreateEntryWindow( wxCommandEvent& event ) {
		wxMessageBox( "PLACEHOLDER", "Create", wxOK | wxICON_INFORMATION );
	}

	wxPanel* m_panel = new wxPanel( this );
	blazon::BlazonDatabase m_blazonDB{};

	wxTextCtrl* m_searchBar = nullptr;
	wxListBox* m_resultListBox = nullptr;

	std::vector<std::string> m_searchResults;
};

extern SearchWindow* g_frame;

class Application : public wxApp
{
	bool OnInit() override {
		g_frame = new SearchWindow();
		return g_frame->Show();
	}
};

}
