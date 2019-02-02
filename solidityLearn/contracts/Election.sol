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

    function addCandidate (string memory _name) private {
        candidatesCount ++;
        candidates[candidatesCount] = Candidate(candidatesCount, _name, 0);
    }
    // This is in memory because it's just a temporary thing.  We would say storage if we wanted it written to the blockchain.
    // For this though, just a temporary parameter so we should be good 

    //Constructor
    constructor() public {
        addCandidate("Candidate 1");
        addCandidate("Candidate 2");
    }

}

// truffle console
// Election.deployed().then(function(i) {app = i})
// app.candidates()
    // That expects a key.  Pass it 1 or 2.
    // If you pass it something wrong, it gives a default value
// app.candidatesCount()
// app.candidates(1).then(function(c) {candidate = c;})
// candidate
// Then index it's stuff with [0] or ['id]
// And can do stuff like .toNumber() to get better data
// Explanation at 1:09:00 with connecting metamask and stuff