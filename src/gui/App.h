#pragma once

#include <wxinclude.h>
#include <wx/popupwin.h>

#include <Blazon.h>
#include <filesystem>

namespace blazon {

class BlazonTagTextCtrl : public wxTextCtrl
{
public:
	BlazonTagTextCtrl( wxWindow* parent,
					   wxWindowID id,
					   const wxString& value = wxEmptyString,
					   const wxPoint& pos = wxDefaultPosition,
					   const wxSize& size = wxDefaultSize,
					   long style = 0,
					   const wxValidator& validator = wxDefaultValidator,
					   const wxString& name = wxASCII_STR( wxTextCtrlNameStr ) );
	void OnTextChanged( wxCommandEvent& event );

	wxPopupTransientWindow* m_autoCompletePopup = nullptr;
	wxListBox* m_autoCompleteListBox = nullptr;
};

class CreateWindow : public wxFrame
{
public:
	CreateWindow( wxWindow* _parent, CreateWindow** _selfPtr );

private:
	wxPanel* m_panel = new wxPanel( this );

	wxTextCtrl* m_entryBlazon = nullptr;
	BlazonTagTextCtrl* m_entryTags = nullptr;
	wxTextCtrl* m_entryDesc = nullptr;
	
	CreateWindow** m_selfPtr;

	void OnClose( wxCloseEvent& event ) {
		if ( m_selfPtr )
			*m_selfPtr = nullptr;
		Destroy();
	}

	void OnCreateAndImport( wxCommandEvent& event );
};

class SearchWindow : public wxFrame
{
public:
	SearchWindow();
	blazon::BlazonDatabase m_blazonDB{};

private:
	void OnAbout( wxCommandEvent& event ) {
		std::string version = "This is some about text";
		wxMessageBox( version, "About", wxOK | wxICON_INFORMATION );
	}

	void OnExit( wxCommandEvent& event ) {
		Close( true );
	}

	void OnSearch( wxCommandEvent& event );
	void OnCreateEntryWindow( wxCommandEvent& event );
	void OnSearchSelected( wxCommandEvent& _event );

	wxPanel* m_panel = new wxPanel( this );
	
	BlazonTagTextCtrl* m_searchBar = nullptr;
	wxListBox* m_resultListBox = nullptr;

	wxStaticText* m_entryHeader = nullptr;
	wxTextCtrl*   m_entryBlazon = nullptr;
	wxTextCtrl*   m_entryTags   = nullptr;
	wxTextCtrl*   m_entryDesc   = nullptr;

	std::vector<std::string> m_searchResults;

	CreateWindow* m_createWindow = nullptr;
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
