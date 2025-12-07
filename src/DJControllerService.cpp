#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    std::string tr_title= track.get_title();
    if (cache.contains(tr_title)){
        cache.get(tr_title);
        return 1;
    }
    PointerWrapper<AudioTrack> cloned = track.clone();
    if (!cloned){
        std::cerr << "[ERROR] Track: \"" << track.get_title()
        << "\" failed to clone";
        return 0;
    }
    cloned->load();
    cloned->analyze_beatgrid();
    int result= cache.put(std::move(cloned));
    if(result){
        return -1;
    }
    return 0;
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    AudioTrack* at = cache.get(track_title);
    if (!at){
        return nullptr;
    }
    return at;
}
