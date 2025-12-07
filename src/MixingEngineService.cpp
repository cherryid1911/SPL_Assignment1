#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks{nullptr, nullptr}, active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    std::cout<<"[MixingEngineService] Initialized with 2 empty decks.\n";
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std::cout<<"[MixingEngineService] Cleaning up decks....\n";
    for (int i=0;i<2;i++){
        delete decks[i];
        decks[i]=nullptr;
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout<<"\n=== Loading Track to Deck ===\n";
    PointerWrapper<AudioTrack> tr = track.clone(); 
    if (!tr){
        std::cerr<<"[ERROR] Track: \""<<track.get_title()<<"\" failed to clone\n";
        return -1;
    }
    int target_deck;
    int prev = active_deck;
    if (!decks[0] && !decks[1]){
        target_deck = 0;
    }
    else{
        target_deck = 1-prev;
    }
    std::cout<<"[Deck Switch] Target deck: "<< target_deck<<"\n";
    if (decks[target_deck]){
        delete decks[target_deck];
        decks[target_deck]=nullptr;
    }
    tr->load();
    tr->analyze_beatgrid();
    
    if (decks[prev] && auto_sync){
        int diff = std::abs(tr->get_bpm() - decks[active_deck]->get_bpm());
        if (diff>bpm_tolerance){
            sync_bpm(tr);
        }
    }
    AudioTrack* at = tr.release();
    decks[target_deck]= at;
    std::cout<<"[Load Complete] \"" <<at->get_title() 
                <<"\" is now loaded on deck " << target_deck<< "\n";
    if (decks[prev] && prev!=active_deck){
        std::cout<<"[Unload] Unloading previous deck "<<
            prev<< "("<<decks[prev]->get_title()<<")\n";
        delete decks[prev];
        decks[prev]=nullptr;
    }
    active_deck=target_deck;
    std::cout<<"[Active Deck] Switched to deck "<<active_deck<<"\n";
    return target_deck;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if (!decks[active_deck] || !track){
        return false;
    }
    int diff = std::abs(track->get_bpm()-decks[active_deck]->get_bpm());
    return diff<=bpm_tolerance;
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if (!decks[active_deck] || !track){
        return;
    }
    int deck_bpm= decks[active_deck]->get_bpm();
    int original_bpm = track->get_bpm();
    int new_bpm = (deck_bpm + original_bpm) / 2;
    track->set_bpm(new_bpm);
    std::cout<<"[Sync BPM] Syncing BPM from " << original_bpm << " to " << new_bpm << std::endl;
}