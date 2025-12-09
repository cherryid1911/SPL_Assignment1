#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    //Todo: Implement buildLibrary method

    int count_tracks=0;
     for (const auto& info : library_tracks) {
       
        if(info.type == "MP3"){
            MP3Track *track = new MP3Track(info.title, info.artists, info.duration_seconds, info.bpm, info.extra_param1, info.extra_param2);
            library.push_back(track);
            //std::cout << "MP3Track created: " << (*track).get_bitrate() << " kbps\n";
            count_tracks++;
        }

        else if (info.type == "WAV") {
            WAVTrack* track = new WAVTrack(info.title, info.artists, info.duration_seconds, info.bpm, info.extra_param1, info.extra_param2);
            library.push_back(track);
            //std::cout << "WAVTrack created: " << (*track).get_sample_rate() << " Hz/ " << (*track).get_bit_depth() << " bit\n";
            count_tracks++;
        }
    }
    std::cout << "[INFO] Track library built: " << count_tracks << " tracks loaded\n";
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here

    AudioTrack* found = playlist.find_track(track_title);
    if (found)
        return found;
    return nullptr;
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    // For now, add a placeholder to fix the linker error

     std::cout << "[INFO] Loading playlist: " << playlist_name << "\n";
     Playlist& playlist = getPlaylist();
     playlist = Playlist(playlist_name);

    int counted_tracks = 0;

     for (int i: track_indices){
        //int i = track_indices[k];
        if (i < 1 || i > library.size())
             std::cout << "[WARNING] Invalid track index: " << i << ".\n";
        else{
            PointerWrapper<AudioTrack> cloneWrapper = library[i-1] -> clone();

            if (!cloneWrapper) 
                  std::cerr << "[Error] track is empty.\n";
            else {
                
              AudioTrack* clonedTrack = cloneWrapper.release();
              clonedTrack->load();
              clonedTrack->analyze_beatgrid();
              playlist.add_track(clonedTrack);
              counted_tracks++;
              //std::cout << "Added " << (*clonedTrack).get_title() << " to playlist " << playlist_name << ".\n";
            }   
        }
     }
    std::cout << "[INFO] Playlist loaded: " << playlist_name <<" (" << counted_tracks << " tracks)\n";

    (void)playlist_name;  // Suppress unused parameter warning
    (void)track_indices;  // Suppress unused parameter warning
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here
    std::vector<std::string> track_titles;
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for (auto it = tracks.rbegin(); it != tracks.rend(); ++it) {
        AudioTrack* track = *it;
        if (track)
            track_titles.push_back(track -> get_title());
    }
    return track_titles;
}

/*
Desractor to handle memory
*/

DJLibraryService::~DJLibraryService() {
    for (AudioTrack* t : library) {
        delete t;
    }
    library.clear();
}
