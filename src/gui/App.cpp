#include "App.h"

#include <wx/srchctrl.h>
#include <wx/wrapsizer.h>

blazon::SearchWindow* blazon::g_frame = nullptr;

blazon::SearchWindow::SearchWindow() :
	wxFrame( nullptr, wxID_ANY, "Blazon Search", wxDefaultPosition, { 800, 500 }, wxDEFAULT_FRAME_STYLE )
{
	const int ID_TOOL_CREATE = 2;
	const int ID_TOOL_SEARCH_BAR = 3;
	const int ID_TOOL_SEARCH_BUTTON = 4;
	//const int ID_TOOL_QUICK_EXPORT = 2;
	//const int ID_TOOL_MERGE = 3;

	wxMenu* menuFile = new wxMenu();
	{
		menuFile->Append( ID_TOOL_CREATE, "&Create Entry...\tCtrl-N", "Create a new blazon entry" );
		Bind( wxEVT_MENU, &SearchWindow::OnCreateEntryWindow, this, ID_TOOL_CREATE );

		menuFile->AppendSeparator();

		menuFile->Append( wxID_EXIT );
		Bind( wxEVT_MENU, &SearchWindow::OnExit, this, wxID_EXIT );
	}

	wxMenuBar* menuBar = new wxMenuBar();
	{
		menuBar->Append( menuFile, "&File" );
		SetMenuBar( menuBar );
	}

	wxPanel* searchBarPanel = new wxPanel( m_panel );
	{
		m_searchBar = new wxTextCtrl( 
			searchBarPanel, ID_TOOL_SEARCH_BAR, 
			"", 
			wxDefaultPosition, wxDefaultSize, 
			wxTE_PROCESS_ENTER );
		m_searchBar->SetHint( "field:azure,crown:or:three" );

		wxButton* searchButton = new wxButton( searchBarPanel, ID_TOOL_SEARCH_BUTTON, "Search" );

		Bind( wxEVT_TEXT_ENTER, &SearchWindow::OnSearch, this, ID_TOOL_SEARCH_BAR );
		Bind( wxEVT_BUTTON, &SearchWindow::OnSearch, this, ID_TOOL_SEARCH_BUTTON );

		wxBoxSizer* searchBarPanelSizer = new wxBoxSizer( wxHORIZONTAL );
		searchBarPanelSizer->Add( m_searchBar, 9, wxEXPAND );
		searchBarPanelSizer->Add( searchButton, 2, wxEXPAND );
		searchBarPanel->SetSizer( searchBarPanelSizer );
	}

	wxPanel* searchResultsPanel = new wxPanel( m_panel );
	{
		m_resultListBox = new wxListBox( searchResultsPanel, -1);
		
		wxBoxSizer* searchResultPanelSizer = new wxBoxSizer( wxHORIZONTAL );
		searchResultPanelSizer->Add( m_resultListBox, 1, wxEXPAND );
		searchResultsPanel->SetSizer( searchResultPanelSizer );
	}

	// Set up the sizer for the panel
	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( searchBarPanel, 0, wxEXPAND );
	panelSizer->Add( searchResultsPanel, 1, wxEXPAND );
	m_panel->SetSizer( panelSizer );
	
	CreateStatusBar();
	SetStatusText( "" );

	// Set up blazon stuff

	m_blazonDB.loadIndexTable();
	
//	blazon::BlazonEntry testBlazon( "azure, three crowns or two over one", "field:azure,crown:or:three:2_over_1", "The Kingdom of Sweden lesser arms" );
//	db.createEntry( testBlazon );
//	db.cacheEntry( testBlazon );

//	db.generateIndexTable();

}

void blazon::SearchWindow::OnSearch( wxCommandEvent& event )
{
	SetStatusText( "Searching..." );

	std::string query = m_searchBar->GetValue().ToStdString();

	std::vector<std::string> results = m_blazonDB.queryIndices( blazon::makeTagListFromString( query ) );
	m_resultListBox->Clear();

	if ( results.size() > 0 )
	{
		wxArrayString items;
	
		for ( const std::string& r : results )
		{
			blazon::BlazonEntry entry = m_blazonDB.getEntry( r );
			items.Add( entry.blazon );
		}

		m_resultListBox->InsertItems( items, 0 );
	}

	SetStatusText( "Done" );
}
