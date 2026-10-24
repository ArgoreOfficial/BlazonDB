#include "App.h"

#include <wx/srchctrl.h>
#include <wx/textcompleter.h>

blazon::SearchWindow* blazon::g_frame = nullptr;

blazon::SearchWindow::SearchWindow() :
	wxFrame( nullptr, wxID_ANY, "Search Blazons", wxDefaultPosition, { 700, 400 }, wxDEFAULT_FRAME_STYLE )
{
	const int ID_TOOL_CREATE = 2;
	const int ID_TOOL_SEARCH_BAR = 3;
	const int ID_TOOL_SEARCH_BUTTON = 4;
	const int ID_TOOL_SEARCH_LIST = 5;
	
	SetMinSize( { 700, 400 } );

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

	// Panels

	wxPanel* searchContainerPanel = new wxPanel( m_panel );
	{
		wxPanel* searchBarPanel = new wxPanel( searchContainerPanel );
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

		wxPanel* searchResultsPanel = new wxPanel( searchContainerPanel );
		{
			m_resultListBox = new wxListBox( searchResultsPanel, ID_TOOL_SEARCH_LIST );
		
			wxBoxSizer* searchResultPanelSizer = new wxBoxSizer( wxHORIZONTAL );
			searchResultPanelSizer->Add( m_resultListBox, 1, wxEXPAND );
			searchResultsPanel->SetSizer( searchResultPanelSizer );

			Bind( wxEVT_LISTBOX, &SearchWindow::OnSearchSelected, this, ID_TOOL_SEARCH_LIST );
		}

		wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
		panelSizer->Add( searchBarPanel, 0, wxEXPAND );
		panelSizer->Add( searchResultsPanel, 1, wxEXPAND );
		searchContainerPanel->SetSizer( panelSizer );
	}

	wxPanel* entryContainerPanel = new wxPanel( m_panel );
	{
		m_entryHeader = new wxStaticText( entryContainerPanel, wxID_ANY, "" );
		m_entryBlazon = new wxTextCtrl( entryContainerPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE );
		m_entryTags   = new wxTextCtrl( entryContainerPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_CHARWRAP );
		m_entryDesc   = new wxTextCtrl( entryContainerPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_AUTO_URL );

		m_entryBlazon->SetMinSize( { 10, 10 } );
		m_entryTags->SetMinSize( { 10, 10 } );
		m_entryDesc->SetMinSize( { 10, 10 } );

		m_entryBlazon->SetHint( "Blazon appears here" );
		m_entryTags->SetHint( "Tags appears here" );

		wxBoxSizer* entryPanelSizer = new wxBoxSizer( wxVERTICAL );
		entryPanelSizer->Add( m_entryHeader, 0, wxEXPAND );
		entryPanelSizer->Add( m_entryBlazon, 2, wxEXPAND );
		entryPanelSizer->Add( m_entryTags, 3, wxEXPAND );
		entryPanelSizer->Add( m_entryDesc, 5, wxEXPAND );
		entryContainerPanel->SetSizer( entryPanelSizer );
	}

	wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
	panelSizer->Add( searchContainerPanel, 7, wxEXPAND );
	panelSizer->Add( entryContainerPanel, 3, wxEXPAND );
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

	m_searchResults = m_blazonDB.queryIndices( blazon::makeTagListFromString( query ) );
	m_resultListBox->Clear();

	if ( m_searchResults.size() > 0 )
	{
		wxArrayString items;

		for ( const std::string& r : m_searchResults )
		{
			blazon::BlazonEntry entry = m_blazonDB.getEntry( r );
			items.Add( entry.blazon );
		}

		m_resultListBox->InsertItems( items, 0 );
	}

	SetStatusText( "Done" );
}

void blazon::SearchWindow::OnCreateEntryWindow( wxCommandEvent& event )
{
	if ( m_createWindow != nullptr )
		return;
	
	m_createWindow = new CreateWindow( this, &m_createWindow );
	m_createWindow->Show();
}

void blazon::SearchWindow::OnSearchSelected( wxCommandEvent& _event )
{
	size_t selected = _event.GetSelection();
	blazon::BlazonEntry entry = m_blazonDB.getEntry( m_searchResults[ selected ] );
	m_entryHeader->SetLabel( m_searchResults[ selected ] );
	m_entryBlazon->SetValue( entry.blazon );
	m_entryTags->SetValue( "TEMPORARY" );
	m_entryDesc->SetValue( entry.description );
}

blazon::CreateWindow::CreateWindow( wxWindow* _parent, CreateWindow** _selfPtr ) :
	wxFrame( _parent, wxID_ANY, "Create Blazon", wxDefaultPosition, { 400, 600 }, wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN )
{
	Bind( wxEVT_CLOSE_WINDOW, &CreateWindow::OnClose, this );
	m_selfPtr = _selfPtr;

	m_entryBlazon = new wxTextCtrl( m_panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_entryTags = new wxTextCtrl( m_panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_CHARWRAP);
	m_entryDesc = new wxTextCtrl( m_panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_AUTO_URL );
	
	m_autoCompletePopup = new wxPopupTransientWindow( m_entryTags );
	m_autoCompleteListBox = new wxListBox( m_autoCompletePopup, wxID_ANY );
	
	{
		wxBoxSizer* autoCompleteSizer = new wxBoxSizer( wxVERTICAL );
		autoCompleteSizer->Add( m_autoCompleteListBox, 1, wxEXPAND );
		m_autoCompletePopup->SetSizer( autoCompleteSizer );
	}

	m_autoCompletePopup->Hide();

	Bind( wxEVT_TEXT, &CreateWindow::OnTagTextChanged, this, m_entryTags->GetId() );

	wxButton* createAndImport = new wxButton( m_panel, wxID_ANY, "Create and Import" );
	Bind( wxEVT_BUTTON, &CreateWindow::OnCreateAndImport, this, createAndImport->GetId() );

	m_entryBlazon->SetMinSize( { 10, 10 } );
	m_entryTags->SetMinSize( { 10, 10 } );
	m_entryDesc->SetMinSize( { 10, 10 } );

	m_entryBlazon->SetHint( "Blazon appears here" );
	m_entryTags->SetHint( "Tags appears here" );

	wxBoxSizer* entryPanelSizer = new wxBoxSizer( wxVERTICAL );
	entryPanelSizer->Add( m_entryBlazon, 2, wxEXPAND );
	entryPanelSizer->Add( m_entryTags, 3, wxEXPAND );
	entryPanelSizer->Add( m_entryDesc, 5, wxEXPAND );
	entryPanelSizer->Add( createAndImport, 0, wxEXPAND );
	m_panel->SetSizer( entryPanelSizer );
}

void blazon::CreateWindow::OnTagTextChanged( wxCommandEvent& event )
{
	long pos = m_entryTags->GetInsertionPoint();
	wxPoint point = m_entryTags->PositionToCoords( pos );
	wxPoint screenPoint = m_entryTags->GetScreenPosition();

	int fontsize = m_entryTags->GetFont().GetPixelSize().y;
	m_autoCompletePopup->SetPosition( 
		{ 
			screenPoint.x,
			point.y + screenPoint.y + fontsize + 2 // magic number alert
		} );

	std::string fulltext = m_entryTags->GetValue().ToStdString();

	size_t lastTagDiv = fulltext.find_last_of( "," );
	size_t lastModDiv = fulltext.find_last_of( ":" );
	std::string display = "";

	const bool isOnTag = lastModDiv == std::string::npos || ( lastTagDiv != std::string::npos && lastTagDiv > lastModDiv );
	
	std::vector<std::string> autoCompleteList{ };

	if ( isOnTag )
	{
		std::string current = fulltext.substr( lastTagDiv + 1 );
		autoCompleteList = blazon::getAutocompleteListTag( current );
	}
	else
	{
		std::string current = fulltext.substr( lastModDiv + 1 );
		autoCompleteList = blazon::getAutocompleteListModifier( current );
	}

	m_autoCompleteListBox->Clear();

	if ( autoCompleteList.empty() )
	{
		m_autoCompletePopup->Hide();
		return;
	}

	std::sort( autoCompleteList.begin(), autoCompleteList.end() );

	wxArrayString array{};
	for ( const std::string& s : autoCompleteList )
		array.Add( s );
	
	m_autoCompleteListBox->InsertItems( array, 0 );

	m_autoCompletePopup->Fit();
	m_autoCompletePopup->Show();
}

void blazon::CreateWindow::OnCreateAndImport( wxCommandEvent& event )
{
	blazon::BlazonEntry newEntry( m_entryBlazon->GetValue().ToStdString(), m_entryTags->GetValue().ToStdString(), m_entryDesc->GetValue().ToStdString() );
	SearchWindow* sw = (SearchWindow*)GetParent();
	sw->m_blazonDB.createEntry( newEntry );
	sw->m_blazonDB.cacheEntry( newEntry );
}
