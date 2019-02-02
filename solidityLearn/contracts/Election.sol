pragma solidity 0.5.0;
// Welp this tutorial is using an old version of solidity and lots of things are different!  This'll be fun

contract Election {
    struct Candidate {
        uint id;
        string name;
        uint voteCount;
    }
    //State variables allow us to write data to the blockchain
    // It's public so solidity gives us a built-in getter

    // Read/write candidate
    mapping(uint => Candidate) public candidates;
    // Id based look up for each candidate?
    // This writes data to the blockchain any time we assign a new key-value pair to it

    uint public candidatesCount;
    // You CAN'T iterate over a mapping, so you need to keep track externally

    function addCandidate (string _name) private {
        candidatesCount ++;
        candidates[candidatesCount] = Candidate(candidatesCount, _name, 0);
    }

    //Constructor
    constructor() public {
        addCandidate("Candidate 1");
        addCandidate("Candidate 2");
    }
    
}