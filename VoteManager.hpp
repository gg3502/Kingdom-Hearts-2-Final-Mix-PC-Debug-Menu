#pragma once
#include <vector>
#include <string>
#ifdef max
#undef max
#endif

class VoteManager {
public:
    void BeginVote(const std::vector<std::string>& options, int durationSeconds);
    void Update();
    void EndVote();
    void ForceStopVoting();

    bool IsVotingActive() const;
    bool IsVoteComplete() const;

    int GetWinningIndex() const;
    const std::vector<std::string>& GetOptions() const;

    // For local testing: increment vote for option
    void CastVote(int optionIndex);

   
    const std::vector<int>& GetVoteCounts() const { return votes; }
    const std::vector<std::string>& GetCurrentOptions() const { return options; }
    int GetSecondsRemaining() const { return std::max(0, duration - static_cast<int>(elapsedTime)); }

    int GetVoteDuration() const { return duration; }

private:
    std::vector<std::string> options;
    std::vector<int> votes;
    bool votingActive = false;
    int duration = 0;  // in seconds
    float elapsedTime = 0.0f;
    bool voteComplete = false;
};

extern VoteManager voteManager;
