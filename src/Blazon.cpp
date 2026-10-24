#include "Blazon.h"

#include <sstream>
#include <fstream>
#include <math.h>
#include "hash.h"

#include <util.h>

blazon::BlazonEntry::BlazonEntry( const std::string& _blazon, const std::string& _tags, const std::string& _description )
{
	blazon = _blazon;
	tags = makeTagListFromString( _tags );
	description = _description;

	for ( const Tag& tag : tags )
		tagBitset.set( tag.index, true );

	std::stringstream ss;
	ss << std::hex << tagBitset.to_ullong();
	tagsHash = ss.str();

	generateEntryID();
}

blazon::BlazonEntry::BlazonEntry( const std::string& _blazon, const std::vector<Tag>& _tags, const std::string& _description )
{
	blazon = _blazon;
	tags = _tags;
	description = _description;

	for ( const Tag& tag : _tags )
		tagBitset.set( tag.index, true );

	std::stringstream ss;
	ss << std::hex << tagBitset.to_ullong();
	tagsHash = ss.str();

	generateEntryID();
}

bool blazon::BlazonEntry::matchesTags( const std::vector<Tag>& _tags, bool _skipBitset ) const
{
	if ( !_skipBitset )
	{
		for ( const auto& tag : _tags )
		{
			if ( !tagBitset.test( tag.index ) )
				return false;
		}
	}

	for ( const auto& searchTag : _tags )
	{
		for ( const auto& cardTag : tags )
		{
			if ( cardTag.index != searchTag.index ) // not the same tag
				continue;

			if ( !doesTagsMatch( cardTag, searchTag ) )
				return false;

			break; // tags match, continue to next
		}
	}

	return true;
}

blazon::BlazonDatabase::BlazonDatabase()
{
	std::filesystem::path dbPath = getEntryPath("asd").parent_path();
	if ( !std::filesystem::exists( dbPath ) )
		std::filesystem::create_directories( dbPath );
}

size_t blazon::BlazonDatabase::loadEntry( const std::string& _entryID )
{
	std::filesystem::path path = getEntryPath( _entryID );
	std::ifstream file( path );
	if ( !file )
		return -1;

	entries.push_back( nlohmann::json::parse( file ) );
	return entries.size() - 1;
}

blazon::BlazonEntry blazon::BlazonDatabase::getEntry( const std::string& _entryID )
{
	// TODO
	for ( size_t i = 0; i < entries.size(); i++ )
	{
		if ( entries[ i ].entryID == _entryID )
			return entries[ i ];
	}

	size_t index = loadEntry( _entryID );
	if ( index != -1 )
		return entries[ index ];

	return {};
}

void blazon::BlazonDatabase::cacheEntry( const BlazonEntry& _entry )
{
	std::filesystem::path path = getEntryPath( _entry.entryID );
	std::ofstream file( path );
	file << nlohmann::json( _entry ).dump( 4 );
}

std::vector<std::string> blazon::BlazonDatabase::query( const std::vector<Tag>& _searchTags )
{
	std::vector<std::string> results;

	for ( size_t i = 0; i < entries.size(); i++ )
	{
		const BlazonEntry& card = entries[ i ];
		if ( card.matchesTags( _searchTags ) )
			results.push_back( card.entryID );
	}

	return results;
}

std::vector<std::string> blazon::BlazonDatabase::queryIndices( const std::vector<Tag>& _searchTags )
{
	TagBitset queryBitset = makeBitsetFromTags( _searchTags );
	std::vector<std::string> presearchEntries{};
	std::vector<std::string> results{};

	for ( size_t i = 0; i < entryIndices.size(); i++ )
	{
		if ( ( queryBitset & entryIndices[ i ].getBitset() ) == queryBitset )
			presearchEntries.push_back( entryIndices[ i ].getEntryName() );
	}
	
	for ( size_t i = 0; i < presearchEntries.size(); i++ )
	{
		const std::string& entryName = presearchEntries[ i ];

		const BlazonEntry& card = getEntry( entryName );
		if ( card.matchesTags( _searchTags ) )
			results.push_back( entryName );
	}
	
	return results;
}

void blazon::BlazonDatabase::generateIndexTable()
{
	std::ofstream file( getEntryIndexPath(), std::ios::binary);
	size_t size = entryIndices.size();
	
	file.write( (char*)&size, sizeof( size_t ) );
	file.write( (char*)entryIndices.data(), sizeof( EntryIndex ) * entryIndices.size() );

	file.close();
}

void blazon::BlazonDatabase::loadIndexTable()
{
	std::ifstream file( getEntryIndexPath(), std::ios::binary);
	if ( !file )
		return;

	size_t size;
	
	file.read( (char*)&size, sizeof( size_t ) );
	entryIndices.resize( size );

	file.read( (char*)entryIndices.data(), sizeof( EntryIndex ) * size );

	file.close();
}

bool blazon::isModifierTincture( const std::string& _modifier )
{
	for ( const auto& t : CONST_TINCTURES )
	{
		if ( t != _modifier )
			continue;
		return true;
	}

	return false;
}

size_t blazon::getModifierIndex( const std::string& _modifier )
{
	for ( size_t i = 0; i < CONST_MODIFIERS.size(); i++ )
	{
		if ( CONST_MODIFIERS[ i ] != _modifier )
			continue;

		return i;
	}

	return -1;
}

blazon::Tag blazon::makeTag( const std::string& _tagName, const std::vector<std::string>& _modifierNames )
{
	Tag tag{};
	
	tag.hasFaultyIndex = true;
	for ( size_t i = 0; i < CONST_TAGS.size(); i++ )
	{
		if ( _tagName != CONST_TAGS[ i ] )
			continue;

		tag.index = i;
		tag.hasFaultyIndex = false;
		break;
	}

	if ( tag.index == -1 )
		tag.hasFaultyIndex = true;

	std::vector<std::string> tinctureTags;

	// build modifier mask
	for ( const std::string& modifierName : _modifierNames )
	{
		if ( isModifierTincture( modifierName ) )
		{
			tinctureTags.push_back( modifierName );
		}
		else
		{
			size_t index = getModifierIndex( modifierName );

			if ( index != (size_t)-1 ) // nonexistent modifiers are ignored
				tag.modifiersBitset.set( index, true );
			else
				tag.hasFaultyModifier = true;
		}
	}

	if ( !tinctureTags.empty() )
	{
		std::string currentTinctureCombo = "";
		for ( size_t i = 0; i < tinctureTags.size(); i++ )
		{
			currentTinctureCombo += tinctureTags[ i ];
			uint32_t hash = wv::Hash::djb2( currentTinctureCombo );
			tag.tinctureHashes.push_back( hash );
		}
	}

	return tag;
}

blazon::Tag blazon::makeTagFromString( const std::string& _string )
{
	std::stringstream sstream( _string );
	std::string tempSegment;

	std::string tagString;
	std::vector<std::string> modifiers;

	if ( std::getline( sstream, tempSegment, ':' ) )
		tagString = tempSegment;

	while ( std::getline( sstream, tempSegment, ':' ) )
		modifiers.push_back( tempSegment );

	Tag tag = makeTag( tagString, modifiers );
	return tag;
}

blazon::TagBitset blazon::makeBitsetFromTags( const std::vector<Tag>& _tags )
{
	TagBitset bitset{};
	for ( const Tag& tag : _tags )
		bitset.set( tag.index, true );
	return bitset;
}

std::vector<blazon::Tag> blazon::makeTagListFromString( const std::string& _string )
{
	std::stringstream sstream( _string );
	std::string tempSegment;
	std::vector<std::string> tagStrings;

	while ( std::getline( sstream, tempSegment, ',' ) )
		tagStrings.push_back( tempSegment );

	std::vector<Tag> tags;
	for ( const auto& str : tagStrings )
		tags.push_back( makeTagFromString( str ) );

	return tags;
}

const bool blazon::doesTagsMatch( const Tag& _entry, const Tag& _query )
{
	// check tag index
	if ( _entry.index != _query.index )
		return false;

	// check modifiers
	if ( ( _entry.modifiersBitset & _query.modifiersBitset ) != _query.modifiersBitset )
		return false;

	// check tinctures, this only fails if two hashes of the same depth are different
	// meaning [azure:or] will match with both [azure] and [azure:or] but not [azure:argent]

	// if the query has MORE tinctures than the entry, then we know it can never match
	if ( _query.tinctureHashes.size() > _entry.tinctureHashes.size() )
		return false;

	for ( size_t i = 0; i < _query.tinctureHashes.size(); i++ )
	{
		if ( _entry.tinctureHashes[ i ] != _query.tinctureHashes[ i ] )
			return false;
	}

	return true;
}

std::filesystem::path blazon::getEntryPath( const std::string& _entryID )
{
	std::filesystem::path path = Util::getFolderPath( FOLDERID_Documents ) / "BlazonDB" / "cache";
	path = path / _entryID;
	path = path.replace_extension( ".json" );
	return path;
}

std::filesystem::path blazon::getEntryIndexPath()
{
	std::filesystem::path path = Util::getFolderPath( FOLDERID_Documents ) / "BlazonDB" / "entry_index";
	path = path.replace_extension( ".dat" );
	return path;
}

std::vector<std::string> blazon::getAutocompleteList( const std::string& _string, const std::vector<std::string>& _options )
{
	if ( _string.empty() )
		return _options;

	std::vector<std::string> res;
	for ( auto& t : _options )
	{
		if ( t.find( _string ) == 0 )
			res.push_back( t );
	}
	return res;
}

std::vector<std::string> blazon::getAutocompleteListTag( const std::string& _string )
{
	return getAutocompleteList( _string, CONST_TAGS );
}

std::vector<std::string> blazon::getAutocompleteListModifier( const std::string& _string )
{
	std::vector<std::string> res{};

	auto m = getAutocompleteList( _string, CONST_MODIFIERS );
	auto t = getAutocompleteList( _string, CONST_TINCTURES );

	res.insert( res.end(), m.begin(), m.end() );
	res.insert( res.end(), t.begin(), t.end() );

	return res;
}

bool blazon::validateTagListString( const std::string& _tagListStr )
{
	std::stringstream sstream( _tagListStr );
	std::string tempSegment;
	std::vector<std::string> tagStrings;

	while ( std::getline( sstream, tempSegment, ',' ) )
		tagStrings.push_back( tempSegment );

	for ( const auto& str : tagStrings )
	{
		Tag t = makeTagFromString( str );
		if ( t.hasFaultyIndex || t.hasFaultyModifier )
			return false;
	}
		
	return true;
}

void blazon::to_json( nlohmann::json& _json, const blazon::Tag& _p )
{
	std::vector<int> tagModifierIndices{};

	for ( size_t i = 0; i < _p.modifiersBitset.size(); i++ )
	{
		if ( _p.modifiersBitset.test( i ) )
			tagModifierIndices.push_back( i );
	}

	_json = nlohmann::json{
		{ "tagIndex", _p.index },
		{ "modifiers", tagModifierIndices },
		{ "tinctureHashes", _p.tinctureHashes }
	};
}

void blazon::from_json( const nlohmann::json& _json, blazon::Tag& _p )
{
	std::vector<int> tagModifierIndices{};

	_json.at( "tagIndex" ).get_to( _p.index );
	_json.at( "modifiers" ).get_to( tagModifierIndices );
	_json.at( "tinctureHashes" ).get_to( _p.tinctureHashes );

	for ( size_t i = 0; i < tagModifierIndices.size(); i++ )
		_p.modifiersBitset.set( tagModifierIndices[ i ] );
}

void blazon::to_json( nlohmann::json& _json, const blazon::BlazonEntry& _p )
{
	_json = {
		{ "id", _p.entryID },
		{ "blazon", _p.blazon },
		{ "description", _p.description },
		{ "hash", _p.tagsHash },
		{ "tags", _p.tags }
	};
}

void blazon::from_json( const nlohmann::json& _json, blazon::BlazonEntry& _p )
{
	_json.at( "id" ).get_to( _p.entryID );
	_json.at( "blazon" ).get_to( _p.blazon );
	_json.at( "description" ).get_to( _p.description );
	_json.at( "hash" ).get_to( _p.tagsHash );
	_json.at( "tags" ).get_to( _p.tags );

	std::stringstream ss(_p.tagsHash);
	unsigned long long v;
	ss >> std::hex >> v;
	
	_p.tagBitset = v;
}