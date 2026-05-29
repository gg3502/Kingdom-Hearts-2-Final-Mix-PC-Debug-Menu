#include "VoteManager.hpp"
#include <algorithm>
#include <iostream>
#include <imgui.h> // For ImGui::GetIO().DeltaTime

void VoteManager::BeginVote(const std::vector<std::string>& opts, int durationSeconds) {
    options = opts;
    votes.clear();
    votes.resize(options.size(), 0);
    duration = durationSeconds;
    elapsedTime = 0.0f;
    votingActive = true;
    voteComplete = false;

    std::cout << "[VoteManager] Vote started! Options:\n";
    for (size_t i = 0; i < options.size(); i++)
        std::cout << i << ": " << options[i] << "\n";
}

void VoteManager::Update() {
    if (!votingActive)
        return;

    elapsedTime += ImGui::GetIO().DeltaTime;

    if (elapsedTime >= duration) {
        voteComplete = true;
    }
}

void VoteManager::EndVote() {
    votingActive = false;
}

void VoteManager::ForceStopVoting() {
    votingActive = false;
    voteComplete = false; // <--- Important: not complete, just canceled
    options.clear();
    votes.clear();
    duration = 0;
    elapsedTime = 0.0f;
}

bool VoteManager::IsVotingActive() const {
    return votingActive;
}

bool VoteManager::IsVoteComplete() const {
    return voteComplete;
}

int VoteManager::GetWinningIndex() const {
    if (votes.empty()) return -1;

    // Return index with max votes, break ties by first max
    int maxVotes = *std::max_element(votes.begin(), votes.end());

    if (maxVotes == 0) {
        // No votes cast
        return -1;
    }

    auto it = std::find(votes.begin(), votes.end(), maxVotes);
    return static_cast<int>(std::distance(votes.begin(), it));
}

const std::vector<std::string>& VoteManager::GetOptions() const {
    return options;
}

void VoteManager::CastVote(int optionIndex) {
    if (!votingActive || optionIndex < 0 || optionIndex >= static_cast<int>(votes.size()))
        return;

    votes[optionIndex]++;
    std::cout << "[VoteManager] Vote cast for option " << optionIndex << ": " << options[optionIndex] << "\n";
}
